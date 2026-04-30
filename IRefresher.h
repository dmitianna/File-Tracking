#ifndef IREFRESHER_H
#define IREFRESHER_H
class IRefresher
{
public:
    virtual ~IRefresher() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
};

#endif // IREFRESHER_H
