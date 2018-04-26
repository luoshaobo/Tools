#-------------------------------------------------
#
# Project created by QtCreator 2017-10-25T17:06:20
#
#-------------------------------------------------

QT       -= core gui

TARGET = injected_so
TEMPLATE = lib

DEFINES += INJECTED_SO_LIBRARY

SOURCES += \
    injected_so_for_fd.cpp \
    TK_sock.cpp \
    TK_Tools.cpp

QMAKE_CFLAGS += -D_GNU_SOURCE -std=gnu99

QMAKE_LFLAGS += -shared -fPIC -pthread

HEADERS += \
    TK_sock.h \
    TK_Tools.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
