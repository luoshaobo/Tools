#################################################################
# Author:       Iulian Sirghi
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               libfscfg component
#################################################################

packages := automotive-dlt glib-2.0 gio-2.0 gio-unix-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INTERFACE_PATH=../../interface

INCLUDES += -I../../interface/public/include \
            -I../../interface/protected/fscfg_common \
	    -I../../interface/protected \
	    -I../../interface/private \
            -I $(CAS_TARGETFS)/usr/include

INCLUDES += `pkg-config --cflags $(packages)`

LIBRARY_PATHS += -L../fscfg_common \

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS +=-Wall -Werror -std=c++11 -fPIC $(INCLUDES)
LDFLAGS += -lfscfg_common -lstdc++ `pkg-config --libs $(packages)` -shared

LIBRARY := libfsm_fscfg.so
VERSION := 0.0.1

SOURCES := $(wildcard *.cpp)

OBJECTS = $(SOURCES:%.cpp=%.o)

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CROSS)gcc $(LOC_CFLAGS) -o $@ $(LIBRARY_PATHS) $^ $(LDFLAGS) -Wl,-soname,$@

%.o: %.cpp
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.cpp} -o $@

clean:
	$(RM) $(OBJECTS)
	$(RM) $(LIBRARY)

install:
	# library
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	# package config
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_fscfg.pc $(buildroot)/usr/lib/pkgconfig
	# headers
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/fscfg
	$(INSTALL) -m 644 $(INTERFACE_PATH)/public/include/fscfg/*.h $(buildroot)/usr/include/fsm/fscfg/

