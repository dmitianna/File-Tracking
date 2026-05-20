QT += core
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
        inputthread.cpp \
        logger.cpp \
        main.cpp \
        manager.cpp \
        timerefresher.cpp \
        trackedfile.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    IRefresher.h \
    inputthread.h \
    logger.h \
    manager.h \
    timerefresher.h \
    trackedfile.h
