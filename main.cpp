#include <QTextStream>
#include <QVector>
#include <QFileInfo>
#include <thread>
#include <chrono>
#include <atomic>
#include "fileentity.h"
#include "manager.h"
#include "logger.h"

std::atomic<bool> trackingActive(false);

void trackingThreadFunction()
{
    while (trackingActive)
    {
        for (TrackedFile* file : FileManager::instance().files())
        {
            file->checkForChanges();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
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

    std::thread trackingThread;

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
            if (trackingActive)
            {
                trackingActive = false;
                if (trackingThread.joinable())
                    trackingThread.join();
            }
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

            if (!trackingActive)
            {
                trackingActive = true;
                trackingThread = std::thread(trackingThreadFunction);
                Logger::instance().logInfo("Tracking started for " +
                                           QString::number(FileManager::instance().fileCount()) + " files");
            }
            else
            {
                Logger::instance().logError("Tracking already running");
            }
        }
        else if (command == "stop")
        {
            if (trackingActive)
            {
                trackingActive = false;
                if (trackingThread.joinable())
                    trackingThread.join();
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
