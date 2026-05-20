#include "inputthread.h"
#include <iostream>
#include <string>

InputThread::InputThread(QObject* parent): QThread(parent)
{
}
void InputThread::run()
{
    std::string line;

    while (true)
    {
        std::getline(std::cin, line);

        emit commandReceived(QString::fromStdString(line));

        if (line == "exit")
        {
            break;
        }
    }
}
