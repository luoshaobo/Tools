QT += core
#QT -= gui

CONFIG += c++11

TARGET = test_draw_to_fb1_yuv
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mxc_ipudev_test.c \
    utils.c

HEADERS += \
    main.h \
    mxc_ipudev_test.h


QMAKE_LFLAGS += -lg2d
