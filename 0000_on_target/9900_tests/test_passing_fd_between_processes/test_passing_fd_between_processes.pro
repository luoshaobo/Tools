QT += core
QT -= gui

CONFIG += c++11

TARGET = test_passing_fd_between_processes
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    TK_sock.cpp \
    TK_Tools.cpp

HEADERS += \
    TK_sock.h \
    TK_Tools.h
