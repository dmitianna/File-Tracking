#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>
#include <atomic>

class InputThread : public QThread
{
    Q_OBJECT
public:
    explicit InputThread(QObject* parent = nullptr);
    void stop();
signals:
    void commandReceived(const QString& command);

protected:
    void run() override;

private:
    std::atomic<bool> m_running = true;
};

#endif // INPUTTHREAD_H
