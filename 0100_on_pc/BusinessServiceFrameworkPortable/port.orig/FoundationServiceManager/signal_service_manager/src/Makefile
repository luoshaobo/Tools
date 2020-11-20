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
# Date:         01-Feb-2017
# Description:  This makefile is used to clean and make
#               FoundationServiceManager's SignalServiceManager module
#################################################################

packages := comaclient automotive-dlt libusermanager libcas-pcl glib-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I $(CAS_TARGETFS)/usr/include -I ../include -I ../../fsm_ccm/include -I ../../fsm_ccm/src/generated -I ../../fsm_persist_data_mgr/include
INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -std=c++11 -Wpointer-arith -fPIC

LDFLAGS += -lstdc++ -ggdb  -ldl `pkg-config --libs $(packages)`

OBJECTS := ssm_runningclients.o ssm.o

LIBRARY := libfsm_ssm.so
VERSION := 0.0.1

all: $(LIBRARY)

### LIBRARY ###
$(LIBRARY): $(OBJECTS)
	$(CC) $(LDFLAGS)  -shared -Wl,-soname,$@ -o $@ $^

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

### CLEAN ###
clean:
	$(RM) *.o
	$(RM) $(LIBRARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) -m 755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_ssm.pc $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm_ssm
	$(INSTALL) -m 644 ../include/ssm.h $(buildroot)/usr/include/fsm_ssm
	$(INSTALL) -m 644 ../include/ssm_global.h $(buildroot)/usr/include/fsm_ssm
	$(INSTALL) -m 644 ../include/ssm_runningclients.h $(buildroot)/usr/include/fsm_ssm
