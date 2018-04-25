QT += core
QT += gui

CONFIG += c++11

TARGET = image_convert
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    TK_Bitmap.cpp \
    TK_Tools.cpp

HEADERS += \
    TK_Bitmap.h \
    TK_Tools.h
