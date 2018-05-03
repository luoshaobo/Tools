#-------------------------------------------------
#
# Project created by QtCreator 2015-10-21T19:44:17
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = qclient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

linux-g++ {
    QMAKE_CXXFLAGS += -DUSE_SESSION_BUS
}

INCLUDEPATH += ../../generated

HEADERS = \
    qclient.h \
    ../../generated/NetCorpMyAppFrobberProxy.h

SOURCES += \
    qclient.cpp \
    ../../generated/NetCorpMyAppFrobberProxy.cpp

