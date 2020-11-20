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
#* CMake-file for ServiceFrameworkServiceFramework.
#*
#* @project     VCC TCAM
#* @subsystem   ServiceFrameworkServiceFramework
#* @version     0.1
#* @author      Elmar Weber (dev), Bernd Winter (integration)
#* @date        2017-05-18
#***************************************************************************/
cmake_minimum_required(VERSION 2.8.12)
project(ServiceFramework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

set(SERV_EXPORTED_PKG_CONFIG_PATH "/usr/lib/pkgconfig")
set(SERV_EXPORTED_LIB_LOCATION "/usr/lib")
set(SERV_EXPORTED_HEADER_LOCATION "/usr/include/service-framework")

#TODO: +DLT in prod mode!
set(LOGGING "STDOUT_AND_DLT")
#set(LOGGING "DLT_ONLY")
#set(LOGGING "STDOUT_ONLY")
add_definitions(-DLOG_USE_COLOURFUL_CONSOLE)

add_subdirectory(src)
