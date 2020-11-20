#################################################################
# Copyright (C) 2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#################################################################

packages := protobuf libfsm_vocframework libfsm_ccm libfsm_ssm libfsm_transfermanager libusermanager libfsm_persist_data_mgr libcas-power-mgr
packages += vc_proxy blem_proxy ipcb_client
packages += automotive-dlt openssl marben_runtime marben_parsers glib-2.0 gobject-2.0 gio-unix-2.0 gio-2.0 tpsys  local_config libcas-lifecycle-mgr

CC = $(CROSS)gcc
INSTALL = /usr/bin/install
INCLUDES := -I $(CAS_TARGETFS)/usr/include -I ../include -I signals/ccm_parsing/generated
INCLUDES += `pkg-config --cflags $(packages)`

LOC_CFLAGS := $(CFLAGS) -Wno-error=unused-variable
LOC_CFLAGS += -g -Wall -Werror -Wpointer-arith -fPIC $(INCLUDES)

LOC_CCFLAGS := -std=c++11

CC_OBJECTS = \
	features/basic_car_control_feature.o \
	features/car_access_feature.o \
	features/feature_configs.o \
	signals/aes_ble_da_key_voc_frame.o \
	signals/aes_cert_voc_frame.o \
	signals/aes_cert_actor_voc_frame.o \
	signals/aes_cert_nearfield_voc_frame.o \
	signals/aes_ca_voc_frame.o \
	signals/aes_car_actor_cert_voc_frame.o \
	signals/aes_csr_voc_frame.o \
	signals/aes_csr_actor_voc_frame.o \
	signals/aes_csr_nearfield_voc_frame.o \
	signals/basic_car_control.o \
	signals/basic_car_control_signal.o \
	signals/bcc_cl_001_signal.o \
	signals/bcc_cl_002_signal.o \
	signals/bcc_cl_010_signal.o \
	signals/bcc_cl_011_signal.o \
	signals/bcc_cl_020_signal.o \
	signals/ca_cat_001_signal.o \
	signals/ca_cat_002_signal.o \
	signals/ca_del_001_signal.o \
	signals/ca_del_002_signal.o \
	signals/ca_dpa_001_signal.o \
	signals/ca_dpa_002_signal.o \
	signals/ca_dpa_020_signal.o \
	signals/ca_dpa_021_signal.o \
	signals/cloud_resources.o \
    signals/geely/gly_vds_svt_signal.o \
	signals/geely/gly_vds_theft_signal.o \
	signals/geely/gly_vds_res_signal.o \
	signals/geely/gly_vds_rce_signal.o\
	signals/geely/gly_vds_jl_signal.o \
	signals/geely/gly_vds_rvs_signal.o \
	signals/geely/gly_vds_mcu_signal.o \
	signals/geely/gly_vds_mta_signal.o \
	signals/geely/gly_vds_dlagent_signal.o \
	signals/geely/gly_vds_rvdc_signal.o \
	signals/geely/gly_vds_rmc_signal.o \
	signals/device_pairing_id.o \
	signals/ecdhe_approval_voc_frame.o \
	signals/ecdhe_request_voc_frame.o \
	signals/voc_frame_codec.o \
	transactions/admin_confirmation_transaction.o \
	transactions/car_locator_advert_transaction.o \
	transactions/car_locator_map_request_transaction.o \
	transactions/car_locator_position_update_transaction.o \
	transactions/car_position_provider.o \
	transactions/catalogue_upload_transaction.o \
	transactions/device_pairing_transaction.o \
	transactions/key_confirmation_transaction.o \
	transactions/list_paired_devices_transaction.o \
	transactions/privacy_notification_transaction.o \
	transactions/role_selected_notification_transaction.o \
	remote_ctrl/remote_ctrl_common.o \
	remote_ctrl/remote_ctrl_door_tailgate.o \
	remote_ctrl/remote_ctrl_flash_horn.o \
	remote_ctrl/remote_ctrl_window_roof.o \
	remote_ctrl/remote_ctrl_pm25_enquire.o \
	remote_start/remote_start_engine.o \
	remote_start/remote_start_basic.o \
	remote_start/remote_start_seatHeat.o \
	remote_start/remote_start_seatVenti.o \
	remote_start/remote_start_steerwhlHeat.o \
	remote_start/remote_start_climate.o \
	applications_lcm/apps_lcm.o \
	svt/svt.o \
	rvs/rvs.o \
	remote_config/remote_config.o \
	theft/theft_notification_transaction.o \
	rms/rms_transaction.o \
	rms/config/settings.o \
	rms/service/alerts/alertFSM.o \
	rms/service/alerts/alertManager.o \
	rms/service/alerts/storage.o \
	rms/service/data/dataProcessor.o \
	rms/service/dbus/dBusManager.o \
	rms/service/dbus/routeDBusMethods.o \
	rms/service/network/networkClient.o \
	rms/service/network/networkDatatype.o \
	rms/service/network/networkFSM.o \
	rms/service/network/networkManager.o \
	rms/sys/postoffice.o \
	rms/sys/semaphores.o \
	rms/sys/service.o \
	rms/sys/thread.o \
	rms/sys/timerManager.o \
	rms/util/utils.o \
	voc_utils.o \
	voc.o \
	journey_log/car_journeylog_transaction.o \
	mta/mta_transaction.o \
	download_agent/car_downloadagent_transaction.o \
	download_agent/car_downloadagent_fileio.o \
	rvdc/rvdc.o



#pick up all ASN1C c files excluding the sample executable
ASN1C_SOURCES = $(filter-out %converter-sample.c,$(wildcard signals/ccm_parsing/generated/*.c))
C_OBJECTS = $(ASN1C_SOURCES:%.c=%.o)

LDFLAGS += -lstdc++ -L ../../FoundationServiceManager/fs_config/src/libfscfg -lfsm_fscfg -lm `pkg-config --libs $(packages)`
LDFLAGS += -L../voc_persist_data_mgr/src -lvoc_persist_data_mgr
LDFLAGS += -L ../../../../release/fs/devel/usr/lib/boost -lboost_system -lboost_thread -lboost_filesystem -lboost_atomic -lboost_chrono
LDFLAGS += -lpugixml

BINARY := voc
VERSION := 0.0.1

### conditional cflags ###

# should signal injection be enable in the VOC app
# only for test, should never be set in production
# builds.
##ifeq ($(ENABLE_SIGNAL_INJECTION),true)
LOC_CFLAGS += -DENABLE_SIGNAL_INJECTION
INCLUDES += -I ../include/voc_dbus_service -I generated
C_OBJECTS += generated/voc_dbus_generated.o
CC_OBJECTS += voc_dbus_service/voc_dbus.o
CC_OBJECTS += voc_dbus_service/voc_inject_signal.o
DEPENDENCIES := generated/voc_dbus_generated.h
### generate gdbus code ###
$(DEPENDENCIES): voc_dbus_service/com.contiautomotive.tcam.VolvoOnCall.Test.xml
	mkdir -p generated
	gdbus-codegen --interface-prefix com.contiautomotive.tcam. \
		--generate-c-code voc_dbus_generated $<
	mv voc_dbus_generated.c generated
	mv voc_dbus_generated.h generated
##endif

OBJECTS = $(C_OBJECTS) $(CC_OBJECTS)

all: $(BINARY)

### BINARIES ###
$(BINARY): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(STATIC_LINK_LIBRARIES)

### OBJECTS ###
$(C_OBJECTS): $(DEPENDENCIES)
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@

$(CC_OBJECTS):$(CC_OBJECTS:%.o=%.cc) $(DEPENDENCIES)
	$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@

### CLEAN ###
clean:
	$(RM) $(OBJECTS)
	$(RM) $(BINARY)
##ifeq ($(ENABLE_SIGNAL_INJECTION),true)
	$(RM) -r generated
##endif

### INSTALL ###
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr
	$(INSTALL) -m 744 -dv $(buildroot)/usr/bin
	$(INSTALL) -m 755 $(BINARY) $(buildroot)/usr/bin
	$(INSTALL) -m 744 -dv $(buildroot)/etc
	$(INSTALL) -m 744 -dv $(buildroot)/etc/dbus-1
	$(INSTALL) -m 744 -dv $(buildroot)/etc/dbus-1/system.d
	$(INSTALL) -m 644 com.contiautomotive.tcam.VolvoOnCall.conf $(buildroot)/etc/dbus-1/system.d
	$(INSTALL) -m 744 -dv $(buildroot)/lib
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd/system
	$(INSTALL) -m 744 -dv $(buildroot)/lib/systemd/system/multi-user.target.wants
	$(INSTALL) -m 644 voc.service $(buildroot)/lib/systemd/system
	ln -fs ../voc.service $(buildroot)/lib/systemd/system/multi-user.target.wants/


