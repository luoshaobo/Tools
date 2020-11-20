#*************************************************************************
#* Copyright (C) 2016 Continental Automotive Systems, Inc.
#*
#* The copyright notice above does not evidence any actual
#* or intended publication of such source code.
#* The code contains Continental Confidential Proprietary Information.
#*************************************************************************/
#
#** @file CMakeLists.txt
#* CMake-file for the proxy-host.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Johan Bohlin & Ola Lilja
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

set(DBUS_CONFIGURATION_FILES_DIR "/etc/dbus-1/system.d/")
set(VC_BINARY_DIR "/usr/bin/") #Will be prefixed by DESTINATION (DESTDIR/usr) in install processs
set(GDBUS_CODEGEN "$ENV{CAS_HOSTFS}/usr/bin/gdbus-codegen")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSYSTEMD_PKG REQUIRED libsystemd)
pkg_check_modules(DBUS1_PKG REQUIRED dbus-1)
pkg_check_modules(GIO_PKG REQUIRED gio-2.0)
pkg_check_modules(GIO_UNIX_PKG REQUIRED gio-unix-2.0)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
pkg_check_modules(TPSYS_PKG REQUIRED tpsys)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)
pkg_check_modules(VCS_PKG REQUIRED libcas-vcs)

SET(VC_PROXY_HOST_SRC_DIR ${CMAKE_SOURCE_DIR}/proxy_host)

include_directories( $ENV{CAS_TARGETFS}/usr/include
                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/interface
                     ${CMAKE_SOURCE_DIR}/proxy_host
                     ${GIO_PKG_INCLUDE_DIRS}
                     ${GIO_UNIX_PKG_INCLUDE_DIRS}
                     ${DBUS1_PKG_INCLUDE_DIRS}
                     ${GLIB_PKG_INCLUDE_DIRS}
                     ${VCS_PKG_INCLUDE_DIRS}
                     ${TPSYS_PKG_INCLUDE_DIRS}
                     ${DLT_PKG_INCLUDE_DIRS} )

SET(requiredlibs ${requiredlibs} ${GIO_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${GIO_UNIX_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${DBUS1_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${GLIB_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${TPSYS_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${LIBSYSTEMD_PKG_LDFLAGS} )
SET(requiredlibs ${requiredlibs} ${DLT_PKG_LDFLAGS} )

### DEPENDENCIES ###
execute_process(COMMAND ${GDBUS_CODEGEN} --interface-prefix com.contiautomotive.tcam.VehicleComm --generate-c-code VehicleComm-ifc-generated --c-namespace VehicleComm --c-generate-object-manager ${CMAKE_SOURCE_DIR}/proxy_host/VehicleComm.xml)
execute_process(COMMAND mv VehicleComm-ifc-generated.h ${CMAKE_SOURCE_DIR}/proxy_host)
execute_process(COMMAND mv VehicleComm-ifc-generated.c ${VC_PROXY_HOST_SRC_DIR})

install(FILES cfg/com.contiautomotive.tcam.vehiclecomm.conf DESTINATION ${DBUS_CONFIGURATION_FILES_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVC_PRINT_DLT_CONTEXT=vehiclecomm_ctx")

add_library(ts_comm STATIC  ${VC_PROXY_HOST_SRC_DIR}/ts_comm.cpp
                            ${VC_PROXY_HOST_SRC_DIR}/vc_proxy_host.cpp
                            ${VC_PROXY_HOST_SRC_DIR}/VehicleComm-ifc-generated.c)
target_link_libraries(ts_comm ${requiredlibs})

