#-------------------------------------------------
#
# Project created by QtCreator 2016-08-23T15:57:53
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = TestFPS
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    MonitorFPS.cpp

LIBS += -lwayland-ivi-shell-client -lwayland-ivi-client -lwayland-client
