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

    Logger::instance().logInfo("Program started");

    FileManager *manager = new FileManager;

    cout << "=== File Tracking System ===\n";
    cout << "Commands:\n";
    cout << "  add <path>    - add file to tracking\n";
    cout << "  remove <path> - remove file from tracking\n";
    cout << "  list          - show all tracked files\n";
    cout << "  start         - start tracking\n";
    cout << "  stop          - stop tracking\n";
    cout << "  exit          - exit program\n\n";

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

        QString argument = line.section(' ', 1).trimmed();
        QString command = line.section(' ', 0, 0).toLower();

        if (command == "exit")
        {
            QMetaObject::invokeMethod(manager,"shutdown",Qt::BlockingQueuedConnection);
            QMetaObject::invokeMethod(manager, "destroyTrackedObjects", Qt::BlockingQueuedConnection);
            workerThread.quit();
            workerThread.wait();
            Logger::instance().logInfo("Program finished");
            break;
        }
        else if (command == "add")
        {
            if (argument.isEmpty())
            {
                Logger::instance().logError("Usage: add <path>");
                continue;
            }

            QMetaObject::invokeMethod(manager,"addFile",Qt::QueuedConnection,Q_ARG(QString, argument));
        }
        else if (command == "remove")
        {
            if (argument.isEmpty())
            {
                Logger::instance().logError("Usage: remove <path>");
                continue;
            }

            QMetaObject::invokeMethod(manager,"removeFile",Qt::QueuedConnection,Q_ARG(QString, argument));
        }
        else if (command == "list")
        {
            QMetaObject::invokeMethod(manager,"listFiles",Qt::QueuedConnection);
        }
        else if (command == "start")
        {
            QMetaObject::invokeMethod(manager,"startTracking",Qt::QueuedConnection);
        }
        else if (command == "stop")
        {
            QMetaObject::invokeMethod(manager,"stopTracking",Qt::QueuedConnection);
        }
        else
        {
            Logger::instance().logError("Unknown command: " + command);
        }
    }

    return 0;
}
