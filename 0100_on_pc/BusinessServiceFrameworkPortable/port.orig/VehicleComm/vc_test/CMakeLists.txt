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
#* CMake-file for VCTest.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Johan Bohlin & Ola Lilja
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

set(VC_TST_DIR "/tst/bin")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
pkg_check_modules(TPSYS_PKG REQUIRED tpsys)
pkg_check_modules(LCFG_PKG REQUIRED local_config)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)
pkg_check_modules(PCL_PKG REQUIRED libcas-pcl)

include_directories( ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/interface
                     ${CMAKE_SOURCE_DIR}/proxy_host
                     ${GLIB_PKG_INCLUDE_DIRS}
                     ${TPSYS_PKG_INCLUDE_DIRS}
                     ${LCFG_PKG_INCLUDE_DIRS}
                     ${DLT_PKG_INCLUDE_DIRS}
                     ${PCL_PKG_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVC_PRINT_DLT_CONTEXT=vehiclecomm_vc_test_ctx")

#Enable debug prints and stdout prints for test client regardless of what's in the default CMakefile
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCLOG_DEBUG_PRINTS -DVCLOG_STDOUT")

add_executable (vc_test vc_test.cpp ${CMAKE_SOURCE_DIR}/vc_log.cpp ${CMAKE_SOURCE_DIR}/vc_message_queue.cpp)
target_link_libraries (vc_test ${TPSYS_PKG_LDFLAGS} -lpthread vc_proxy)

install(TARGETS vc_test DESTINATION ${VC_TST_DIR} )
