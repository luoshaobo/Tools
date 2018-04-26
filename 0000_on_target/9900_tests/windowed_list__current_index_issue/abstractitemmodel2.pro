TARGET = abstractitemmodel2
QT += qml quick

CONFIG += c++11

HEADERS = model.h \
    api-windowed-list.h
SOURCES = main.cpp \
          model.cpp
RESOURCES += abstractitemmodel.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/quick/models/abstractitemmodel
INSTALLS += target
