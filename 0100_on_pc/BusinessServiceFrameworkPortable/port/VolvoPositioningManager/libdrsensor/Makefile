#################################################################
# Copyright (C) 2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#################################################################

CC = $(CROSS)gcc

packages := glib-2.0 automotive-dlt ipcb_client tpsys gio-2.0 dbus-1 ipcb_client

INSTALL = /usr/bin/install

INCLUDES += `pkg-config --cflags $(packages)` -I../include
INCLUDES += -I ../../../../release/fs/devel/usr/include
override CFLAGS += -g -fno-strict-aliasing -Wall -Wpointer-arith -fPIC
CFLAGS += -DDISABLE_DEBUG_LOG

LDFLAGS += `pkg-config --libs $(packages)` -lpthread 
OBJECTS = dr_sensor.o
OBJECTS2 = dr_whlinfo.o

LIBRARY := libcas-drsensor.so

all: $(LIBRARY)

### BINARIES ###
$(LIBRARY): $(OBJECTS) $(OBJECTS2)
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@  $^ -lipcb_gdbus_client_proxy

### OBJECTS ###
$(OBJECTS):
	$(CC) $(CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@

$(OBJECTS2):
	$(CC) $(CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@
### CLEAN ###
clean:
	rm -f *.o $(LIBRARY)

### INSTALL ###
install:
	$(INSTALL) --mode=755 -dv $(buildroot)$(CONTI_IPR_DIR)/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)$(CONTI_IPR_DIR)/lib
	$(INSTALL) --mode=755 -dv $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 cfg/locationmgr_plugins_config.xml.pers-override $(buildroot)$(CAS_PERS_CONFIG_DIR)
	$(INSTALL) --mode=644 cfg/persistence_config_locationmgr-override.xml $(buildroot)$(CAS_PERS_CONFIG_DIR)
