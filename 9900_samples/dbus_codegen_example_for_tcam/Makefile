BUILD_TARGET ?= 0

ifeq ($(BUILD_TARGET), 1)
    SYS_BUILD_ROOT ?= /home/uidv4956/tcam_root/tcam_src_root_03.19.16.01
    SYS_ROOT = $(SYS_BUILD_ROOT)/release/fs/devel
    PATH := $(SYS_BUILD_ROOT)/release-toolchain/fs/devel/usr/bin:$(SYS_BUILD_ROOT)/release-host/fs/host/usr/bin:$(PATH)
    export PATH
    PKG_CONFIG_PATH := $(SYS_BUILD_ROOT)/release/fs/devel/usr/lib/pkgconfig:$(PKG_CONFIG_PATH)
    export PKG_CONFIG_PATH
    TOOLCHAIN_PREFIX = arm-cas-linux-gnueabi-
else
    SYS_ROOT = 
    TOOLCHAIN_PREFIX = 
endif

CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_PREFIX)ld
RM = rm

packages = gmodule-2.0 gio-2.0 gio-unix-2.0 glib-2.0 dbus-1 dbus-glib-1

CFLAGS = \
    `pkg-config --cflags $(packages)` \
    -I$(SYS_ROOT)/usr/lib/glib-2.0/include/ \
    -I./generated \

ifneq ($(BUILD_TARGET), 1)
    CFLAGS += -DUSE_SESSION_BUS
endif
    
CXXFLAGS = $(CFLAGS)

LDFLAGS = \
    -pthread \
    -L$(SYS_ROOT)/usr/lib \
    `pkg-config --libs $(packages)` \
    -lffi -lz

TARGET1 = gserver
TARGET2 = gclient

.PHONY : all
all : gpro

.PHONY : gpro
gpro : gcode $(TARGET1) $(TARGET2)

$(TARGET1) : gserver.o generated/myapp-generated.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET2) : gclient.o generated/myapp-generated.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^
    
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^
    
.PHONY : clean
clean :
	$(RM) -f $(TARGET1) $(TARGET2) *.o generated/*.o
    
.PHONY : distclean
distclean : clean
	$(RM) -fr generated
	find . -name "*.pro.user" | xargs rm -f
    
.PHONY : code
code : gcode
   
.PHONY : gcode   
gcode :
	mkdir -p generated
	(   \
        cd generated && \
        gdbus-codegen \
            --interface-prefix net.corp.MyApp. \
            --generate-c-code myapp-generated \
            --c-namespace MyApp \
            ../net.Corp.MyApp.Frobber.xml \
    )
