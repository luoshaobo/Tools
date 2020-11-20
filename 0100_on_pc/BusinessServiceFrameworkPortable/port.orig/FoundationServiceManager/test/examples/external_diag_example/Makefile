########################################################################################################################
# Author:       Iulian Sirghi
#
# Date:         26-Jan-2018
#
# Description:  This makefile is used to clean and make our external diagnostics example application.
#               The example application uses the voc_framework library to retrieve the ExternalDiagnostics feature,
#               check for it's state (enabled state) and also retrieve the ClientCapabilities and RemoteSession
#               resources
#
########################################################################################################################

packages := libfsm_vocframework automotive-dlt tpsys

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS += -Wall -Werror -std=c++11 $(INCLUDES)
LDFLAGS += -lstdc++ `pkg-config --libs $(packages)`

EXECUTABLE := external_diag_example
VERSION := 0.0.1

SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:%.cpp=%.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(LOC_CFLAGS) -c ${@:.o=.cpp} -o $@

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXECUTABLE)

