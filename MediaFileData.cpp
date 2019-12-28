#include "MediaFileData.h"

#include <QDebug>
#include <QFileInfo>
#include <QCryptographicHash>

qint64 NB_BYTES_FOR_MEDIA_FILES = 10000;

FileID::FileID( const QString & p_sFilePath )
{
    QFile file( p_sFilePath );
    if( file.open( QIODevice::ReadOnly ) )
    {
        nSize = file.size();

        if( p_sFilePath.endsWith( ".jpg", Qt::CaseInsensitive ) or p_sFilePath.endsWith( ".mp4", Qt::CaseInsensitive ) )
        {
//            qDebug() << Q_FUNC_INFO << NB_BYTES_FOR_MEDIA_FILES;
            QByteArray baBegin = file.read( NB_BYTES_FOR_MEDIA_FILES );
            hash_md5 = QString( QCryptographicHash::hash( baBegin, QCryptographicHash::Md5 ).toHex() );
        }
        else
        {
            QCryptographicHash hash( QCryptographicHash::Md5 );
            if( hash.addData( &file ) )
            {
                hash_md5 = QString( hash.result().toHex() );
            }
        }
    }
}

QString FileID::toString() const
{
    return QString::number( nSize ) + "_" + hash_md5;
}


MediaFileData::MediaFileData( const QString & p_sFilePath )
    : filePath( p_sFilePath )
    , fileID  ( p_sFilePath )
{
    QFileInfo fileInfo( p_sFilePath );
    created      = fileInfo.created();
    lastModified = fileInfo.lastModified();
    lastRead     = fileInfo.lastRead();
}

void MediaFileData::print() const
{
    qDebug() << filePath
             << "\n" << created.toString( Qt::ISODate )
             << "\n" << lastModified.toString( Qt::ISODate )
             << "\n" << lastRead.toString( Qt::ISODate )
             << "\n" << fileID.hash_md5;
}
