#include "manager.h"
#include "logger.h"

FileManager::FileManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_tracking(false)
{
    m_timer->setInterval(100);
    connect(m_timer, &QTimer::timeout, this, &FileManager::checkAllFiles);
    Logger::instance().logInfo("FileManager created");
}


FileManager::~FileManager()
{
    if(m_timer->isActive())
    {
        m_timer->stop();
    }
    m_files.clear();
    Logger::instance().logInfo("FileManager destroyed");
}


void FileManager::addFile(const QString &path)
{
    QString normalizedPath = path.trimmed();
    if(normalizedPath.isEmpty())
    {
        Logger::instance().logError("Empty path");
        return;
    }

    QFileInfo info(normalizedPath);
    if (info.exists() && !info.isFile())
    {
        Logger::instance().logError("The path is not a file: " + normalizedPath);
        return;
    }

    if (!info.exists())
    {
        Logger::instance().logError("File does not exist: " + normalizedPath);
        return;
    }
    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i] && (m_files[i]->path() == normalizedPath))
        {
            Logger::instance().logEvent("File already tracked: " + path);
            return;
        }
    }
    TrackedFile* file = new TrackedFile(normalizedPath, this);
    connect(file, &TrackedFile::fileCreated,this, &FileManager::onFileCreated);
    connect(file, &TrackedFile::fileModified,this, &FileManager::onFileModified);
    connect(file, &TrackedFile::fileNotExists,this, &FileManager::onFileNotExists);

    m_files.append(QPointer<TrackedFile>(file));

    Logger::instance().logEvent("File added: " + normalizedPath);

    if (file->exists())
    {
        Logger::instance().logEvent(
            QString("File exists: %1, size: %2 bytes")
                .arg(file->path())
                .arg(file->size()));
    }
    else
    {
        Logger::instance().logEvent(QString("File does not exist: %1").arg(file->path()));
    }
}

void FileManager::removeFile(const QString &path)
{
    QString normalizedPath = path.trimmed();
    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i] && (m_files[i]->path() == normalizedPath))
        {
            TrackedFile *file = m_files[i];
            m_files.removeAt(i);

            if (file)
            {
                delete file;
            }
            Logger::instance().logEvent("File removed: " + normalizedPath);
            return;
        }
    }
    Logger::instance().logError("File not found: " + normalizedPath);
}

void FileManager::listFiles()
{
    if (m_files.isEmpty())
    {
        Logger::instance().logInfo("No files being tracked");
        return;
    }
    Logger::instance().logInfo(
        "Tracked files (" + QString::number(m_files.size()) + "):");

    for (int i = 0; i < m_files.size(); ++i)
    {
        TrackedFile *file = m_files[i];
        if (file->exists())
        {
            Logger::instance().logInfo(
                QString("  %1 (exists, size: %2 bytes)")
                    .arg(file->path())
                    .arg(file->size()));
        }
        else
        {
            Logger::instance().logInfo(
                QString("  %1 (does not exist)")
                    .arg(file->path()));
        }
    }
}

void FileManager::startTracking()
{
    if (m_files.isEmpty())
    {
        Logger::instance().logError("No files to track. Add files first.");
        return;
    }

    if (m_tracking)
    {
        Logger::instance().logError("Tracking already running");
        return;
    }

    m_timer->start();
    m_tracking = true;

    Logger::instance().logInfo(
        "Tracking started for " + QString::number(m_files.size()) + " files");
}

void FileManager::stopTracking()
{
    if (!m_tracking)
    {
        Logger::instance().logError("Tracking is not running");
        return;
    }

    m_timer->stop();
    m_tracking = false;

    Logger::instance().logInfo("Tracking stopped");
}

void FileManager::checkAllFiles()
{
    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i])
        {
            m_files[i]->checkForChanges();
        }
    }
}

void FileManager::shutdown()
{
    if (m_tracking)
    {
        m_timer->stop();
        m_tracking = false;
    }
}

void FileManager::destroyTrackedObjects()
{
    for (int i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i])
        {
            delete m_files[i];
        }
    }
    m_files.clear();
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
