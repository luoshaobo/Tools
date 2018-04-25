TEMPLATE = app

TARGET = wayland_surface

QT += 
CONFIG += c++11

LIBS *= -L$$(ADK_PREFIX_HOST)/usr/lib \
    -lwayland-client -lpthread -lEGL

LIBS *= -lwayland-ivi-client
LIBS *= -lwayland-ivi-shell-client


INCLUDEPATH *= \
    . \
    src \
    src/winston \

SOURCES += \
    main.cpp \
    log.cpp \
    src/api-surface-gui.cpp \
    src/api-surface-data.cpp \
    src/winston/api-surface-bl-winston.cpp \
    src/winston/Bridge.cpp \
    src/winston/IviShell.cpp \
    src/winston/Listener.cpp \
    src/winston/Manager.cpp \
    src/winston/Properties.cpp \
    src/winston/Transaction.cpp \
    

HEADERS += \
    main.h \
    log.h \
    logger.h \
    api-surface-gui.h \
    api-surface-data.h \
    api-surface-bl.h \
    src/winston/Bridge.h \
    src/winston/IviShell.h \
    src/winston/Listener.h \
    src/winston/Manager.h \
    src/winston/Properties.h \
    src/winston/api-surface-bl-winston.cpp \
    src/winston/Transaction.h \
    
