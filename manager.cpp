#include "manager.h"
#include "logger.h"
#include "timerefresher.h"
#include <QDir>

FileManager::FileManager(QObject *parent): QObject(parent)
{
    //Logger::instance().logInfo("FileManager created");

    TimeRefresher *refresher = new TimeRefresher(this);
    refresher->setInterval(100);
    connect(refresher, &TimeRefresher::refreshRequested,this, &FileManager::checkAllFiles);
    m_refresher = refresher;


    connect(this, &FileManager::fileExists,this, &FileManager::onFileExists);
    connect(this, &FileManager::fileModified,this, &FileManager::onFileModified);
    connect(this, &FileManager::fileNotExists,this, &FileManager::onFileNotExists);
}

FileManager::~FileManager()
{
    shutdown();
}

FileManager& FileManager::instance()
{
    static FileManager instance;
    return instance;
}


QString FileManager::normalizePath(const QString &path) const
{
    QString normalized = path.trimmed();

    if (normalized.length() >= 2)
    {
        const QChar first = normalized.front();
        const QChar last = normalized.back();

        if ((first == '"' && last == '"') ||
            (first == '\'' && last == '\''))
        {
            normalized = normalized.mid(1, normalized.length() - 2).trimmed();
        }
    }
    return QDir::cleanPath(QDir::fromNativeSeparators(normalized));
}

void FileManager::addFile(const QString &path)
{
    QString normalizedPath = normalizePath(path);
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

    for (std::size_t i = 0; i < m_files.size(); ++i)
    {
        if (m_files[i] && (m_files[i]->path() == normalizedPath))
        {
            Logger::instance().logEvent("File already tracked: " + normalizedPath);
            return;
        }
    }
    auto file = std::make_unique<TrackedFile>(normalizedPath);
    bool exists = info.exists() && info.isFile();
    qint64 size = 0;

    if (exists)
    {
        size = info.size();
    }

    file->setState(exists, size);
    m_files.push_back(std::move(file));

    Logger::instance().logEvent("File added: " + normalizedPath);

    if (m_files.back()->exists())
    {
        Logger::instance().logEvent(QString("File exists: %1, size: %2 bytes").arg(m_files.back()->path()).arg(m_files.back()->size()));
    }
    else
    {
        Logger::instance().logEvent(QString("File does not exist: %1").arg(m_files.back()->path()));
    }
}

void FileManager::removeFile(const QString &path)
{
    QString normalizedPath = normalizePath(path);
    for (auto it = m_files.begin(); it != m_files.end(); ++it)
    {
        if ((*it)->path() == normalizedPath)
        {
            m_files.erase(it);
            Logger::instance().logEvent("File removed: " + normalizedPath);

            if (m_files.empty() && m_refresher->isRunning())
            {
                stopTracking();
            }

            return;
        }
    }

    Logger::instance().logError("File not found: " + normalizedPath);
}

void FileManager::listFiles()
{
    if (m_files.empty())
    {
        Logger::instance().logInfo("No files being tracked");
        return;
    }
    Logger::instance().logInfo("Tracked files (" + QString::number(m_files.size()) + "):");

    for (std::size_t i = 0; i < m_files.size(); ++i)
    {
        QFileInfo info(m_files[i]->path());
        bool exists = info.exists() && info.isFile();
        if (exists)
        {
            Logger::instance().logInfo(QString("  %1 (exists, size: %2 bytes)").arg(m_files[i]->path()).arg(info.size()));
        }
        else
        {
            Logger::instance().logInfo(QString("  %1 (does not exist)").arg(m_files[i]->path()));
        }
    }
}

void FileManager::startTracking()
{
    if (m_files.empty())
    {
        Logger::instance().logError("No files to track. Add files first.");
        return;
    }

    if (m_refresher->isRunning())
    {
        Logger::instance().logError("Tracking already running");
        return;
    }
    m_refresher->start();
    Logger::instance().logInfo("Tracking started for " + QString::number(m_files.size()) + " files");
}

void FileManager::stopTracking()
{
    if (!m_refresher->isRunning())
    {
        Logger::instance().logError("Tracking is not running");
        return;
    }
    m_refresher->stop();
    Logger::instance().logInfo("Tracking stopped");
}

void FileManager::checkAllFiles()
{
    for (std::size_t i = 0; i < m_files.size(); ++i)
    {
        TrackedFile &file = *m_files[i];

        bool oldExists = file.exists();
        qint64 oldSize = file.size();

        QFileInfo info(file.path());
        bool newExists = info.exists() && info.isFile();
        qint64 newSize = 0;

        if (newExists)
        {
            newSize = info.size();
        }

        if (!oldExists && newExists)
        {
            file.setState(newExists, newSize);
            emit fileExists(file.path(), newSize);
            continue;
        }

        if (oldExists && !newExists)
        {
            file.setState(newExists, newSize);
            emit fileNotExists(file.path());
            continue;
        }

        if (oldExists && newExists && oldSize != newSize)
        {
            file.setState(newExists, newSize);
            emit fileModified(file.path(), newSize);
            continue;
        }

        if (oldExists != newExists || oldSize != newSize)
        {
            file.setState(newExists, newSize);
        }
    }
}

void FileManager::shutdown()
{
    if (m_isShutdown) return;

    if (m_refresher && m_refresher->isRunning())
    {
        m_refresher->stop();
    }

    m_files.clear();
    m_isShutdown = true;
}


void FileManager::onFileExists(const QString &path, qint64 size)
{
    Logger::instance().logEvent(QString("File exists: %1, size: %2 bytes").arg(path).arg(size));
}

void FileManager::onFileModified(const QString &path, qint64 size)
{
    Logger::instance().logEvent(QString("File modified: %1, size: %2 bytes").arg(path).arg(size));
}

void FileManager::onFileNotExists(const QString &path)
{
    Logger::instance().logEvent(QString("File does not exist: %1").arg(path));
}
