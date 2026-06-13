#include "manager.h"
#include "logger.h"
#include "timerefresher.h"
#include <QDir>

FileManager::FileManager(QObject *parent): QObject(parent), m_refresher(nullptr)
{
    //Logger::instance().logInfo("FileManager created");
    IRefresher *refresher = new TimeRefresher(this);
    if (!refresher)
    {
        Logger::instance().logError("Refresher is not available. Tracking disabled.");
    }
    else
    {
        refresher->setInterval(100);
        connect(refresher, &IRefresher::refreshRequested,this, &FileManager::checkAllFiles);
        m_refresher = refresher;
    }
    connect(this, &FileManager::fileExists,this, &FileManager::onFileExists);
    connect(this, &FileManager::fileModified,this, &FileManager::onFileModified);
    connect(this, &FileManager::fileNotExists,this, &FileManager::onFileNotExists);
    connect(this,&FileManager::shutdownRequested,this,&FileManager::shutdown);
}

FileManager::~FileManager()
{
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
    if (!m_refresher)
    {
        Logger::instance().logError("Tracking functionality is not available.");
        return;
    }
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
            Logger::instance().logInfo("File already tracked: " + normalizedPath);
            return;
        }
    }
    std::unique_ptr<TrackedFile> file(new TrackedFile(normalizedPath));
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

            if (m_files.empty() && m_refresher && m_refresher->isRunning())
            {
                stopTracking();
            }

            return;
        }
    }

    Logger::instance().logError("File is not found in the list: " + normalizedPath);
}

QVector<FileInfo> FileManager::getFiles() const
{
    QVector<FileInfo> result;
    for (const auto& file : m_files)
    {
        QFileInfo info(file->path());

        bool exists = info.exists() && info.isFile();
        qint64 size = 0;
        if (exists)
        {
            size = info.size();
        }
        result.append({ file->path(), exists, size });
    }
    return result;
}

void FileManager::startTracking()
{
    if (!m_refresher)
    {
        Logger::instance().logError("Cannot start tracking: refresher not initialized");
        return;
    }

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
    if (!m_refresher)
    {
        Logger::instance().logError("Internal error: refresher is not initialized");
        return;
    }
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
    }
}

void FileManager::shutdown()
{
    if (m_isShutdown) return;

    if (m_refresher && m_refresher->isRunning())
    {
        m_refresher->stop();
        Logger::instance().logInfo("Tracking stopped");
    }
    m_files.clear();
    m_isShutdown = true;
}

void FileManager::processCommand(const QString& input)
{
    QString line = input.trimmed();

    if (line.isEmpty())
    {
        return;
    }

    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    QString command = parts[0].toLower();

    QString argument;

    if ((command == "list" || command == "start" || command == "stop" || command == "exit") && parts.size() > 1)
    {
        Logger::instance().logError("Too many arguments for command: " + command);
        return;
    }

    if ((command == "add" || command == "remove") && parts.size() != 2)
    {
        Logger::instance().logError("Invalid number of arguments for command: " + command);
        Logger::instance().logInfo("Usage: add/remove <path>");
        return;
    }

    if (command == "add" || command == "remove")
        argument = parts[1];

    if (command == "add") addFile(argument);
    else if (command == "remove") removeFile(argument);
    else if (command == "list") Logger::instance().logFileList(getFiles());
    else if (command == "start") startTracking();
    else if (command == "stop") stopTracking();
    else if (command == "exit")
    {
        emit shutdownRequested();
    }
    else
    {
        Logger::instance().logError("Unknown command: " + command);
    }
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
