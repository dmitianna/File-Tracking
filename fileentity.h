#ifndef FILEENTITY_H
#define FILEENTITY_H

#include <QObject>
#include <QFileInfo>

class TrackedFile : public QObject
{
    Q_OBJECT

public:
    explicit TrackedFile(const QString &filePath, QObject *parent = nullptr);

    QString path() const { return m_filePath; }
    bool exists() const { return m_exists; }
    qint64 size() const { return m_fileSize; }

signals:
    void fileCreated(const QString &path, qint64 size);
    void fileModified(const QString &path, qint64 size);
    void fileDeleted(const QString &path);

private:
    QString m_filePath;
    bool m_exists;
    qint64 m_fileSize;
    QFileInfo m_fileInfo;
};

#endif // FILEENTITY_H
