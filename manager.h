#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include "fileentity.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    static FileManager& instance();

    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

    void addFile(const QString &path);
    void removeFile(const QString &path);
    void listFiles() const;
    int fileCount() const;
    const QVector<TrackedFile*>& files() const { return m_files; }
    TrackedFile* getFile(const QString &path) const;

private slots:
    void onFileCreated(const QString &path, qint64 size);
    void onFileModified(const QString &path, qint64 size);
    void onFileNotExists(const QString &path);

private:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();

    QVector<TrackedFile*> m_files;
    mutable QMutex m_mutex;
};

#endif // FILEMANAGER_H
