TEMPLATE = app

QT += qml quick widgets

DEFINES += PLATFORM_OVIP

LIBS += -lwayland-ivi-client -lwayland-ivi-shell-client

SOURCES += main.cpp \
    BLImageProvider.cpp \
    ImageDecoderAdaptor.cpp \
    WaylandController.cpp \
    WaylandIviShell.cpp \
    WaylandMonitor.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    main.h \
    BLImageProvider.h \
    ImageDecoderAdaptor.h \
    WaylandController.h \
    WaylandIviShell.h \
    WaylandListener.h \
    WaylandMonitor.h
