#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <vector>
#include <memory>
#include "IRefresher.h"
#include "trackedfile.h"

struct FileInfo
{
    QString path;
    bool exists;
    qint64 size;
};

class FileManager : public QObject
{
    Q_OBJECT

public:
    static FileManager& instance();
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;

public slots:
    void addFile(const QString &path);
    void removeFile(const QString &path);
    //void listFiles();
    void startTracking();
    void stopTracking();
    QVector<FileInfo> getFiles() const;
    void processCommand(const QString& input);
signals:
    void fileExists(const QString &path, qint64 size);
    void fileModified(const QString &path, qint64 size);
    void fileNotExists(const QString &path);
    void shutdownRequested();
private slots:
    void checkAllFiles();
    void onFileExists(const QString &path, qint64 size);
    void onFileModified(const QString &path, qint64 size);
    void onFileNotExists(const QString &path);
    void shutdown();

private:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();
    IRefresher *m_refresher;
    QString normalizePath(const QString &path) const;
    std::vector<std::unique_ptr<TrackedFile>> m_files;
    bool m_isShutdown = false;
};

#endif // FILEMANAGER_H
