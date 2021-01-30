packages += serviceframework

CC := g++
INSTALL := /usr/bin/install

INCLUDES := \
    -I./ \
    -I./main/include \
    -I./test/include \
    -I./misc/include \
    -I./rmc/include \
    -I./theft_notification/include \
    -I./simulator/include \
    -I./port/VolvoOnCall/BussinessServiceFramework/include/bsfwk \
    -I./port/ServiceFramework/include \
    -I./port/ServiceFramework/project/vs2015/ServiceFramework/ServiceFramework/include \
    -I./port/FoundationServiceManager/voc_framework/include \
    -I./port/VolvoOnCall/include \
    -I./port/UserManager/include \
    -I./port/misc/include \
    -I./port/misc/include/dlt \
    -I./port/VehicleComm/interface \
    -I./port/VehicleComm/include \
    -I./port/VolvoPositioningManager/include \
    -I./port/VolvoPositioningManager/interface \
    -I./port/FoundationServiceManager/voc_framework/src/signals/marben_parsing/marben_gen_cplus \

INCLUDES += -pthread `pkg-config --cflags $(packages)` -I/usr/include/service-framework

LOC_CFLAGS := -g $(CFLAGS) -Wno-error=unused-variable -g -Wall -Werror -Wpointer-arith -Wno-error=switch -fPIC -Wno-error=unused-function $(INCLUDES) -DCOMALOG_NO_DLT -DVCLOG_NO_DLT

LOC_CCFLAGS := -std=c++11

LDFLAGS += -pthread -lstdc++ `pkg-config --libs $(packages)`

STATIC_LINK_LIBRARIES := -L./port/VolvoOnCall/BussinessServiceFramework -lBussinessServiceFramework

DEPENDENCIES :=

C_OBJECTS := \

CC_OBJECTS := \
    port/FoundationServiceManager/voc_framework/src/signals/signal.o \
    port/FoundationServiceManager/voc_framework/src/signals/vdmsg_signal.o \
    port/FoundationServiceManager/voc_framework/src/signals/vdmsg_signal_types.o \
    port/FoundationServiceManager/voc_framework/src/transactions/transaction_id.o \
    port/FoundationServiceManager/voc_framework/src/signal_adapt/gly_signal_adapt/rmc_signal_adapt.o \
    port/VolvoOnCall/src/signals/geely/gly_vds_rdl_signal.o \
    port/VolvoOnCall/src/signals/geely/gly_vds_rmc_signal.o \
    port/VolvoOnCall/src/signals/geely/gly_vds_theft_signal.o \

CC_OBJECTS += \
    theft_notification/src/theft_notification.o \
    theft_notification/src/theft_notification_global.o \
    rmc/src/rmc_pm25.o \
    rmc/src/rmc_flash_horn.o \
    rmc/src/rmc_door_tailgate.o \
    rmc/src/rmc_window_roof.o \
    rmc/src/rmc_common.o \
    rmc/src/rmc_global.o \
    simulator/src/vc_simulator.o \
    simulator/src/vpom_simulator.o \
    simulator/src/rmc_simulator.o \
    simulator/src/simulator_common.o \
    misc/src/misc.o \
    test/src/test_app_0001.o \
    test/src/test_app_0002.o \
    test/src/test_app_0003.o \
    test/src/test_app_0004.o \
    test/src/test_app_0005.o \
    test/src/test_app_0006.o \
    test/src/test_app_0007.o \
    test/src/test_app_0008.o \
    test/src/test_app_0010.o \
    test/src/test_app_0011.o \
    test/src/test_app_0101.o \
    test/src/test_app_0102.o \
    test/src/test_app_0103.o \
    test/src/test_app_0104.o \
    test/src/test_app_0201.o \
    test/src/test_app_0202.o \
    test/src/test_app_0203.o \
    test/src/test_app_0204.o \
    test/src/test_app_0301.o \
    test/src/test_app_0401.o \
    test/src/test_app_0501.o \
    test/src/test_app_0502.o \
    test/src/test_app_5001.o \
    test/src/test_app_6001.o \
    test/src/test_app_global.o \
    main/src/main.o \

OBJECTS = $(C_OBJECTS) $(CC_OBJECTS)

BINARY := sample_app

all: ServiceFramework BussinessServiceFramework $(BINARY)

### BINARIES ###
$(BINARY) : $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@ $(STATIC_LINK_LIBRARIES)

### OBJECTS ###
$(C_OBJECTS) : $(DEPENDENCIES)
	$(CC) $(LOC_CFLAGS) $(INCLUDES) -c ${@:.o=.c} -o $@

# $(CC_OBJECTS) : $(CC_OBJECTS:%.o=%.cc) $(DEPENDENCIES)
	# $(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c ${@:.o=.cc} -o $@
    
define CC_COMPILING_template
    $(1): $(1:%.o=%.cc)
		$(CC) $(LOC_CFLAGS) $(LOC_CCFLAGS) $(INCLUDES) -c $${@:.o=.cc} -o $$@
endef

$(foreach cc,$(CC_OBJECTS),$(eval $(call CC_COMPILING_template,$(cc))))
    
ServiceFramework : 
	(cd port/ServiceFramework && cmake CMakeLists.txt && make && sudo make install)
    
BussinessServiceFramework : 
	make -C port/VolvoOnCall/BussinessServiceFramework

### CLEAN ###
clean :
	make -C port/ServiceFramework clean
	make -C port/VolvoOnCall/BussinessServiceFramework clean
	rm -f $(BINARY) $(CC_OBJECTS) $(CPP_OBJECTS) log.txt
