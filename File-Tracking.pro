QT += core
CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
        fileentity.cpp \
        logger.cpp \
        main.cpp \
        manager.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    fileentity.h \
    logger.h \
    manager.h
