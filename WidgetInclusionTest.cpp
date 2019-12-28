#include "WidgetInclusionTest.h"
#include "ui_WidgetInclusionTest.h"
#include "CommonFunctions.h"

#include <QDebug>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>

QString KEY_REFERENCE_DIRECTORY = "ReferenceDirectory";
QString KEY_SUBSET_DIRECTORY    = "SubsetDirectory";

WidgetInclusionTest::WidgetInclusionTest( QWidget * parent, QSettings & p_settings )
    : QWidget( parent )
    , m_settings( p_settings )
    , ui( new Ui::WidgetInclusionTest )
{
    ui->setupUi( this );

    ui->comboBox_inclusionTestMode->setCurrentIndex( 1 );
    ui->lineEdit_referenceDirectory->setText( m_settings.value( KEY_REFERENCE_DIRECTORY ).toString() );
    ui->lineEdit_subsetDirectory   ->setText( m_settings.value( KEY_SUBSET_DIRECTORY    ).toString() );

    connect( &m_watcherAnalysisRef, SIGNAL( progressRangeChanged( int, int ) ), ui->progressBar_referenceAnalysis, SLOT( setRange( int, int ) ) );
    connect( &m_watcherAnalysisRef, SIGNAL( progressValueChanged( int ) ),      ui->progressBar_referenceAnalysis, SLOT( setValue( int ) ) );

    connect( &m_watcherAnalysisSubset, SIGNAL( progressRangeChanged( int, int ) ), ui->progressBar_subsetAnalysis, SLOT( setRange( int, int ) ) );
    connect( &m_watcherAnalysisSubset, SIGNAL( progressValueChanged( int ) ),      ui->progressBar_subsetAnalysis, SLOT( setValue( int ) ) );

    connect( &m_watcherAnalysisSubset, SIGNAL( finished() ), this, SLOT( slot_analysisSubset_finished() ) );
    connect( &m_watcherAnalysisRef, SIGNAL( finished() ), this, SLOT( slot_analysisRef_finished() ) );
}

WidgetInclusionTest::~WidgetInclusionTest()
{
    delete ui;
}

void WidgetInclusionTest::on_pushButton_switchDirs_clicked()
{
    QString sDir_reference = ui->lineEdit_referenceDirectory->text();
    QString sDir_subset    = ui->lineEdit_subsetDirectory->text();

    ui->lineEdit_referenceDirectory->setText( sDir_subset );
    m_settings.setValue( KEY_REFERENCE_DIRECTORY, sDir_subset );

    ui->lineEdit_subsetDirectory->setText( sDir_reference );
    m_settings.setValue( KEY_SUBSET_DIRECTORY, sDir_reference );
}

void WidgetInclusionTest::on_pushButton_referenceDirectory_clicked()
{
    QString sDirectory = QFileDialog::getExistingDirectory( this, "Select Reference Directory",
                                                            ui->lineEdit_referenceDirectory->text(),
                                                            QFileDialog::ShowDirsOnly );
    if( not sDirectory.isEmpty() )
    {
        ui->lineEdit_referenceDirectory->setText( sDirectory );
        m_settings.setValue( KEY_REFERENCE_DIRECTORY, sDirectory );
    }
}

void WidgetInclusionTest::on_pushButton_subsetDirectory_clicked()
{
    QString sDirectory = QFileDialog::getExistingDirectory( this, "Select Subset Directory",
                                                            ui->lineEdit_subsetDirectory->text(),
                                                            QFileDialog::ShowDirsOnly );
    if( not sDirectory.isEmpty() )
    {
        ui->lineEdit_subsetDirectory->setText( sDirectory );
        m_settings.setValue( KEY_SUBSET_DIRECTORY, sDirectory );
    }
}

void WidgetInclusionTest::on_pushButton_checkInclusion_clicked()
{
    NB_BYTES_FOR_MEDIA_FILES = ui->spinBox_nbBytesMediaHash->value();

    ui->progressBar_referenceAnalysis->reset();
    ui->progressBar_subsetAnalysis->reset();
    ui->lineEdit_filesInReference->clear();
    ui->lineEdit_filesInSubset->clear();
    ui->lineEdit_filesOfSubsetMissingInReference->clear();
    ui->textEdit_inclusionOutput->clear();

    if( ui->comboBox_inclusionTestMode->currentIndex() == 0 )
    {
        checkInclusion_RelativeFilePath();
    }
    else if( ui->comboBox_inclusionTestMode->currentIndex() == 1 )
    {
        checkInclusion_FileID();
    }
}

void WidgetInclusionTest::checkInclusion_RelativeFilePath()
{
    QSet<QString> setRelativeFilePath_reference;

    QStringList listFilePaths_ref = getAllFilePathsInDir( ui->lineEdit_referenceDirectory->text() );
    ui->lineEdit_filesInReference->setText( QString::number( listFilePaths_ref.size() ) );
    QDir dirRef( ui->lineEdit_referenceDirectory->text() );
    for( const QString & sFilePath : listFilePaths_ref )
    {
        setRelativeFilePath_reference.insert( dirRef.relativeFilePath( sFilePath ) );
    }


    QStringList listFilePaths_subset = getAllFilePathsInDir( ui->lineEdit_subsetDirectory->text() );
    ui->lineEdit_filesInSubset->setText( QString::number( listFilePaths_subset.size() ) );
    QDir dirSubset( ui->lineEdit_subsetDirectory->text() );

    int nNbFilesMissing = 0;
    for( const QString & sFilePath : listFilePaths_subset )
    {
        QString sRelativeFilePath = dirSubset.relativeFilePath( sFilePath );
        if( not setRelativeFilePath_reference.contains( sRelativeFilePath ) )
        {
            ui->textEdit_inclusionOutput->append( sRelativeFilePath );
            ++nNbFilesMissing;
        }
    }
    ui->lineEdit_filesOfSubsetMissingInReference->setText( QString::number( nNbFilesMissing ) );
}

void WidgetInclusionTest::checkInclusion_FileID()
{
    qDebug() << Q_FUNC_INFO;

    QStringList listFilePaths_ref = getAllFilePathsInDir( ui->lineEdit_referenceDirectory->text() );
    QFuture<QMap<FileID, QList<MediaFileData>>> future = QtConcurrent::mappedReduced( listFilePaths_ref, calculateFileData, insertInMap );
    m_watcherAnalysisRef.setFuture( future );
}

void WidgetInclusionTest::slot_analysisRef_finished()
{
    qDebug() << Q_FUNC_INFO;

    m_mapFileID_FileDataList_ref = m_watcherAnalysisRef.future().result();
    ui->lineEdit_filesInReference->setText( QString::number( m_mapFileID_FileDataList_ref.size() ) );

    QStringList listFilePaths_subset = getAllFilePathsInDir( ui->lineEdit_subsetDirectory->text() );
    QFuture<QMap<FileID, QList<MediaFileData>>> future = QtConcurrent::mappedReduced( listFilePaths_subset, calculateFileData, insertInMap );
    m_watcherAnalysisSubset.setFuture( future );
}

void WidgetInclusionTest::slot_analysisSubset_finished()
{
    qDebug() << Q_FUNC_INFO;

    m_mapFileID_FileDataList_subset = m_watcherAnalysisSubset.future().result();
    int nNbFilesSubset = m_mapFileID_FileDataList_subset.size();
    ui->lineEdit_filesInSubset->setText( QString::number( nNbFilesSubset ) );

    int nNbFilesMissing = 0;
    for( QMap<FileID, QList<MediaFileData>>::iterator it = m_mapFileID_FileDataList_subset.begin(); it != m_mapFileID_FileDataList_subset.end(); ++it )
    {
        if( not m_mapFileID_FileDataList_ref.contains( it.key() ) )
        {
            ++nNbFilesMissing;
            for( const MediaFileData & fileData : it.value() )
            {
                ui->textEdit_inclusionOutput->append( fileData.filePath + " (" + it.key().toString() + ")" );
            }
        }
    }

    ui->lineEdit_filesOfSubsetMissingInReference->setText( QString::number( nNbFilesMissing ) );
}

