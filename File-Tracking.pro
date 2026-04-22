QT += core
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
        logger.cpp \
        main.cpp \
        manager.cpp \
        trackedfile.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    logger.h \
    manager.h \
    trackedfile.h
