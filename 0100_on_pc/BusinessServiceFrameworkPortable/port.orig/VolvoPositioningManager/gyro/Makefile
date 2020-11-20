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

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES += -I $(CAS_TARGETFS)/usr/include
INCLUDES += -I ../libdrsensor
LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -O2 -Wall -Werror -Wpointer-arith $(INCLUDES)

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

OBJECTS := L3g4200d.o
CFLAGS += -g -Wall -Werror -Wpointer-arith $(INCLUDES)
LDFLAGS += -L./lib -lpthread -lm -lrt -rdynamic

all: $(OBJECTS)

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

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(OBJECTS))))
