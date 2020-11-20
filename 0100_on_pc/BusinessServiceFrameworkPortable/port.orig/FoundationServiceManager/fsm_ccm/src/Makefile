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
# Author:       Piotr Tomaszewski
# Date:         17-Nov-2016
# Description:  This makefile is used to clean and make libfsm
#
#################################################################

packages := automotive-dlt openssl gio-2.0 gio-unix-2.0 libxml-2.0 gobject-2.0 libusermanager

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I $(CAS_TARGETFS)/usr/include -I ../include/ -I ./generated `pkg-config --cflags $(packages)`

#LOC_CFLAGS := $(CFLAGS) -Wno-error=deprecated-declarations
LOC_CFLAGS := $(CFLAGS) -Wno-deprecated-declarations
LOC_CFLAGS += -Wno-error=unused-variable -g -Wall -Werror -Wpointer-arith -fPIC $(INCLUDES)

LDFLAGS += -lstdc++ `pkg-config --libs $(packages)`

C_OBJECTS = $(C_SOURCES:%.c=%.o)
C_SOURCES = \
            fsm.c

#pick up all ASN1C c files excluding the sample executable
ASN1C_SOURCES = $(filter-out %converter-sample.c,$(wildcard generated/*.c))
C_SOURCES += $(ASN1C_SOURCES)


CPP_OBJECTS = $(CPP_SOURCES:%.cpp=%.o)
CPP_SOURCES = \
              xml.cpp

CC_OBJECTS = $(CC_SOURCES:%.cc=%.o)
CC_SOURCES = \
            fsm_ccm.cc

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS) $(CC_OBJECTS)

LIBRARY := libfsm_ccm.so
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###

$(LIBRARY): $(CC_OBJECTS) $(CPP_OBJECTS) $(C_OBJECTS)
	$(CC) $(LOC_CFLAGS) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^


.PHONY: ;

### OBJECTS ###
%.o: %.c
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cc
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.cpp
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c $< -o $@


### CLEAN ###
clean:
	$(RM) $(CC_OBJECTS) $(CPP_OBJECTS) $(C_OBJECTS)
	$(RM) $(LIBRARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) -m 755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_ccm.pc $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm_ccm
	$(INSTALL) -m 644 ../include/fsm.h $(buildroot)/usr/include/fsm_ccm
	$(INSTALL) -m 644 ../include/fsm_ccm.h $(buildroot)/usr/include/fsm_ccm
	$(INSTALL) -m 644 ../include/xml.hpp $(buildroot)/usr/include/fsm_ccm
