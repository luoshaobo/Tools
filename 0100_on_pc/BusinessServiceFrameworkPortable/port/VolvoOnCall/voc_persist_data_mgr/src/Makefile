#################################################################
# Copyright (C) 2018 Continental Automotive GmbH
# Author:       linhaixia
# Date:         01-08-2018
# Description:  This makefile is used to clean and make libvoc_persist_data_mgr
#
#################################################################

packages := glib-2.0 automotive-dlt libcas-pcl

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I ../include `pkg-config --cflags $(packages)`
INCLUDES += -I $(CAS_TARGETFS)/usr/include

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -std=c++0x -Wpointer-arith -fPIC $(INCLUDES)

LDFLAGS += -lstdc++ `pkg-config --libs $(packages)`

OBJECTS := \
	voc_persist_data_mgr.o

LIBRARY := libvoc_persist_data_mgr.so
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###

$(LIBRARY): $(OBJECTS)
	$(CC) $(LOC_CFLAGS) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^

### OBJECTS ###
$(OBJECTS):
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@ $^

### CLEAN ###
clean:
	rm -f *.o
	rm -f *.so
	rm -f $(BINARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=755 -dv $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 ../persistence_config_voc.xml $(buildroot)$(CAS_PERS_CONFIG_DIR)
