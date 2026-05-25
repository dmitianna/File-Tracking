#include "inputthread.h"
#include <iostream>
#include <string>

InputThread::InputThread(QObject* parent): QThread(parent)
{
}

void InputThread::stop()
{
    m_running = false;
}

void InputThread::run()
{
    std::string line;

    while (m_running)
    {
        std::getline(std::cin, line);

        if (!m_running)
        {
            break;
        }

        emit commandReceived(QString::fromStdString(line));

        if (line == "exit")
        {
            break;
        }
    }
}
