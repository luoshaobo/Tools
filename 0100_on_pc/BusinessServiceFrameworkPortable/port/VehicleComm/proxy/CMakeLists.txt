#/**
#* Copyright (C) 2016 Continental Automotive GmbH
#*
#* Licensed under the "Supplier unrestricted white box" category, as
#* described in the 20160322-1_SW-amendment TCAM.pdf agreement
#* between Volvo Cars and Continental Automotive GmbH.
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*/
#
#** @file CMakeLists.txt
#* CMake-file for TSComm-lib.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Johan Bohlin & Ola Lilja
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX_PKG REQUIRED gio-unix-2.0)
pkg_check_modules(TPSYS_PKG REQUIRED tpsys)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)

set(PUBLIC_HEADERS
  ${CMAKE_SOURCE_DIR}/interface/vehicle_comm_messages.hpp
  ${CMAKE_SOURCE_DIR}/interface/vehicle_comm_interface.hpp
  ${CMAKE_SOURCE_DIR}/interface/vehicle_comm_types.hpp
  ${CMAKE_SOURCE_DIR}/interface/vc_general_interface.hpp
  ${CMAKE_SOURCE_DIR}/interface/vc_ble_interface.hpp
  ${CMAKE_SOURCE_DIR}/interface/vc_diagnostics_interface.hpp
  ${CMAKE_SOURCE_DIR}/proxy_host/VehicleComm-ifc-generated.h
  ${CMAKE_SOURCE_DIR}/include/vc_utility.hpp
  ${CMAKE_SOURCE_DIR}/include/vc_common.hpp
  ${CMAKE_SOURCE_DIR}/include/vc_message_queue.hpp
  ${CMAKE_SOURCE_DIR}/include/vc_log.hpp
)

#Override DLT_CONTEXT for vc_proxy as it has its own context

include_directories( ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/interface
                     ${CMAKE_SOURCE_DIR}/proxy_host
                     ${GIO_UNIX_PKG_INCLUDE_DIRS}
                     ${TPSYS_PKG_INCLUDE_DIRS}
                     ${GLIB_PKG_INCLUDE_DIRS}
                     ${DLT_PKG_INCLUDE_DIRS})
add_library (vc_proxy SHARED vc_proxy.cpp
                                        ${CMAKE_SOURCE_DIR}/vc_utility.cpp
                                        ${CMAKE_SOURCE_DIR}/vc_log.cpp
                                        ${CMAKE_SOURCE_DIR}/vc_message_queue.cpp
                                        ${CMAKE_SOURCE_DIR}/proxy_host/VehicleComm-ifc-generated.c)
                                
target_link_libraries (vc_proxy ${DLT_PKG_LDFLAGS} ${GLIB_PKG_LDFLAGS} ${GIO_UNIX_PKG_LDFLAGS} ${TPSYS_PKG_LDFLAGS})

install(TARGETS vc_proxy DESTINATION /usr/lib COMPONENT lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION /usr/include/vehiclecomm COMPONENT includes)
install(FILES vc_proxy.pc DESTINATION /usr/lib/pkgconfig COMPONENT pkgconfig)
