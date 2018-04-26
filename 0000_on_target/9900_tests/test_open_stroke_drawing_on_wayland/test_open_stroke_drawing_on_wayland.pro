QT += core
#QT -= gui

DEFINES += LINUX="1"
DEFINES += EGL_API_FB="1"
DEFINES += WL_EGL_PLATFORM="1"

CONFIG += c++11

TARGET = test_open_stroke_drawing_on_wayland
CONFIG += console
CONFIG -= app_bundle

LIBS *= -lEGL -lGLESv2
LIBS *= -L$$(ADK_PREFIX_HOST)/usr/lib \
    -lwayland-client -lpthread -lEGL

TEMPLATE = app

SOURCES += main.cpp \
    sample_gl_on_wayland.cpp \
    GLApp.cpp \
    input-reader.cpp

HEADERS += \
    GLApp.h \
    input-reader.h
