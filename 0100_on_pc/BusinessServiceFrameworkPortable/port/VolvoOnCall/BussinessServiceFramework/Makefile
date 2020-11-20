packages += serviceframework

CC := g++
AR := ar
INSTALL := /usr/bin/install

INCLUDES := \
    -I./ \
    -I./include/bsfwk \
    -I../../ServiceFramework/include \
    -I../../ServiceFramework/project/vs2015/ServiceFramework/ServiceFramework/include \
    -I../../FoundationServiceManager/voc_framework/include \
    -I../../UserManager/include \
    -I../../VolvoPositioningManager/include \
    -I../../VolvoPositioningManager/interface \

INCLUDES += -pthread `pkg-config --cflags $(packages)` -I/usr/include/service-framework

LOC_CFLAGS := -g $(CFLAGS) -Wno-error=unused-variable -g -Wall -Werror -Wpointer-arith -Wno-error=switch -fPIC -Wno-error=unused-function $(INCLUDES) -DCOMALOG_NO_DLT -DVCLOG_NO_DLT

LOC_CCFLAGS := -std=c++11

LDFLAGS += -rcvs

DEPENDENCIES :=

C_OBJECTS := \

CC_OBJECTS := \
    src/bsfwk_BusinessServiceStateMachine.o \
    src/bsfwk_BusinessJobStateMachine.o \
    src/bsfwk_Common.o \
    src/bsfwk_BusinessService.o \
    src/bsfwk_Global.o \
    src/bsfwk_JobEntityBase.o \
    src/bsfwk_EntityFactoryBase.o \
    src/bsfwk_ServiceEntityBase.o \
    src/bsfwk_BusinessServer.o \
    src/bsfwk_StatemachineJobEntity.o \
    src/bsfwk_StatemachineServiceEntity.o \

OBJECTS = $(C_OBJECTS) $(CC_OBJECTS)

BINARY := libBussinessServiceFramework.a

all: $(BINARY)

### BINARIES ###
$(BINARY) : $(OBJECTS)
	$(AR) $(LDFLAGS) $@ $^

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

### CLEAN ###
clean :
	rm -f $(BINARY) $(CC_OBJECTS) $(CPP_OBJECTS)
