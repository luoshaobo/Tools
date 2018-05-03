#-------------------------------------------------
#
# Project created by QtCreator 2015-10-26T18:10:35
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = qserver
CONFIG   += console
CONFIG   -= app_bundle

CONFIG += c++11

TEMPLATE = app

linux-g++ {
    QMAKE_CXXFLAGS += -DUSE_SESSION_BUS
}

INCLUDEPATH += ../../generated

HEADERS = \
    Frobber.h \
    ../../generated/NetCorpMyAppFrobberAdaptor.h

SOURCES += \
    qserver.cpp \
    Frobber.cpp \
    ../../generated/NetCorpMyAppFrobberAdaptor.cpp
