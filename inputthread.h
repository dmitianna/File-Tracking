#ifndef INPUTTHREAD_H
#define INPUTTHREAD_H

#include <QThread>

class InputThread : public QThread
{
    Q_OBJECT
public:
    explicit InputThread(QObject* parent = nullptr);
signals:
    void commandReceived(const QString& command);

protected:
    void run() override;
};

#endif // INPUTTHREAD_H
