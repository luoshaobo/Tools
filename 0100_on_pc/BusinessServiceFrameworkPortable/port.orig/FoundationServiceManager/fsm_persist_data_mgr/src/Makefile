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
# Author:       Maksym Mozok
# Date:         27-Feb-2017
# Description:  This makefile is used to clean and make libfsm_persist_data_mgr
#
#################################################################

packages := glib-2.0 automotive-dlt libcas-pcl

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I ../include `pkg-config --cflags $(packages)`
INCLUDES += -I $(CAS_TARGETFS)/usr/include

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -std=c++0x -Wpointer-arith -fPIC

LDFLAGS += -lstdc++ `pkg-config --libs $(packages)`

OBJECTS := \
	fsm_persist_data_mgr.o

LIBRARY := libfsm_persist_data_mgr.so
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###

$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

### CLEAN ###
clean:
	$(RM) *.o
	$(RM) $(LIBRARY)

### INSTALL ### 644
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm
	$(INSTALL) --mode=755 ../include/fsm_persist_data_mgr.h $(buildroot)/usr/include/fsm
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=644 libfsm_persist_data_mgr.pc $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=755 -dv $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 ../persistence_config_fsm_ssm.xml $(buildroot)$(CAS_PERS_CONFIG_DIR)

