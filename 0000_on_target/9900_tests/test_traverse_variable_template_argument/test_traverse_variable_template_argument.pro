QT += core
QT -= gui

CONFIG += c++11

QMAKE_CXXFLAGS += -ftemplate-depth=10000

TARGET = test_traverse_variable_template_argument
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    myscreens.h
