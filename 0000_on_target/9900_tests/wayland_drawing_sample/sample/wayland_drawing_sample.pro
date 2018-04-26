TEMPLATE = app
TARGET = simple_wayland_drawing

DESTDIR = ../bin

CONFIG  -= qt
CONFIG  += console
QT      -= core gui network

OE_PLATFORM = $$(OE_QMAKE_PLATFORM)

DEFINES += LINUX="1"
DEFINES += EGL_API_FB="1"
DEFINES += WL_EGL_PLATFORM="1"
DEFINES += TARGET_BUILD
DEFINES += VPCORE_V4_0

COMPILER_FLAGS  = -tarmv6  -c -pipe -fexpensive-optimizations -fomit-frame-pointer -frename-registers -O2 -fpermissive -g -Wall -W

LIBS *= -lEGL -lGLESv2 -L$${DESTDIR}

LIBS *= -L$$(ADK_PREFIX_HOST)/usr/lib \
    -lwayland-client -lpthread -lEGL

INCLUDEPATH *= $$(OE_QMAKE_INCDIR_QT)/wayland-ivi
INCLUDEPATH *= \
    ../include/common \
    ../include/vpcore \
    ../include/vpsm \
    src \
    inc \

HEADERS *= \
    src/log.h \

SOURCES *= \
    src/log.cpp \
    src/main.cpp \
    src/simple_touch_on_wayland.cpp \
