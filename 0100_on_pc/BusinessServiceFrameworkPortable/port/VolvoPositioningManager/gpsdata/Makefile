#################################################################
# Copyright (C) 2018 Continental Automotive AG and Subsidiary
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#################################################################

CC = $(CROSS)gcc
AR=$(CROSS)ar

INSTALL = /usr/bin/install

INCLUDES += `pkg-config --cflags $(packages)` -I../include
INCLUDES += -I ../../../../release/fs/devel/usr/include
override CFLAGS += -g -fno-strict-aliasing -Wall -Wpointer-arith -fPIC
CFLAGS += -DDISABLE_DEBUG_LOG

LDFLAGS += `pkg-config --libs $(packages)` -lpthread -lm
OBJECTS = gpsdata.o

LIBRARY := libgpsdata.a
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###
$(LIBRARY): $(OBJECTS)
	$(AR) rsu $(LIBRARY) $(OBJECTS) 

### OBJECTS ###
$(OBJECTS):
	$(CC) $(CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@

### CLEAN ###
clean:
	rm -f *.o $(LIBRARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
