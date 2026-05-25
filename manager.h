#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <vector>
#include <memory>
#include "IRefresher.h"
#include "trackedfile.h"
#include "fileinfo.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    static FileManager& instance();
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    QVector<FileInfo> getFiles() const;
public slots:
    void addFile(const QString &path);
    void removeFile(const QString &path);
    void startTracking();
    void stopTracking();
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

private:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();
    IRefresher *m_refresher;
    QString normalizePath(const QString &path) const;
    std::vector<std::unique_ptr<TrackedFile>> m_files;
    bool m_isShutdown = false;
    void shutdown();
};

#endif // FILEMANAGER_H
