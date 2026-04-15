#include "logger.h"

Logger::Logger(): m_consoleStream(stdout)
{
    //logInfo("Logging started");
}

Logger::~Logger()
{
    //logInfo("Logging stopped");
}

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::logInfo(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "INFO: " << message << "\n" << Qt::endl;
}

void Logger::logError(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "ERROR: " << message << "\n"<< Qt::endl;
}

void Logger::logEvent(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_consoleStream << "EVENT: " << message << "\n"<< Qt::endl;
}
