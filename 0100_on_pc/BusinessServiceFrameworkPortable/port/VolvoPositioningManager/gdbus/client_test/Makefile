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

packages := automotive-dlt tpsys glib-2.0 gio-2.0#we use this to do pkg-config for includes and ld_flags

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I ../include `pkg-config --cflags $(packages)`
INCLUDES += -I ../ -I $(CAS_TARGETFS)/usr/include
INCLUDES += -I ../../interface/
LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith $(INCLUDES)

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

OBJECTS = \
	vpom_gdbus_client.o \

LDFLAGS += -L./lib -lstdc++ `pkg-config --libs $(packages)` -lsystemd
LDFLAGS += -L../client_proxy/ -lvpom_gdbus_client_proxy

BINARY := vpom_gdbus_client

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
	$(INSTALL) -m 744 -dv $(buildroot)/tst/bin
	$(INSTALL) -m 755 $(BINARY) $(buildroot)/tst/bin

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS))))
