##########################################################################
# Copyright (C) 2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##########################################################################

packages := automotive-dlt gio-2.0 gio-unix-2.0 glib-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I ../../include/voc_dbus_service -I ../generated
INCLUDES += `pkg-config --cflags $(packages)`
INCLUDES += -I $(CAS_TARGETFS)/usr/include

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS +=-Wall -Werror $(INCLUDES)
LDFLAGS += `pkg-config --libs $(packages)`

LDFLAGS += -lstdc++ -L ../../src/vo -lm

EXECUTABLE := voc_dbus_client
VERSION := 0.0.1

SOURCES := voc_dbus_client.cc
VOC_DBUS_SOURCES = $(wildcard ../generated/*.c)
C_SOURCES += $(VOC_DBUS_SOURCES)

OBJECTS = $(SOURCES:%.cc=%.o)
C_OBJECTS = $(C_SOURCES:%.c=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(C_OBJECTS)
	$(CC) -Wall -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.cc} -o $@

%.o: %.c
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.c} -o $@

clean:
	$(RM) $(OBJECTS) $(C_OBJECTS)
	$(RM) $(EXECUTABLE)

install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(EXECUTABLE) $(buildroot)/usr/bin
