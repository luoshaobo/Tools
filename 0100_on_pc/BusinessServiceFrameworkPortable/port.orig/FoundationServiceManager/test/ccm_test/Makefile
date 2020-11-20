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
#################################################################

packages := automotive-dlt openssl libusermanager

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I $(CAS_TARGETFS)/usr/include -I ../../fsm_ccm/include/ -I ../../fsm_ccm/src/generated -I ../../service_discovery/daemon/
INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith

LDFLAGS += -lstdc++ `pkg-config --libs $(packages)` -lm -lrt -rdynamic

OBJECTS = ccm_test.o \
          ../../fsm_ccm/src/fsm.o \
          ../../fsm_ccm/src/fsm_ccm.o

#pick up all ASN1C c files excluding the sample executable
ASN1C_OBJECTS = $(filter-out %converter-sample.o,$(patsubst %.c, %.o, $(wildcard ../../fsm_ccm/src/generated/*.c)))
OBJECTS += $(ASN1C_OBJECTS)

BINARY := ccm_test

all: $(BINARY)

### BINARIES ###
$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

### CLEAN ###
clean:
	$(RM) $(OBJECTS)
	$(RM) $(BINARY)

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(BINARY) $(buildroot)/usr/bin
