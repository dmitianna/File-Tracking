#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QPointer>
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
    void fileCreated(const QString &path, qint64 size);
    void fileModified(const QString &path, qint64 size);
    void fileNotExists(const QString &path);
private slots:
    void checkAllFiles();
    void shutdown();
    void onFileCreated(const QString &path, qint64 size);
    void onFileModified(const QString &path, qint64 size);
    void onFileNotExists(const QString &path);

private:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();

    QString normalizePath(const QString &path) const;
    QVector<QPointer<TrackedFile>> m_files;
    QTimer *m_timer;
    bool m_tracking;
};

#endif // FILEMANAGER_H
