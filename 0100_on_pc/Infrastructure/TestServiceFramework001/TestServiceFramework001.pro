QT += core
QT -= gui

CONFIG += c++11

TARGET = TestServiceFramework001
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

unix|win32: LIBS += -lserviceframework
