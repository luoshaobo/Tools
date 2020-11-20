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
#* CMake-file for VGMComm.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Ola Lilja & Johan Bohlin
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

# Boost
set(BOOST_LIBRARYDIR $ENV{SYSROOT}/usr/lib/boost/)
find_package( Boost 1.54 COMPONENTS system thread log REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

find_package(PkgConfig REQUIRED)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)
pkg_check_modules(LCFG_PKG REQUIRED local_config)

include_directories( ../../../opensource/vSomeIP/interface 
                     ${CMAKE_SOURCE_DIR}/interface
                     ${CMAKE_SOURCE_DIR}/include
                     ${DLT_PKG_INCLUDE_DIRS}
                     ${VSOMEIP_INCLUDE_DIRS}
                     ${LCFG_PKG_INCLUDE_DIRS})

add_library (lan_comm STATIC lan_comm.cpp lan_base.cpp vgm_comm.cpp ihu_comm.cpp)
target_link_libraries(lan_comm -lpthread -lvsomeip ${Boost_LIBRARIES} ${DLT_PKG_LDFLAGS} ${LCFG_PKG_LDFLAGS})

file(COPY lan_comm.json DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# If stdout is enabled enable it for vsomeip prints too.
if (VC_STDOUT)
    execute_process(COMMAND sed -i "s+\"console\" : \"false\"+\"console\" : \"true\"+g" ${CMAKE_CURRENT_BINARY_DIR}/lan_comm.json)
endif()

install (
    FILES ${CMAKE_CURRENT_BINARY_DIR}/lan_comm.json
    DESTINATION /etc
)
