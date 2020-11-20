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

packages := automotive-dlt tpsys glib-2.0 gio-2.0 libcas-pcl#we use this to do pkg-config for includes and ld_flags

CC = $(CROSS)gcc
AR=$(CROSS)ar
INSTALL = /usr/bin/install
INCLUDES += -I $(CAS_TARGETFS)/usr/include `pkg-config --cflags $(packages)`
INCLUDES += -I ../../interface/
LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -O2 -Wall -Werror -Wpointer-arith $(INCLUDES)

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

OBJECTS = vpom_gdbus_client_proxy.o ../vpom-ifc-generated.o vpom_Client.o

LDFLAGS += -L./lib -lstdc++ `pkg-config --libs $(packages)` -lsystemd

LIBRARY := libvpom_gdbus_client_proxy.a
VERSION := 0.0.1

all: $(LIBRARY)

### BINARIES ###
$(LIBRARY): $(OBJECTS)
	$(AR) rsu $(LIBRARY) $(OBJECTS)

### OBJECTS ###
%.o:%.cpp
%.o:%.cpp $(DEPDIR)/%.d
	$(CC) $(DEPFLAGS) $(LOC_CFLAGS) $(INCLUDES) $(LDFLAGS) -c ${@:.o=.cpp} -o $@
	@$(POSTCOMPILE)

### CLEAN ###
clean:
	rm -f *.o
	rm -f $(LIBRARY)
	rm -rf $(DEPDIR)

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/lib
	$(INSTALL) -m 755 $(LIBRARY) $(buildroot)/usr/lib

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS))))
