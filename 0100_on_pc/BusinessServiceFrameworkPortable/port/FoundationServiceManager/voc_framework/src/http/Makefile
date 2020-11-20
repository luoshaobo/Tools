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
# Date:         05-June-2017
# Description:  This makefile is used to clean and make
#               FoundationServiceManager's volvo on call framework
#################################################################

packages := libusermanager comaclient conti-nadif openssl automotive-dlt comaipc
packages += glib-2.0 gobject-2.0 gio-unix-2.0 gio-2.0 tpsys

$(warning make http_test begin)

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I .
INCLUDES += `pkg-config --cflags $(packages)`
INCLUDES += -I baseLogSystem/logInterface
INCLUDES += -I $(CAS_TARGETFS)/usr/include
INCLUDES += -I ../../../../opensource
INCLUDES += -I ../../../CommunicationManager/include/communicationmanager

LOC_CFLAGS := $(CFLAGS) -Wno-error=unused-variable -Wno-error=sign-compare
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith -fPIC

LOC_CCFLAGS := -std=c++11

LDFLAGS += -lstdc++  -lm `pkg-config --libs $(packages)` -lprotobuf
$(warning LDFLAGS = $(LDFLAGS))

OBJECTS := http_get.o
OBJECTS += http_get_test.o

$(warning OBJECTS = $(OBJECTS))

BINARY := http_test
VERSION := 0.0.1

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(STATIC_LINK_LIBRARIES)

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

%.o:%.cc
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@

%.o: %.c
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c $< -o $@

### CLEAN ###
clean:
	$(RM) $(OBJECTS)
	$(RM) $(BINARY)

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/tst/bin
	#$(INSTALL) -m 755 $(BINARY) $(buildroot)/tst/bin

