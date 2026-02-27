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
