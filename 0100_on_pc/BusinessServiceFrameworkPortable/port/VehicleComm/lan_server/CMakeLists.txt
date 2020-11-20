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
#* CMake-file for VGMServer.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Johan Bohlin & Ola Lilja
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (lan_server)

# This will get us acces to
#   VSOMEIP_INCLUDE_DIRS - include directories for vSomeIP
#   VSOMEIP_LIBRARIES    - libraries to link against
find_package(vsomeip)
if (NOT vsomeip_FOUND)
    message("vsomeip was not found. Please specify vsomeip_DIR")
endif()

# Boost
find_package( Boost 1.54 COMPONENTS system thread log REQUIRED )

#Changed to match old gcc compiler in ubuntu 12.04
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCLOG_NO_DLT -DVCLOG_STDOUT")

include_directories( ../interface
                     ../include
                     ../lan_comm
                     ${VSOMEIP_INCLUDE_DIRS}
                     ${Boost_INCLUDE_DIR})

add_executable (vgm_server vgm_server.cpp someip_server.cpp ../lan_comm/lan_base.cpp ../vc_log.cpp ../vc_utility.cpp ../vc_message_queue.cpp)
target_link_libraries(vgm_server -lpthread ${VSOMEIP_LIBRARIES} ${Boost_LIBRARIES})

add_executable (ihu_server ihu_server.cpp someip_server.cpp ../lan_comm/lan_base.cpp ../vc_log.cpp ../vc_utility.cpp ../vc_message_queue.cpp)
target_link_libraries(ihu_server -lpthread ${VSOMEIP_LIBRARIES} ${Boost_LIBRARIES})

