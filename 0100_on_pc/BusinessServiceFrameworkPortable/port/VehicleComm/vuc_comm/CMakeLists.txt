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
#* CMake-file for VuCComm.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Ola Lilja & Johan Bohlin
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
pkg_check_modules(VCS_PKG REQUIRED libcas-vcs)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)
pkg_check_modules(DLT_PKG REQUIRED libcas-pcl)
pkg_check_modules(LCFG_PKG REQUIRED local_config)

include_directories( ${CMAKE_SOURCE_DIR}/include
                     ${CMAKE_SOURCE_DIR}/interface
                     ${VCS_PKG_INCLUDE_DIRS}
                     ${GLIB_PKG_INCLUDE_DIRS}
                     ${DLT_PKG_INCLUDE_DIRS}
                     ${LCFG_PKG_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVUCTRACE_DLT_CONTEXT=vuctrace_ctx")

add_library (vuc_comm STATIC vuc_comm.cpp vuc_trace.cpp)
target_link_libraries(vuc_comm -lpthread ${VCS_PKG_LDFLAGS} ${DLT_PKG_LDFLAGS} ${LCFG_PKG_LDFLAGS})
