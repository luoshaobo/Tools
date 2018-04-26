QT += core
QT -= gui

CONFIG += c++11

TARGET = hmi_app_sample
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../pub/inc/

SOURCES += main.cpp

HEADERS += \
    ../pub/inc/kaola_interface.h
    
use_static_plugin {
    DEFINES += USE_STATIC_PLUGIN
    
    LIBS += -L../kaola_accesslib_plugin -lkaola_accesslib_plugin
}
