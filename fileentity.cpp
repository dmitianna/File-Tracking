#include "fileentity.h"
#include <QDebug>

TrackedFile::TrackedFile(const QString &filePath, QObject *parent): QObject(parent)
    , m_filePath(filePath)
    , m_exists(false)
    , m_fileSize(0)
    , m_fileInfo(filePath)
{
    if (m_fileInfo.exists())
    {
        m_exists = true;
        m_fileSize = m_fileInfo.size();
    } else
    {
        m_exists = false;
        m_fileSize = 0;
    }

    qDebug() << "Start of tracking:" << m_filePath;
}

void TrackedFile::checkForChanges()
{
    bool nowExists = m_fileInfo.exists();

    // Раньше файла не было
    if (m_exists == false)
    {
        if (nowExists == true)
        {
            m_fileInfo.refresh();
            qint64 newSize = m_fileInfo.size();

            m_exists = true;
            m_fileSize = newSize;

            emit fileCreated(m_filePath, newSize);
            qDebug() << "File created:" << m_filePath << "size:" << newSize;
        }
        return;
    }

    // Раньше файл существовал
    if (m_exists == true)
    {
        if (nowExists == false)
        {
            m_exists = false;
            m_fileSize = 0;

            emit fileNotExists(m_filePath);
            qDebug() << "File does not exist:" << m_filePath;
            return;
        }
        if (nowExists == true)
        {
            m_fileInfo.refresh();
            qint64 newSize = m_fileInfo.size();

            if (m_fileSize != newSize)
            {
                qint64 oldSize = m_fileSize;
                m_fileSize = newSize;

                emit fileModified(m_filePath, newSize);
                qDebug() << "File modified:" << m_filePath << "was:" << oldSize << "now:" << newSize;
            }
        }
    }
}
