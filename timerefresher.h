#ifndef TIMEREFRESHER_H
#define TIMEREFRESHER_H

#include <QObject>
#include <QTimer>
#include "IRefresher.h"

class TimeRefresher:public QObject, public IRefresher
{
    Q_OBJECT
public:
    explicit TimeRefresher(QObject *parent = nullptr);

    void start() override;
    void stop() override;
    bool isRunning() const override;

    void setInterval(int ms);

signals:
    void refreshRequested();

private:
    QTimer m_timer;
};

#endif // TIMEREFRESHER_H
