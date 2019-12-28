#include "WidgetDuplicateSearch.h"
#include "ui_WidgetDuplicateSearch.h"
#include "CommonFunctions.h"

#include <QDebug>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QtConcurrent/QtConcurrent>

QString KEY_DIRECTORY = "Directory";

WidgetDuplicateSearch::WidgetDuplicateSearch( QWidget * parent, QSettings & p_settings )
    : QWidget( parent )
    , m_settings( p_settings )
    , ui( new Ui::WidgetDuplicateSearch )
{
    ui->setupUi( this );

    ui->lineEdit_directory         ->setText( m_settings.value( KEY_DIRECTORY           ).toString() );

    connect( this, &WidgetDuplicateSearch::signal_numberOfFiles,    this, &WidgetDuplicateSearch::slot_receiveNumberOfFiles    );
    connect( this, &WidgetDuplicateSearch::signal_analysisProgress, this, &WidgetDuplicateSearch::slot_receiveAnalysisProgress );

    connect( &m_watcherAnalysis, SIGNAL( finished() ), this, SLOT( slot_analysisFinished() ) );

    connect( &m_watcherAnalysis, SIGNAL( progressRangeChanged( int, int ) ), ui->progressBar_analysis, SLOT( setRange( int, int ) ) );
    connect( &m_watcherAnalysis, SIGNAL( progressValueChanged( int ) ),      ui->progressBar_analysis, SLOT( setValue( int ) ) );
}

WidgetDuplicateSearch::~WidgetDuplicateSearch()
{
    delete ui;
}


void WidgetDuplicateSearch::startAnalysis_thread_not_used()
{
    // 1. Collection of FilePaths
    QStringList listFilePaths;
    QDirIterator iter( ui->lineEdit_directory->text(),
                       QStringList() << "*.jpg",
                       QDir::Files,
                       QDirIterator::Subdirectories );
    while( iter.hasNext() )
    {
        listFilePaths.append( iter.next() );
    }

    // 2. Group by file size
    std::map<qint64, QList<QString>> mapSize_FilePaths;
    for( const QString & sFilePath : listFilePaths )
    {
        QFile file( sFilePath );
        if( file.open( QIODevice::ReadOnly ) )
        {
            mapSize_FilePaths[ file.size() ].append( sFilePath );
        }
    }

    // 3. Statistic of NbFiles for each Multiplicity
    std::map<int, int> mapMultiplicity_NbFiles;
    for( const auto & pairSize_FilePaths : mapSize_FilePaths )
    {
        const QList<QString> & listFilePaths = pairSize_FilePaths.second;
        mapMultiplicity_NbFiles[ listFilePaths.size() ] += listFilePaths.size();
    }

    for( const auto & pairMultiplicity_NbFiles : mapMultiplicity_NbFiles )
    {
        qDebug() << "Multiplicity" << pairMultiplicity_NbFiles.first << "NbFiles" << pairMultiplicity_NbFiles.second;
    }

    // 4. Calculate Hash of File Beginning for SizeGroups of Multiplicity > 1
    std::map<qint64, std::map<QString, QList<QString>>> mapSize_MapHash_FilePaths;
    for( const auto & pairSize_FilePaths : mapSize_FilePaths )
    {
        qint64                 nSize         = pairSize_FilePaths.first;
        const QList<QString> & listFilePaths = pairSize_FilePaths.second;

        if( listFilePaths.size() > 1 )
        {
            for( const QString & sFilePath : listFilePaths )
            {
                QFile file( sFilePath );
                if( file.open( QIODevice::ReadOnly ) )
                {
                    QByteArray baBegin = file.read( ui->spinBox_nbFirstBytes->value() ); // min 260 bytes

                    QString sHash( QCryptographicHash::hash( baBegin, QCryptographicHash::Md5 ).toHex() );
                    mapSize_MapHash_FilePaths[ nSize ][ sHash ].append( sFilePath );

//                    QCryptographicHash hash( QCryptographicHash::Md5 );
//                    if( hash.addData( file..addData( &file ) )
//                    {
//                        QString sHash( hash.result().toHex() );
//                        mapSize_MapHash_FilePaths[ nSize ][ sHash ].append( sFilePath );
//                    }
                }
            }
        }
    }

    // statistics
    for( const auto & pairSize_MapHash_FilePath : mapSize_MapHash_FilePaths )
    {
        qint64                                    nSize             = pairSize_MapHash_FilePath.first;
        const std::map<QString, QList<QString>> & mapHash_FilePaths = pairSize_MapHash_FilePath.second;

        for( const auto & pairHash_FilePaths : mapHash_FilePaths )
        {
            const QString     & sHash         = pairHash_FilePaths.first;
            const QStringList & listFilePaths = pairHash_FilePaths.second;

            if( listFilePaths.size() > 1 )
            {
                QString sOutput;
                QTextStream ts( &sOutput );
                ts << "Size: " << nSize << ", Hash: " << sHash << ", NbFilePaths: " << listFilePaths.size() << "\n";
                for( const QString & sFilePath : listFilePaths )
                {
                    ts << sFilePath << "\n";
                }
            }
        }
    }





//    emit signal_numberOfFiles( listFilePaths.size() );

//    for( int i = 0; i < listFilePaths.size(); ++i )
//    {
//        const QString & sFilePath = listFilePaths[i];

//        MediaFileData fileData( sFilePath );
//        mapHash_ListMedias[ fileData.hash_md5 ].append( fileData );

//        emit signal_analysisProgress( 100.0 * (i+1)/listFilePaths.size());
//    }

//    for( const QList<MediaFileData> & listMedia : mapHash_ListMedias )
//    {
//        if( listMedia.count() >= 2 )
//        {
//            qDebug() << "Duplicate Files:";
//            for( const MediaFileData & fileData : listMedia )
//            {
//                qDebug() << fileData.filePath;
//            }
//        }
    //    }
}

void WidgetDuplicateSearch::on_pushButton_chooseDirectory_clicked()
{
    QString sDirectory = QFileDialog::getExistingDirectory( this, "Select Directory",
                                                            ui->lineEdit_directory->text(),
                                                            QFileDialog::ShowDirsOnly );
    if( not sDirectory.isEmpty() )
    {
        ui->lineEdit_directory->setText( sDirectory );
        m_settings.setValue( KEY_DIRECTORY, sDirectory );
    }
}



void WidgetDuplicateSearch::slot_receiveNumberOfFiles( int nNumber )
{
    ui->lineEdit_nbFiles->setText( QString::number( nNumber ) );
}

void WidgetDuplicateSearch::slot_receiveAnalysisProgress( double dPercent )
{
    ui->progressBar_analysis->setValue( std::round( dPercent ) );
}

void WidgetDuplicateSearch::on_pushButton_startAnalysis_clicked()
{
    ui->progressBar_analysis ->reset();
    ui->lineEdit_nbFiles     ->clear();
    ui->lineEdit_nbFileIDs   ->clear();
    ui->lineEdit_nbDuplicates->clear();
    ui->lineEdit_wastedMemory->clear();
    ui->treeWidget_duplicates->clear();

    QStringList listFilePaths = getAllFilePathsInDir( ui->lineEdit_directory->text() );
    ui->lineEdit_nbFiles->setText( QString::number( listFilePaths.size() ) );

    QFuture<QMap<FileID, QList<MediaFileData>>> future = QtConcurrent::mappedReduced( listFilePaths, calculateFileData, insertInMap );
    m_watcherAnalysis.setFuture( future );
}

void WidgetDuplicateSearch::slot_analysisFinished()
{
    QFile fileOut( "debug.txt" );
    fileOut.open( QIODevice::WriteOnly );
    QTextStream ts( &fileOut );


    QMap<FileID, QList<MediaFileData>> mapFileID_FileDataList = m_watcherAnalysis.future().result();
    ui->lineEdit_nbFileIDs->setText( QString::number( mapFileID_FileDataList.size() ) );

    int nTotalDuplicates = 0;
    int nTotalDuplicateSize = 0;
    for( QMap<FileID, QList<MediaFileData>>::iterator it = mapFileID_FileDataList.begin(); it != mapFileID_FileDataList.end(); ++it )
    {
        if( it.value().size() > 1 )
        {
            ts << it.key().nSize << endl;

            int nNbDuplicates = it.value().size() - 1;
            nTotalDuplicates    += nNbDuplicates;
            nTotalDuplicateSize += nNbDuplicates * it.key().nSize;

            QString sGroupLabel = it.key().toString() + " (" + QString::number( it.value().size() ) + ")";

            QTreeWidgetItem * pItemFileID = new QTreeWidgetItem( ui->treeWidget_duplicates, {sGroupLabel} );

            for( const MediaFileData & fileData : it.value() )
            {
                new QTreeWidgetItem( pItemFileID, {fileData.filePath} );
            }
        }
    }
    ui->lineEdit_nbDuplicates->setText( QString::number( nTotalDuplicates ) );
    ui->lineEdit_wastedMemory->setText( QString::number( nTotalDuplicateSize / 1024.0 / 1024.0, 'f', 1 ) + " MB" );
}
