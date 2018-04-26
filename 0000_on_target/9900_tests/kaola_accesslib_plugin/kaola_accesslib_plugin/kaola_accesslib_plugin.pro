#-------------------------------------------------
#
# Project created by QtCreator 2018-01-19T17:53:59
#
#-------------------------------------------------

QT       -= gui

TARGET = kaola_accesslib_plugin
TEMPLATE = lib

use_static_plugin {
    CONFIG += staticlib
} else {
    CONFIG += dll
}

INCLUDEPATH += \
    ../pub/inc/

SOURCES += kaola_accesslib_plugin.cpp

HEADERS += \
    ../pub/inc/kaola_interface.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
