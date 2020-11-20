#################################################################
# Copyright (C) 2017 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
# Author:       Axel Fagerstedt
# Date:         26-Apr-2017
# Description:  This makefile is used to clean and make
#               Usermanager modules
#################################################################

packages := openssl sqlite3 automotive-dlt glib-2.0 libcas-pcl

INSTALL = /usr/bin/install
CC = $(CROSS)gcc

INCLUDES += -I ../include -I $(CAS_TARGETFS)/usr/include
INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS) -Wno-error=deprecated-declarations
LOC_CFLAGS += -g -Wall -Werror -std=c++11 -Wpointer-arith -fPIC

LDFLAGS += `pkg-config --libs $(packages)`
LDFLAGS += -ggdb -lstdc++ -ldl

OBJECTS := usermanager_interface.o usermanager.o keystore.o
LIBRARY := libfsm_usermanager.so

VERSION := 0.0.1

all: $(LIBRARY)

### LIBRARIES ###
$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^

### OBJECTS ###
%.o:%.cc
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@

### CLEAN ###
clean:
	$(RM) *.o
	$(RM) $(LIBRARY)

### INSTALL ###
install:
	# library
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	# package config
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libusermanager.pc $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_usermanager.pc $(buildroot)/usr/lib/pkgconfig
	# includes
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/usermanager
	$(INSTALL) -m 644 ../include/*.h $(buildroot)/usr/include/fsm/usermanager
	# persistence
	$(INSTALL) --mode=755 -dv $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 ../config/persistence_config_fsm_keystore.xml $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 ../config/keystore.sqlite $(buildroot)$(CAS_PERS_CONFIG_DIR)
