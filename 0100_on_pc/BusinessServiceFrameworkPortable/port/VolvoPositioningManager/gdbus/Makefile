#*
#* Copyright (C) 2017 Continental Automotive GmbH
#*
#* Licensed under the "Supplier unrestricted white box" category, as
#* described in the 20160322-1_SW-amendment TCAM.pdf agreement
#* between Volvo Cars and Continental Automotive GmbH.
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*

GDBUS_CODEGEN = "${CAS_HOSTFS}/usr/bin/gdbus-codegen"

#we use this to do pkg-config for includes and ld_flags
packages := automotive-dlt tpsys glib-2.0 dbus-1 ipcb_client libcas-hal libcas-pcl

CC = $(CROSS)gcc
AR=$(CROSS)ar
INSTALL = /usr/bin/install
INCLUDES += -I $(CAS_TARGETFS)/usr/include `pkg-config --cflags $(packages)`
INCLUDES += -I ./client_proxy
INCLUDES += -I ./
INCLUDES += -I ../interface/
INCLUDES += -I ../include
LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += $(INCLUDES) -O2 -Wall -Werror -Wpointer-arith
INCLUDE_DIR = .
DBUS_GEN = vpom-ifc-generated

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

OBJECTS = vpom_gdbus.o
OBJECTS_GEN = $(DBUS_GEN).o

LDFLAGS += -L./lib -lstdc++ `pkg-config --libs $(packages)` -lsystemd
LDFLAGS += -L./release/fs/devel/usr/lib/

LIBRARY := libvpom_gdbus.a
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###
$(LIBRARY): $(OBJECTS_GEN) $(OBJECTS)
	$(AR) rsu $(LIBRARY) $(OBJECTS_GEN) $(OBJECTS)

### OBJECTS ###
$(OBJECTS_GEN):%.o:%.c $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) $(LOC_CFLAGS) $(LDFLAGS) -c ${@:.o=.c} -o $@
	@$(POSTCOMPILE)

$(OBJECTS):%.o:%.cpp $(DBUS_GEN).c $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) $(LOC_CFLAGS) $(LDFLAGS) -c ${@:.o=.cpp} -o $@
	@$(POSTCOMPILE)

$(DBUS_GEN).c : $(INCLUDE_DIR)/$(DBUS_GEN).h

$(INCLUDE_DIR)/$(DBUS_GEN).h:
	$(shell ${GDBUS_CODEGEN} --interface-prefix com.contiautomotive.tcam.vpom --generate-c-code $(DBUS_GEN) --c-namespace vpom --c-generate-object-manager ${PWD}/vpom_server_gdbus.xml)

### CLEAN ###
clean:
	rm -f *.o
	rm -f $(LIBRARY)
	rm -rf $(DEPDIR)
	rm $(DBUS_GEN).c $(DBUS_GEN).h

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/lib
	$(INSTALL) -m 755 $(LIBRARY) $(buildroot)/usr/lib
	$(INSTALL) -d -m 755 $(buildroot)/etc/dbus-1/system.d
	cp com.contiautomotive.tcam.vpom.conf $(buildroot)/etc/dbus-1/system.d

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS_GEN) $(OBJECTS))))
