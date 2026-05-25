#include "logger.h"
#include <QMutexLocker>
Logger::Logger(): m_consoleStream(stdout)
{
}

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::logFileList(const QVector<FileInfo>& files)
{
    if (files.isEmpty())
    {
        logInfo("No files being tracked");
        return;
    }

    logInfo(QString("Tracked files (%1):").arg(files.size()));

    for (const auto& f : files)
    {
        if (f.exists)
        {
            logInfo(QString("  %1 (exists, size: %2 bytes)").arg(f.path).arg(f.size));
        }
        else
        {
            logInfo(QString("  %1 (does not exist)").arg(f.path));
        }
    }
}

void Logger::logInfo(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "INFO: " << message << Qt::endl;
}

void Logger::logError(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "ERROR: " << message << Qt::endl;
}

void Logger::logEvent(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "EVENT: " << message << Qt::endl;
}
