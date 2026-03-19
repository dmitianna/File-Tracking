#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>
#include <QMutex>

class Logger
{
public:
    static Logger& instance();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logInfo(const QString &message);
    void logError(const QString &message);
    void logEvent(const QString &message);

private:
    Logger();
    ~Logger();

    QTextStream m_consoleStream;
    QMutex m_mutex;
};

#endif // LOGGER_H
