QT += core
QT -= gui

CONFIG += c++11

TARGET = test_TypeCollection
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

QMAKE_CXXFLAGS += -ftemplate-depth=10000

HEADERS += \
    mytypes.h
