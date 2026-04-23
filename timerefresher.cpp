#include "timerefresher.h"

TimeRefresher::TimeRefresher(QObject *parent): QObject(parent), m_timer(this)
{
    connect(&m_timer, &QTimer::timeout,this, &TimeRefresher::refreshRequested);
}

void TimeRefresher::start()
{
    m_timer.start();
}

void TimeRefresher::stop()
{
    m_timer.stop();
}

bool TimeRefresher::isRunning() const
{
    return m_timer.isActive();
}

void TimeRefresher::setInterval(int interval)
{
    if (interval <= 0)
    {
        interval = 100;
    }
    m_timer.setInterval(interval);
}
