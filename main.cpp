#include <QTextStream>
#include <QCoreApplication>

#include <atomic>
#include <thread>

#include "manager.h"
#include "logger.h"

static void printHelp()
{
    QTextStream cout(stdout);

    cout << "=== File Tracking System ===\n";
    cout << "Commands:\n";
    cout << "  add <path>    - add file to tracking\n";
    cout << "  remove <path> - remove file from tracking\n";
    cout << "  list          - show all tracked files\n";
    cout << "  start         - start tracking\n";
    cout << "  stop          - stop tracking\n";
    cout << "  exit          - exit program\n";
    cout << "Note: paths with spaces are not supported\n\n";
    cout.flush();
}

void handleCommand(const QString &input)
{
    QString line = input.trimmed();
    if (line.isEmpty())
        return;

    FileManager &manager = FileManager::instance();

    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    QString command = parts[0].toLower();
    QString argument;

    if ((command == "list" || command == "start" || command == "stop" || command == "exit") && parts.size() > 1)
    {
        Logger::instance().logError("Too many arguments for command: " + command);
        return;
    }

    if ((command == "add" || command == "remove") && parts.size() != 2)
    {
        Logger::instance().logError("Invalid number of arguments for command: " + command);
        Logger::instance().logError("Usage: add/remove <path>");
        return;
    }

    if (command == "add" || command == "remove")
        argument = parts[1];

    if (command == "add") manager.addFile(argument);
    else if (command == "remove") manager.removeFile(argument);
    else if (command == "list") manager.listFiles();
    else if (command == "start") manager.startTracking();
    else if (command == "stop") manager.stopTracking();
    else if (command == "exit")
    {
        QCoreApplication::quit();
    }
    else
    {
        Logger::instance().logError("Unknown command: " + command);
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    printHelp();

    std::atomic_bool running = true;
    std::thread consoleThread([&running]()
    {
    QTextStream cin(stdin);

    while (running)
    {
        QString line = cin.readLine();
        // EOF (Ctrl+D / Ctrl+Z)
        if (line.isNull()){
            QMetaObject::invokeMethod(qApp,&QCoreApplication::quit,Qt::QueuedConnection);
            running = false;
            break;
        }

        QString cmd = line.trimmed().toLower();
        QMetaObject::invokeMethod(qApp,[line](){handleCommand(line);},Qt::QueuedConnection);

        if (cmd == "exit") running = false;
    }
    });

    int result = app.exec();

    running = false;

    if (consoleThread.joinable()) consoleThread.join();
    return result;
}
