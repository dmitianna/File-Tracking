#include <QTextStream>
#include <QCoreApplication>
#include <QThread>

#include "manager.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream cin(stdin);
    QTextStream cout(stdout);

    FileManager *manager = new FileManager;

    cout << "=== File Tracking System ===\n";
    cout << "Commands:\n";
    cout << "  add <path>    - add file to tracking\n";
    cout << "  remove <path> - remove file from tracking\n";
    cout << "  list          - show all tracked files\n";
    cout << "  start         - start tracking\n";
    cout << "  stop          - stop tracking\n";
    cout << "  exit          - exit program\n";
    cout << "  Note: paths with spaces are not supported\n\n";

    QThread workerThread;
    manager->moveToThread(&workerThread);
    QObject::connect(&workerThread, &QThread::finished,
                     manager, &QObject::deleteLater);
    workerThread.start();

    while (true)
    {
        cout << "> ";
        cout.flush();

        QString line = cin.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(' ', Qt::SkipEmptyParts);

        QString command = parts[0].toLower();
        QString argument;

        if (command == "list" || command == "start" || command == "stop" || command == "exit")
        {
            if (parts.size() > 1)
            {
                Logger::instance().logError("Too many arguments for command: " + command);
                continue;
            }
        }

        else if (command == "add" || command == "remove")
        {
            if (parts.size() != 2)
            {
                Logger::instance().logError("Invalid number of arguments for command: " + command);
                Logger::instance().logError("Usage: add/remove <path>");
                continue;
            }

            argument = parts[1];
        }
        else
        {
            Logger::instance().logError("Unknown command: " + command);
            continue;
        }

        if (command == "exit")
        {
            QMetaObject::invokeMethod(manager,"shutdown",Qt::BlockingQueuedConnection);
            workerThread.quit();
            workerThread.wait();
            //Logger::instance().logInfo("Program finished");
            break;
        }
        else if (command == "add")
        {
            if (argument.isEmpty())
            {
                Logger::instance().logError("Usage: add <path>");
                continue;
            }

            QMetaObject::invokeMethod(manager,"addFile",Qt::BlockingQueuedConnection,Q_ARG(QString, argument));
        }
        else if (command == "remove")
        {
            if (argument.isEmpty())
            {
                Logger::instance().logError("Usage: remove <path>");
                continue;
            }

            QMetaObject::invokeMethod(manager,"removeFile",Qt::BlockingQueuedConnection,Q_ARG(QString, argument));
        }
        else if (command == "list")
        {
            QMetaObject::invokeMethod(manager,"listFiles",Qt::BlockingQueuedConnection);
        }
        else if (command == "start")
        {
            QMetaObject::invokeMethod(manager,"startTracking",Qt::BlockingQueuedConnection);
        }
        else if (command == "stop")
        {
            QMetaObject::invokeMethod(manager,"stopTracking",Qt::BlockingQueuedConnection);
        }
    }

    return 0;
}
