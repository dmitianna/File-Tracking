#include "fileentity.h"

TrackedFile::TrackedFile(const QString &filePath, QObject *parent): QObject(parent)
    , m_filePath(filePath)
    , m_exists(false)
    , m_fileSize(0)
    , m_fileInfo(filePath)
{
    m_fileInfo.refresh();

    if (m_fileInfo.exists() && m_fileInfo.isFile())
    {
        m_exists = true;
        m_fileSize = m_fileInfo.size();
    }
}

void TrackedFile::checkForChanges()
{
    m_fileInfo.refresh();
    bool nowExists = m_fileInfo.exists() && m_fileInfo.isFile();;

    if (!m_exists && nowExists)
    {
        qint64 newSize = m_fileInfo.size();

        m_exists = true;
        m_fileSize = newSize;

        emit fileCreated(m_filePath, newSize);
        return;
    }

    if (m_exists && !nowExists)
    {
        m_exists = false;
        m_fileSize = 0;

        emit fileNotExists(m_filePath);
        return;
    }

    if (m_exists && nowExists)
    {
        qint64 newSize = m_fileInfo.size();

        if (m_fileSize != newSize)
        {
            m_fileSize = newSize;
            emit fileModified(m_filePath, newSize);
        }
    }
}

void TrackedFile::refreshState()
{
    m_fileInfo.refresh();

    if (m_fileInfo.exists() && m_fileInfo.isFile())
    {
        m_exists = true;
        m_fileSize = m_fileInfo.size();
    }
    else
    {
        m_exists = false;
        m_fileSize = 0;
    }
}
