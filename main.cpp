#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include "manager.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cin(stdin);
    QTextStream cout(stdout);

    Logger::instance().logInfo("Program started");

    cout << "=== File Tracking System ===\n";
    cout << "Commands:\n";
    cout << "  add <path>    - add file to tracking\n";
    cout << "  remove <path> - remove file from tracking\n";
    cout << "  list          - show all tracked files\n";
    cout << "  start         - start tracking\n";
    cout << "  stop          - stop tracking\n";
    cout << "  exit          - exit program\n\n";

    bool isTracking = false;

    while (true)
    {
        cout << "> ";
        cout.flush();

        QString line = cin.readLine();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        QString command = parts[0].toLower();

        if (command == "exit")
        {
            Logger::instance().logInfo("Program finished");
            break;
        }
        else if (command == "add" && parts.size() > 1)
        {
            FileManager::instance().addFile(parts[1]);
        }
        else if (command == "remove" && parts.size() > 1)
        {
            FileManager::instance().removeFile(parts[1]);
        }
        else if (command == "list")
        {
            FileManager::instance().listFiles();
        }
        else if (command == "start")
        {
            if (FileManager::instance().fileCount() == 0)
            {
                Logger::instance().logError("No files to track. Add files first.");
                continue;
            }

            isTracking = true;
            Logger::instance().logInfo("Tracking started for " +
                                       QString::number(FileManager::instance().fileCount()) + " files");

            while (isTracking)
            {
                for (TrackedFile* file : qAsConst(FileManager::instance().files()))
                {
                    file->checkForChanges();
                }

                QCoreApplication::processEvents();
                QThread::msleep(100);
            }
        }
        else if (command == "stop")
        {
            if (isTracking)
            {
                isTracking = false;
                Logger::instance().logInfo("Tracking stopped");
            }
            else
            {
                Logger::instance().logError("Tracking is not running");
            }
        }
        else
        {
            Logger::instance().logError("Unknown command: " + command);
        }
    }

    return 0;
}
