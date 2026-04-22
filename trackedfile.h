#ifndef TRACKEDFILE_H
#define TRACKEDFILE_H

#include <QObject>

class TrackedFile : public QObject
{
    Q_OBJECT

public:
    explicit TrackedFile(const QString &filePath);

    QString path() const { return m_filePath; }
    bool exists() const { return m_exists; }
    qint64 size() const { return m_fileSize; }

    void setState(bool exists, qint64 size);
private:
    QString m_filePath;
    bool m_exists;
    qint64 m_fileSize;
};

#endif // TRACKEDFILE_H
