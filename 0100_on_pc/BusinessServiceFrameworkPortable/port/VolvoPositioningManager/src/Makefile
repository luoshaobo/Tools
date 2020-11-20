#*
#* Copyright (C) 2018 Continental Automotive GmbH
#*
#* Licensed under the "Supplier unrestricted white box" category, as
#* described in the 20160322-1_SW-amendment TCAM.pdf agreement
#* between Volvo Cars and Continental Automotive GmbH.
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*

packages := automotive-dlt tpsys glib-2.0 gio-2.0 dbus-1 local_config ipcb_client vc_proxy
packages += libcas-lifecycle-mgr libcas-pcl

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I $(CAS_TARGETFS)/usr/include -I ../include `pkg-config --cflags $(packages)`
INCLUDES += -I ../gdbus/
INCLUDES += -I ../gdbus/client_proxy/
INCLUDES += -I ../interface/
INCLUDES += -I ../gpsdata/
LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -O2 -Wall -Werror -Wpointer-arith $(INCLUDES)

#Find out last VPOM git patch and print it out when starting VPOM
GIT_VERSION="$(shell git log -1 --pretty=format:'%h')"
LOC_CFLAGS += -DVPOM_GIT_VERSION=\"${GIT_VERSION}\"

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

OBJECTS = \
	vpom.o \
	vpom_lcm.o

	LDFLAGS += -L./lib -lstdc++ `pkg-config --libs $(packages)` -lsystemd -lcas-locationmgr 
	LDFLAGS += -L../gdbus/ -lvpom_gdbus -lm
	LDFLAGS += -L../gpsdata/ -lgpsdata
	###-lgpsdata

BINARY := vpom

all: $(BINARY)

### BINARIES ###
$(BINARY): $(OBJECTS)
#	pkg-config --libs --debug $(packages)
	$(CC) $(LOC_CFLAGS) $^ $(LDFLAGS) -o $@

### OBJECTS ###
%.o:%.c
%.o:%.c $(DEPDIR)/%.d
#	pkg-config --libs --debug $(packages)
	$(CC) $(DEPFLAGS) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@
	@$(POSTCOMPILE)

%.o:%.cpp
%.o:%.cpp $(DEPDIR)/%.d
#	pkg-config --libs --debug $(packages)
	$(CC) $(DEPFLAGS) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@
	@$(POSTCOMPILE)

### CLEAN ###
clean:
	rm -f *.o
	rm -f $(BINARY)
	rm -rf $(DEPDIR)

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(BINARY) $(buildroot)/usr/bin
	$(INSTALL) -d -m 755 $(buildroot)/lib/systemd/system
	cp vpom.service $(buildroot)/lib/systemd/system
	$(INSTALL) -d -m 744 $(buildroot)/lib/systemd/system/multi-user.target.wants
	ln -fs ../vpom.service $(buildroot)/lib/systemd/system/multi-user.target.wants

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS))))
