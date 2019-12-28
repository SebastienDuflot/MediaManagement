#pragma once

#include <QString>
#include <QDateTime>

extern qint64 NB_BYTES_FOR_MEDIA_FILES;

struct FileID
{
    qint64 nSize = -1;
    QString hash_md5;


    FileID() = default;
    FileID( const QString & p_filePath );
    QString toString() const;
    bool operator <( const FileID & other ) const
    {
        if( nSize != other.nSize )
            return nSize > other.nSize;

        return hash_md5 < other.hash_md5;
    }
};

struct MediaFileData
{
    QString   filePath;
    FileID    fileID;

    QDateTime created;
    QDateTime lastModified;
    QDateTime lastRead;


    MediaFileData() = default;
    MediaFileData( const QString & p_sFilePath );
    void print() const;
};
