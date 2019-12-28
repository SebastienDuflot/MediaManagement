#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include "MediaFileData.h"

#include <QDirIterator>

inline MediaFileData calculateFileData( const QString & p_sFilePath )
{
    return MediaFileData( p_sFilePath );
}

inline void insertInMap( QMap<FileID, QList<MediaFileData>> & p_mapFileID_FileDataList, const MediaFileData & p_fileData )
{
    p_mapFileID_FileDataList[ p_fileData.fileID ].append( p_fileData );
}

inline QStringList getAllFilePathsInDir( const QString & p_sDirPath )
{
    QDirIterator dirIter( p_sDirPath,
                          QDir::Files,
                          QDirIterator::Subdirectories );

    QStringList listFilePaths;
    while( dirIter.hasNext() )
    {
        listFilePaths.append( dirIter.next() );
    }
    return listFilePaths;
}

#endif // COMMON_FUNCTIONS_H
