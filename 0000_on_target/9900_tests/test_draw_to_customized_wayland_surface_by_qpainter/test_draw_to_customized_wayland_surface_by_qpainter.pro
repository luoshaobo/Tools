QT += core

CONFIG += c++11
CONFIG += console

TEMPLATE = app

SOURCES += main.cpp \
    simple_touch_on_wayland.cpp \
    log.cpp

HEADERS += \
    main.h \
    log.h

OE_PLATFORM = $$(OE_QMAKE_PLATFORM)

DEFINES += LINUX="1"
DEFINES += EGL_API_FB="1"
DEFINES += WL_EGL_PLATFORM="1"
DEFINES += TARGET_BUILD

LIBS *= -lEGL -lGLESv2 -L$${DESTDIR}

LIBS *= -L$$(ADK_PREFIX_HOST)/usr/lib \
    -lwayland-client -lpthread -lEGL

INCLUDEPATH *= $$(OE_QMAKE_INCDIR_QT)/wayland-ivi
