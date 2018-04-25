BUILD_TARGET ?= 0

ifeq ($(BUILD_TARGET), 1)
SYS_ROOT = /home/uidv4956/work/dev_env/wave2/SyDK/NAC_CN/imx6x-std-adk/armv7a-mv-linux
TOOLCHAIN_PREFIX = arm-montavista-linux-gnueabi-
else
SYS_ROOT = 
TOOLCHAIN_PREFIX = 
endif

CC = $(TOOLCHAIN_PREFIX)gcc
CXX = $(TOOLCHAIN_PREFIX)g++
LD = $(TOOLCHAIN_PREFIX)ld
RM = rm

CFLAGS = \
    -I$(SYS_ROOT)/usr/include/dbus-1.0 \
    -I$(SYS_ROOT)/usr/include/gio-unix-2.0 \
    -I$(SYS_ROOT)/usr/include/glib-2.0 \
    -I./generated \

ifneq ($(BUILD_TARGET), 1)
CFLAGS += -DUSE_SESSION_BUS -I$(SYS_ROOT)/usr/lib/x86_64-linux-gnu/glib-2.0/include
endif
    
CXXFLAGS = $(CFLAGS)

LDFLAGS = \
    -pthread \
    -L$(SYS_ROOT)/usr/lib \
    -lglib-2.0 -lgthread-2.0 -lgobject-2.0 -ldbus-1 -ldbus-glib-1 -l gio-2.0 -lgmodule-2.0 -lz \
    
ifneq ($(BUILD_TARGET), 1)
LDFLAGS += -L/usr/lib/x86_64-linux-gnu
else
LDFLAGS += -lffi
endif

TARGET1 = gserver
TARGET2 = gclient

TARGET3 = qserver
TARGET4 = qclient

.PHONY : all
all : gpro

.PHONY : gpro
gpro : gcode $(TARGET1) $(TARGET2)

$(TARGET1) : gserver.o generated/myapp-generated.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TARGET2) : gclient.o generated/myapp-generated.o
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY : qpro
qpro : qcode $(TARGET3) $(TARGET4)

$(TARGET3) :
	mkdir -p qt/build-$@
	(   \
        cd qt/build-$@ && \
        qmake ../$@/$@.pro && \
        make \
    )
    
$(TARGET4) :
	mkdir -p qt/build-$@
	(   \
        cd qt/build-$@ && \
        qmake ../$@/$@.pro && \
        make \
    )

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
	find qt -name "build-*" -type d | xargs rm -fr
    
.PHONY : code
code : gcode qcode
   
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

.PHONY : qcode
qcode :
	mkdir -p generated
	(   \
        cd generated && \
        qdbusxml2cpp \
            -p NetCorpMyAppFrobberProxy \
            ../net.Corp.MyApp.Frobber.xml && \
        qdbusxml2cpp \
            -a NetCorpMyAppFrobberAdaptor \
            ../net.Corp.MyApp.Frobber.xml \
    )
