#ifndef IREFRESHER_H
#define IREFRESHER_H
#include <QObject>
class IRefresher : public QObject
{
    Q_OBJECT
public:
    explicit IRefresher(QObject* parent = nullptr): QObject(parent)
    {
    }
    virtual ~IRefresher() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual void setInterval(int ms) = 0;
signals:
    void refreshRequested();
};

#endif // IREFRESHER_H
