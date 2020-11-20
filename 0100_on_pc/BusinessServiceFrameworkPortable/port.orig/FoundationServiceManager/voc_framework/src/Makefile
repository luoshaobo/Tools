#################################################################
# Copyright (C) 2017 Continental Automotive AG and Subsidiaries
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
# Author:       Piotr Tomaszewski
# Date:         05-June-2017
# Description:  This makefile is used to clean and make
#               FoundationServiceManager's volvo on call framework
#################################################################

packages := libusermanager comaclient conti-nadif openssl automotive-dlt
packages += vc_proxy ipcb_client vpom_client glib-2.0 gobject-2.0 gio-unix-2.0 gio-2.0 tpsys
packages += marben_runtime marben_parsers

CC = $(CROSS)gcc
INSTALL = /usr/bin/install

INCLUDES += -I ../include -I signals/ccm_parsing/generated -I ../../fsm_ccm/include
INCLUDES += -I signals/marben_parsing/marben_gen_cplus
INCLUDES += -I signal_adapt/vds_marben_adapt
INCLUDES += -I ../include/voc_framework/signal_adapt

INCLUDES += -I baseLogSystem/logInterface
INCLUDES += -I ../../transfermanager/include -I ../../transfermanager/include -I ../../signal_service_manager/include
INCLUDES += -I ../../fsm_persist_data_mgr/include  -I ../../service_discovery/include  #TODO: why service discoveryh still here?
INCLUDES += -I ../../fs_config/interface/public/include
INCLUDES += `pkg-config --cflags $(packages)`
INCLUDES += -I $(CAS_TARGETFS)/usr/include
INCLUDES += -I ../../../../opensource
INCLUDES += -I ../../../CommunicationManager/include/communicationmanager

LOC_CFLAGS := $(CFLAGS) -Wno-error=unused-variable
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith -fPIC

LOC_CCFLAGS := -std=c++11

LDFLAGS += -lstdc++

OBJECTS :=  features/feature.o \
            features/feature_config.o \
            signals/assistance_call.o \
            signals/car_access_signal.o \
            signals/ccm.o \
            signals/ccm_codec.o \
            signals/ccm_signal.o \
            signals/cloud_resource.o \
            signals/entry_point_signal.o \
            signals/features_signal.o \
            signals/location_data.o \
            signals/marben_codec.o \
            signals/response_info_data.o \
            signals/signal.o \
            signals/signal_factory.o \
            signals/timeout_signal.o \
            signals/xml_codec.o \
            signals/xml_payload.o \
            signals/geely_asn_common.o \
            signals/vdmsg_signal_types.o \
            transactions/transaction_id.o \
            transactions/transaction.o \
            transactions/smart_transaction.o \
            signal_sources/ble_signal_source.o \
            signal_sources/car_access_signal_source.o \
            signal_sources/signal_source.o \
            signal_sources/ssm_signal_source.o \
            signal_sources/ip_command_broker_signal_source.o \
            signal_sources/internal_signal_source.o \
            signal_sources/timeout_receiver.o \
            signal_sources/timeout_signal_source.o \
            signal_sources/vehicle_comm_signal_source.o \
            signal_sources/vocmo_signal_source.o \
            signal_sources/vpom_signal_source.o \
            signal_sources/sms_certificate.o \
            http/http_get.o \
            signal_sources/s_ack.o \

OBJECTS += signals/vdmsg_signal.o

OBJECTS += signal_adapt/vds_marben_adapt/vds_marben_adapt_base.o
OBJECTS += signal_adapt/vds_marben_adapt/vds_marben_adapt_cus.o
OBJECTS += signal_adapt/vds_marben_adapt/vds_marben_adapt_gen.o
OBJECTS += signal_adapt/gly_signal_adapt/signal_adapt_base.o
OBJECTS += signal_adapt/gly_signal_adapt/svt_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/mcu_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/dlagent_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/rvdc_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/res_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/rce_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/rcc_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/mta_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/call_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/theft_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/journeylog_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/rmc_signal_adapt.o
OBJECTS += signal_adapt/gly_signal_adapt/rvs_signal_adapt.o


$(warning OBJECTS = $(OBJECTS))

ASN1C_SOURCES = $(filter-out %converter-sample.c,$(wildcard signals/ccm_parsing/generated/*.c))
ASN1C_OBJECTS = $(ASN1C_SOURCES:%.c=%.o)

MARBEN_PER_SOURCES = $(filter-out %converter-sample.c,$(wildcard signals/marben_parsing/marben_gen_cplus/*.cc))
MARBEN_PER_OBJECTS = $(MARBEN_PER_SOURCES:%.cc=%.o)

#LOG_SOURCES = $(wildcard baseLogSystem/logInterface/*.c)
#LOG_OBJECTS = $(LOG_SOURCES:%.c=%.o)

LIBRARY := libfsm_vocframework.so
VERSION := 0.0.1

# should signal injection be enable in the VOC app
# only for test, should never be set in production
# builds.
##ifeq ($(ENABLE_SIGNAL_INJECTION),true)
        LOC_CFLAGS += -DENABLE_SIGNAL_INJECTION
##endif


all: $(LIBRARY)

### LIBRARIES ###
$(LIBRARY): $(OBJECTS) $(ASN1C_OBJECTS) $(MARBEN_PER_OBJECTS)
	$(CC) $(LDFLAGS) -shared -Wl,-soname,$@ -o $@ $^

### OBJECTS ###
%.o:%.cpp
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cpp} -o $@

%.o:%.cc
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@

%.o: %.c
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c $< -o $@

### CLEAN ###
clean:
	$(RM) $(OBJECTS)
	$(RM) $(ASN1C_OBJECTS)
	$(RM) $(LIBRARY)

### INSTALL ###
install:
	# library
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib
	$(INSTALL) --mode=755 $(LIBRARY) $(buildroot)/usr/lib
	# package config
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 644 libfsm_vocframework.pc $(buildroot)/usr/lib/pkgconfig
	# includes
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/features
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/signals
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/signal_adapt
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/signal_sources
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/signal_sources/generated
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/signal_sources/generated/vds_marben
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/transactions
	$(INSTALL) --mode=755 -dv $(buildroot)/usr/include/fsm/voc_framework/src/http
	$(INSTALL) -m 644 ../include/voc_framework/signals/*.h  $(buildroot)/usr/include/fsm/voc_framework/signals/
	$(INSTALL) -m 644 ../include/voc_framework/transactions/*.h  $(buildroot)/usr/include/fsm/voc_framework/transactions/
	$(INSTALL) -m 644 ../include/voc_framework/features/*.h  $(buildroot)/usr/include/fsm/voc_framework/features/
	$(INSTALL) -m 644 ../include/voc_framework/signal_sources/*.h $(buildroot)/usr/include/fsm/voc_framework/signal_sources/
	$(INSTALL) -m 644 signals/ccm_parsing/generated/*.h  $(buildroot)/usr/include/fsm/voc_framework/signal_sources/generated/
	$(INSTALL) -m 644 signals/marben_parsing/marben_gen_cplus/*.h  $(buildroot)/usr/include/fsm/voc_framework/signal_sources/generated/
	$(INSTALL) -m 644 baseLogSystem/logInterface/*.h  $(buildroot)/usr/include/fsm/voc_framework/signal_sources/generated/
	$(INSTALL) -m 644 ../include/voc_framework/signal_adapt/*.h  $(buildroot)/usr/include/fsm/voc_framework/signal_adapt/
	$(INSTALL) -m 644 signal_adapt/vds_marben_adapt/*.h  $(buildroot)/usr/include/fsm/voc_framework/signal_adapt/
	$(INSTALL) -m 644 ../src/http/*.h  $(buildroot)/usr/include/fsm/voc_framework/src/http

	#mkdir -p $(buildroot)/usr/etc/
	#$(INSTALL) -m 755 ./cert/server.cert.pem $(buildroot)/usr/etc/
	$(INSTALL) -m 644  ./cert/sms_server.cert.pem  $(buildroot)/opt/conti/share/persistence/

