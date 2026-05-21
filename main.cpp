#include <QTextStream>
#include <QCoreApplication>
#include "manager.h"
#include "inputthread.h"
#include <iostream>
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    printHelp();

    FileManager& manager = FileManager::instance();
    InputThread inputThread;

    QObject::connect(&inputThread,&InputThread::commandReceived,&manager,&FileManager::processCommand);
    QObject::connect(&manager,&FileManager::shutdownRequested,&app,&QCoreApplication::quit);

    inputThread.start();
    int result = app.exec();

    inputThread.stop();
    inputThread.wait();
#ifdef QT_DEBUG
    std::cout << "Press Enter to close...";
    std::cin.get();
#endif
    return result;
}
