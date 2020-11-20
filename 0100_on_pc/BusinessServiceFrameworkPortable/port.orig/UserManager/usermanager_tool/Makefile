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
#               Usermanager commandline tool modules
#################################################################

packages := openssl sqlite3 automotive-dlt glib-2.0 libcas-pcl

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I ../include -I $(CAS_TARGETFS)/usr/include
INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith  $(INCLUDES)

LDFLAGS += $(CFLAGS)
LDFLAGS += -L../src/ -lfsm_usermanager -lstdc++ `pkg-config --libs $(packages)` -lm -lrt -rdynamic

OBJECTS = usermanager_tool.o quark.o
BINARY := usermanager_tool

all: $(BINARY)

### BINARIES ###
$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.cpp} -o $@

### CLEAN ###
clean:
	$(RM) *.o
	$(RM) $(BINARY)

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(BINARY) $(buildroot)/usr/bin
