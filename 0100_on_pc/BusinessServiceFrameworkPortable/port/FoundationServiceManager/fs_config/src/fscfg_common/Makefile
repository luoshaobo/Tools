#################################################################
# Author:       Iulian Sirghi
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               fscfg_common component
#################################################################

packages := automotive-dlt glib-2.0 gio-2.0 gio-unix-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I../../interface/public/include \
            -I../../interface/protected/fscfg_common \
	    -I../../interface/protected \
	    -I../../interface/protected/fscfg_common/generated \
            -I$(CAS_TARGETFS)/usr/include

INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS +=-Wall -Werror -fPIC $(INCLUDES)

LIBRARY_NAME := fscfg_common
LIBRARY := lib${LIBRARY_NAME}.a
VERSION := 0.0.1

SOURCES_C := $(wildcard generated/*.c)
SOURCES_CPP := $(wildcard *.cpp)

OBJECTS_C = $(SOURCES_C:%.c=%.o)
OBJECTS_CPP = $(SOURCES_CPP:%.cpp=%.o)

OBJECTS := $(OBJECTS_C) $(OBJECTS_CPP)
OBJECTS_GENERATED = $(wildcard generated/*.o)

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CROSS)ar rcs $@ $^

%.o: %.c
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@

%.o: %.cpp
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

clean:
	$(RM) $(OBJECTS)
	$(RM) $(OBJECTS_GENERATED)
	$(RM) $(LIBRARY)

install: ;

