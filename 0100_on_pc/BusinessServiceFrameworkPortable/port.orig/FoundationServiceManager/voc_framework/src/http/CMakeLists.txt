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
#* CMake-file for CommunicationManager.
#*
#* @project     VCC TCAM
#* @subsystem   CommunicationManager
#* @version     0.1
#* @author      Bernd Winter (integration)
#* @date        2017-01-17
#***************************************************************************/
cmake_minimum_required(VERSION 2.8.12)
project(test_http)

add_definitions(-std=c++11)

set(CMAKE_VERBOSE_MAKEFILE ON)
message("http test")
message("WHEREAMI: ${CMAKE_CURRENT_SOURCE_DIR}")

find_package(PkgConfig REQUIRED)
# NADIF
pkg_check_modules(MODULE_NADIF REQUIRED conti-nadif)
# PSAP-Call
pkg_check_modules(MODULE_PSAP REQUIRED conti-psapcall)
# ServiceFramework
pkg_check_modules(MODULE_SFW REQUIRED serviceframework)
# BLE-Manager
pkg_check_modules(MODULE_BLEM REQUIRED blem_proxy)
# Coma client
pkg_check_modules(MODULE_COMACLIENT REQUIRED comaclient)

message("INCLUDES: ${MODULE_NADIF_INCLUDE_DIRS} ${MODULE_PSAP_INCLUDE_DIRS} ${MODULE_SFW_INCLUDE_DIRS} ${MODULE_BLEM_INCLUDE_DIRS} ${MODULE_COMACLIENT_INCLUDE_DIRS}")
message("LIB_DIRS: ${MODULE_NADIF_LIBRARY_DIRS} ${MODULE_PSAP_LIBRARY_DIRS} ${MODULE_SFW_LIBRARY_DIRS} ${MODULE_BLEM_LIBRARY_DIRS} ${MODULE_COMACLIENT_LIBRARY_DIRS}")
message("LIBS: ${MODULE_NADIF_LIBRARIES} ${MODULE_PSAP_LIBRARIES} ${MODULE_SFW_LIBRARIES} ${MODULE_BLEM_LIBRARIES} ${MODULE_COMACLIENT_LIBRARIES}")
message("CFLAGS: ${MODULE_NADIF_CFLAGS} ${MODULE_PSAP_CFLAGS} ${MODULE_SFW_CFLAGS} ${MODULE_BLEM_CFLAGS_OTHER} ${MODULE_COMACLIENT_CFLAGS}")

add_executable(http-test
  http_get.cc
  http_get_test.cc
)

include_directories(http-test PUBLIC
  .
  ${MODULE_COMACLIENT_INCLUDE_DIRS}
)


link_libraries(http-test
  ${MODULE_COMACLIENT_LIBRARY_DIRS}
)

install (
    TARGETS http-test
    DESTINATION /usr/bin
)

#TODO: find way to use different contexts for core/client/foundation/tester,
#      maybe set ctx via COMALOG-INIT runtime parameter...
#
#target_compile_definitions(coma-core PUBLIC
#  COMALOG_DLT_CONTEXT="coma-client-test"
#)
