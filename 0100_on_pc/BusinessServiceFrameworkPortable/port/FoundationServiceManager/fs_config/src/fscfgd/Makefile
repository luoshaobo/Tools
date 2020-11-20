#################################################################
# Author:       Iulian Sirghi
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               fscfgd component
#################################################################

packages := gobject-2.0 glib-2.0 gio-2.0 gio-unix-2.0 libxml-2.0 openssl comaclient automotive-dlt libcas-pcl
packages += libfsm_usermanager conti-nadif vc_proxy ipcb_client vpom_client tpsys marben_runtime marben_parsers local_config

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

FSM_PATH = ../../../
INCLUDES += -I $(CAS_TARGETFS)/usr/include -I../../interface/public/include \
            -I../../interface/protected/fscfg_common \
            -I../../interface/protected \
            -I../../interface/private \
            -I${FSM_PATH}/fsm_ccm/include \
            -I${FSM_PATH}/fsm_persist_data_mgr/include \
            -I$(FSM_PATH)/transfermanager/include \
            -I${FSM_PATH}/voc_framework/include \
            -I${FSM_PATH}/signal_service_manager/include \
            -I${FSM_PATH}/service_discovery/include \
            -I${FSM_PATH}/voc_framework/src/signals/ccm_parsing/generated\
            -I${FSM_PATH}/voc_framework/src/signals/marben_parsing/marben_gen_cplus


INCLUDES += `pkg-config --cflags $(packages)`

LIBRARY_PATHS += -L../fscfg_common \
                 -L../libfscfg \
                 -L$(FSM_PATH)/fsm_ccm/src \
                 -L$(FSM_PATH)/voc_framework/src \
                 -L$(FSM_PATH)/signal_service_manager/src \
                 -L$(FSM_PATH)/fsm_persist_data_mgr/src \
                 -L${FSM_PATH}/transfermanager/src \
                 -L${FSM_PATH}/service_discovery/lib

LOC_CFLAGS := $(CFLAGS)
LOC_CFLAGS +=-Wall -Werror -std=c++11 $(INCLUDES)
LDFLAGS += -lfscfg_common -lfsm_fscfg -lfsm_ccm -lfsm_persist_data_mgr -lfsm_transfermanager -lfsm_usermanager -lfsm_ssm -lfsm_vocframework -lstdc++ `pkg-config --libs $(packages)`

EXECUTABLE := fscfgd
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

install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(EXECUTABLE) $(buildroot)/usr/bin
	$(INSTALL) -m 744 -dv $(buildroot)/etc
	$(INSTALL) -m 744 -dv $(buildroot)/etc/dbus-1
	$(INSTALL) -m 744 -dv $(buildroot)/etc/dbus-1/system.d
	$(INSTALL) -m 644 com.contiautomotive.tcam.FoundationServices.Config.conf $(buildroot)/etc/dbus-1/system.d
	$(INSTALL) -m 744 -dv $(buildroot)/lib
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd/system
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd/system/multi-user.target.wants
	$(INSTALL) -m 644 fscfgd.service $(buildroot)/lib/systemd/system
	ln -fs ../fscfgd.service $(buildroot)/lib/systemd/system/multi-user.target.wants/
