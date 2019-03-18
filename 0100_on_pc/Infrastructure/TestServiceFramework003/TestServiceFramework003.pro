QT += core
QT -= gui

CONFIG += c++11

TARGET = TestServiceFramework003
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -DCOMALOG_NO_DLT

TEMPLATE = app

SOURCES += main.cpp

unix|win32: LIBS += -lserviceframework
