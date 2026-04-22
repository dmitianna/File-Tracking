#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVector>
#include <QPointer>
#include <memory>
#include "IRefresher.h"
#include "trackedfile.h"

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
    void listFiles();
    void startTracking();
    void stopTracking();
signals:
    void fileExists(const QString &path, qint64 size);
    void fileModified(const QString &path, qint64 size);
    void fileNotExists(const QString &path);
private slots:
    void checkAllFiles();
    void shutdown();
    void onFileExists(const QString &path, qint64 size);
    void onFileModified(const QString &path, qint64 size);
    void onFileNotExists(const QString &path);

private:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();
    std::unique_ptr<IRefresher> m_refresher;
    QString normalizePath(const QString &path) const;
    QVector<QPointer<TrackedFile>> m_files;
};

#endif // FILEMANAGER_H
