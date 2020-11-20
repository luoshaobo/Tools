#################################################################
# Author:       Iulian Sirghi
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               fscfgd component
#################################################################

packages := automotive-dlt glib-2.0 gio-2.0 gio-unix-2.0

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I../../interface/public/include \
            -I../../interface/protected/fscfg_common \
	    -I../../interface/protected \
	    -I../../interface/private

INCLUDES += `pkg-config --cflags $(packages)`

LIBRARY_PATHS += -L../libfscfg \

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS +=-Wall -Werror -std=c++11 $(INCLUDES)
LDFLAGS += -lfsm_fscfg -lstdc++ `pkg-config --libs $(packages)`

EXECUTABLE := test_client
VERSION := 0.0.1

SOURCES := $(wildcard *.cpp)

OBJECTS = $(SOURCES:%.cpp=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CROSS)gcc -Wall -o $@ $(LIBRARY_PATHS) $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.cpp} -o $@

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXECUTABLE)
