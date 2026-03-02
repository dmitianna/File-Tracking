#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QTextStream>
#include <QMutex>

class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger& instance();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void logInfo(const QString &message);
    void logError(const QString &message);
    void logEvent(const QString &message);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    QTextStream m_consoleStream;
    QMutex m_mutex;
};

#endif // LOGGER_H
