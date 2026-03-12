#include "manager.h"
#include "logger.h"

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
    Logger::instance().logInfo("FileManager created");
}

FileManager::~FileManager()
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_files.size(); ++i)
    {
        delete m_files[i];
    }
    m_files.clear();
    Logger::instance().logInfo("FileManager destroyed");
}

FileManager& FileManager::instance()
{
    static FileManager manager;
    return manager;
}

void FileManager::addFile(const QString &path)
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->path() == path)
        {
            Logger::instance().logEvent("File already tracked: " + path);
            return;
        }
    }

    TrackedFile* file = new TrackedFile(path);

    connect(file, &TrackedFile::fileCreated,this, &FileManager::onFileCreated);
    connect(file, &TrackedFile::fileModified,this, &FileManager::onFileModified);
    connect(file, &TrackedFile::fileNotExists,this, &FileManager::onFileNotExists);

    m_files.append(file);

    Logger::instance().logEvent("File added: " + path);
    file->checkForChanges();
}

void FileManager::removeFile(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->path() == path)
        {
            delete m_files[i];
            m_files.remove(i);
            Logger::instance().logEvent("File removed: " + path);
            return;
        }
    }
    Logger::instance().logError("File not found: " + path);
}

void FileManager::listFiles() const
{
    QMutexLocker locker(&m_mutex);

    if (m_files.isEmpty())
    {
        Logger::instance().logInfo("No files being tracked");
    }
    else
    {
        Logger::instance().logInfo("Tracked files (" + QString::number(m_files.size()) + "):");
        for (int i = 0; i < m_files.size(); ++i)
        {
            const TrackedFile* file = m_files[i];
            QString status;
            if (file->exists())
            {
                status = "exists, size: " + QString::number(file->size()) + " bytes";
            }
            else
            {
                status = "does not exist";
            }
            Logger::instance().logInfo("  " + file->path() + " (" + status + ")");
        }
    }
}

int FileManager::fileCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_files.size();
}

const QVector<TrackedFile*>& FileManager::files() const
{
    return m_files;
}

TrackedFile* FileManager::getFile(const QString &path) const
{
    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i]->path() == path)
        {
            return m_files[i];
        }
    }
    return nullptr;
}

void FileManager::onFileCreated(const QString &path, qint64 size)
{
    QString message;
    if (size == 0)
    {
        message = QString("File exists: %1, size: %2 bytes (empty)").arg(path).arg(size);
    }
    else
    {
        message = QString("File exists: %1, size: %2 bytes").arg(path).arg(size);
    }
    Logger::instance().logEvent(message);
}

void FileManager::onFileModified(const QString &path, qint64 size)
{
    QString message = QString("File changed: %1, new size: %2 bytes").arg(path).arg(size);
    Logger::instance().logEvent(message);

    QString existsMsg = QString("File exists: %1, size: %2 bytes").arg(path).arg(size);
    Logger::instance().logEvent(existsMsg);
}

void FileManager::onFileNotExists(const QString &path)
{
    QString message = QString("File does not exist: %1").arg(path);
    Logger::instance().logEvent(message);
}
