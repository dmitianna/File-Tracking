#include "logger.h"

Logger::Logger(): m_consoleStream(stdout)
{
    logInfo("Logging started");
}

Logger::~Logger()
{
    logInfo("Logging stopped");
}

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::logInfo(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "INFO: " << message << "\n";
    m_consoleStream.flush();
}

void Logger::logError(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "ERROR: " << message << "\n";
    m_consoleStream.flush();
}

void Logger::logEvent(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "EVENT: " << message << "\n";
    m_consoleStream.flush();
}
