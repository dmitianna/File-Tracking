#ifndef FILEINFO_H
#define FILEINFO_H
#include <QObject>

struct FileInfo
{
    QString path;
    bool exists;
    qint64 size;
};

#endif // FILEINFO_H
