TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# gcc -o wayland-egl wayland-egl.c -lwayland-client -lwayland-egl -lEGL -lGL
#DEFINES += LINUX
DEFINES += WL_EGL_PLATFORM

INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include
INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/oip
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/HMI
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/HMI/api/no-ipc
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/HMI/api/api/bl
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/HMI/api/api/common
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/HMI/event/event
#INCLUDEPATH    *= $$(ADK_PREFIX)/armv7a-mv-linux/usr/include/prod/Radio/inc

HEADERS += \
    ivi-application-client-protocol.h \
    WaylandWindow.h \

SOURCES += \
    WaylandWindow.cpp \
    ivi-application-protocol.c \
    main.cpp



LIBS *= -L$$(ADK_PREFIX_HOST)/usr/lib \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/Radio \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/Audio \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/Conn \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/Vehicle \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/Media \
        -L$$(ADK_PREFIX_HOST)/usr/lib/prod/HMI \
        -L$$(ADK_PREFIX_HOST)/usr/lib \
        -lwayland-client -lpthread -lEGL -lGLESv2
