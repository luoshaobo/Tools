#################################################################
# Copyright (C) 2017-2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
# Author:       Piotr Tomaszewski
# Date:         17-Nov-2016
# Description:  This makefile is used to clean and make transfermanager
#
#################################################################

packages := automotive-dlt libcas-pcl conti-nadif comaclient conti-psapcall
packages += sqlite3 openssl glib-2.0 gio-2.0 gio-unix-2.0 libxml-2.0 gobject-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I $(CAS_TARGETFS)/usr/include -I ../include/ -I ../../fsm_persist_data_mgr/include -I ./generated `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith -fPIC $(INCLUDES)

LDFLAGS += -lstdc++ -L../../fsm_persist_data_mgr/src -lfsm_persist_data_mgr `pkg-config --libs $(packages)` -lstdc++fs

VERSION := 0.0.1

LIBRARY := libfsm_transfermanager.so

BINARY := dbcmdtool

CPP_OBJECTS :=  default_cache.o message.o request.o response.o transfer_manager.o transfer_response.o cache_utils.o

BIN_OBJECTS := http_cache_db.o dbcmdtool.o

OBJECTS := $(CPP_OBJECTS) http_cache_db.o

all: $(LIBRARY) $(BINARY)
### Transfer Manager ###
$(LIBRARY): $(OBJECTS)
	$(CC) $(LOC_CFLAGS) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^ -lstdc++fs

$(BINARY): $(BIN_OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ -lstdc++fs

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@ -lstdc++fs
%.o:%.cc
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@ -lstdc++fs

build_libfsm_transfermanager/libfsm_transfermanager.so: build_libfsm_transfermanager/Makefile .PHONY
	$(MAKE) -C build_libfsm_transfermanager VERBOSE=1

.PHONY: clean all install;

### CLEAN ###
clean:
	$(RM) *.o
	$(RM) $(BINARY)
	$(RM) $(LIBRARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) -m 755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_transfermanager.pc $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm_transfermanager
	$(INSTALL) -m 644 ../include/*.h $(buildroot)/usr/include/fsm_transfermanager
	$(INSTALL) --mode=755 -dv $(buildroot)$(CAS_PERS_CONFIG_DIR)

