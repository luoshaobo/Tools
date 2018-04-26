QT += core
QT -= gui

CONFIG += c++11

TARGET = udev_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    udev_test_1.cpp \
    udev_test_2.cpp

QMAKE_LFLAGS += -ludev
