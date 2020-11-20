#################################################################
# Copyright (C) 2017-2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#################################################################

cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME unittest_transfermanager)
project(${PROJECT_NAME} CXX)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(LIBBSQLITE sqlite3)
# pkg_check_modules(LIBCURL libcurl)
# pkg_check_modules(PAHO_MQTT paho-mqtt)
# pkg_check_modules(AUTOMOTIVE_DLT automotive-dlt)

add_definitions(-D__UT_DLT_DISABLE_PRINTF__)
add_definitions(-DUNIT_TESTS)
add_definitions(-std=c++11)

set(fsm_PATH ../../../)
set(transfermanager_include_PATH ${fsm_PATH}/transfermanager/include)
set(transfermanager_src_PATH ${fsm_PATH}/transfermanager/src)

set (fsm_persist_data_mgr_include_PATH ${fsm_PATH}/fsm_persist_data_mgr/include)

set(workarea_root_PATH ${fsm_PATH}/../../../)
set(persistency_include_PATH ${workarea_root_PATH}/package/conti/conti-persistence/pcl/include/public/)

set(stubs_PATH ./stub)

file(GLOB transfermanager_INCLUDE_FILES
     ${transfermanager_include_PATH}/*.h)

file (GLOB transfermanager_SRC_FILES
      ${transfermanager_src_PATH}/*.h
      ${transfermanager_src_PATH}/http_cache_db.cc
      ${transfermanager_src_PATH}/cache_utils.cpp)


file (GLOB persistency_INCLUDE_FILES
      ${persistency_include_PATH}/*.h)

file (GLOB stubs_SRC_FILES
      ${stubs_PATH}/*.h
      ${stubs_PATH}/*.cc)

file(GLOB unittest_transfermanager_SRC
#   "*.h"
#   "*.cpp"
     "unittest_http_cache_db.cpp"
     "unittest_cache_utils.cpp")

include_directories(.)
include_directories(${transfermanager_include_PATH})
include_directories(${transfermanager_src_PATH})
include_directories(${fsm_persist_data_mgr_include_PATH})
include_directories(${persistency_include_PATH})
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(../googletest/googletest/include)
include_directories(${stubs_PATH})
#include_directories(${coma_mock_PATH}/include)

link_directories(../build_googletest/googlemock/gtest)

add_executable(${PROJECT_NAME}
               ${transfermanager_SRC_FILES}
               ${unittest_transfermanager_SRC}
               ${transfermanager_INCLUDE_FILES}
               ${stubs_SRC_FILES}
#               ${coma_mock_PATH}/src/libcoma_mock.cpp
                          )

target_link_libraries(${PROJECT_NAME}
                       gtestd
                       ${LIBBSQLITE_LIBRARIES}
                       pthread
#                       ${LIBCURL_LIBRARIES}
#                       ${PAHO_MQTT_LIBRARIES}
#                       ${AUTOMOTIVE_DLT_LIBRARIES}
                       )

set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ..)
