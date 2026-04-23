#include "trackedfile.h"

TrackedFile::TrackedFile(const QString &filePath): m_filePath(filePath), m_exists(false), m_fileSize(0)
{
}

void TrackedFile::setState(bool exists, qint64 size)
{
    m_exists = exists;

    if (m_exists)
    {
        m_fileSize = size;
    }
    else
    {
        m_fileSize = 0;
    }
}

