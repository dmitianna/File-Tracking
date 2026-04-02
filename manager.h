#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QPointer>
#include "fileentity.h"

class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);
    ~FileManager();

public slots:
    void addFile(const QString &path);
    void removeFile(const QString &path);
    void listFiles();
    void startTracking();
    void stopTracking();
    void checkAllFiles();
    void shutdown();

private slots:
    void onFileCreated(const QString &path, qint64 size);
    void onFileModified(const QString &path, qint64 size);
    void onFileNotExists(const QString &path);

private:
    QVector<QPointer<TrackedFile>> m_files;
    QTimer *m_timer;
    bool m_tracking;
};

#endif // FILEMANAGER_H
