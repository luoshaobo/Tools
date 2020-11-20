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
#* CMake-file for VehicleComm.
#*
#* @project     VCC
#* @subsystem   VehicleComm
#* @version     00.01
#* @author      Ola Lilja & Johan Bohlin
#* @date        07-Dec-2016
#***************************************************************************/

cmake_minimum_required (VERSION 2.8.7)
project (VehicleComm)

option(VC_STDOUT "Print logs on stdout" OFF)
option(VC_TEST "Add test-functionality in VehicleComm" OFF)
option(VC_PRINTMEM "Print /proc/<pid>/statm for VehicleComm process after each handled message" OFF)

set(SYSTEMD_CONFIGURATIONS_FILES_DIR "/lib/systemd/system/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCLOG_DEBUG_PRINTS -std=c++11 -Wall -Wextra -Werror")

if (VC_STDOUT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCLOG_STDOUT")
endif()

if (VC_PRINTMEM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVC_PRINTMEM")
endif()

add_subdirectory(vuc_comm)
add_subdirectory(lan_comm)
add_subdirectory(proxy)
add_subdirectory(proxy_host)
add_subdirectory(vc_test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0)
pkg_check_modules(VCS_PKG REQUIRED libcas-vcs)
pkg_check_modules(TPSYS_PKG REQUIRED tpsys)
pkg_check_modules(DBUS1_PKG REQUIRED dbus-1)
pkg_check_modules(DLT_PKG REQUIRED automotive-dlt)
pkg_check_modules(LCM_PKG REQUIRED libcas-lifecycle-mgr)
pkg_check_modules(LCFG_PKG REQUIRED local_config)
pkg_check_modules(PCL_PKG REQUIRED libcas-pcl)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCLOG_DLT_CONTEXT=vehiclecomm_ctx")

include_directories(
					$ENV{CAS_TARGETFS}/usr/include
                    ${CMAKE_SOURCE_DIR}/interface
                    ${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_SOURCE_DIR}/vuc_comm
                    ${CMAKE_SOURCE_DIR}/lan_comm
                    ${CMAKE_SOURCE_DIR}/proxy_host
                    ${VCS_PKG_INCLUDE_DIRS}
                    ${GLIB_PKG_INCLUDE_DIRS}
                    ${VSOMEIP_INCLUDE_DIRS}
                    ${DBUS1_PKG_INCLUDE_DIRS}
                    ${DLT_PKG_INCLUDE_DIRS}
                    ${LCFG_PKG_INCLUDE_DIRS}
                    ${PCL_PKG_INCLUDE_DIRS})

set(SOURCES vehicle_comm.cpp
            vc_log.cpp
            vc_utility.cpp
            vc_message_queue.cpp
            vc_session_list.cpp
            vc_watchdog.cpp
            vc_lcm.cpp
            vc_data_storage.cpp
            vc_parameter_collection.cpp
            vc_message_processor.cpp)

if (VC_TEST)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVC_TEST")
    set(SOURCES ${SOURCES} vehicle_comm_test.cpp)
endif()
add_executable(vehicle_comm ${SOURCES})

target_link_libraries(vehicle_comm lan_comm vuc_comm ts_comm ${TPSYS_PKG_LDFLAGS} ${DLT_PKG_LDFLAGS} ${LCM_PKG_LDFLAGS} ${LCFG_PKG_LDFLAGS} ${PCL_PKG_LDFLAGS})

install (
    TARGETS vehicle_comm
    DESTINATION /usr/bin COMPONENT bin
)

execute_process(COMMAND ln -fs ../vc.service ${CMAKE_CURRENT_BINARY_DIR}/vc.service.symlink)

install(FILES vc.service DESTINATION ${SYSTEMD_CONFIGURATIONS_FILES_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/vc.service.symlink DESTINATION ${SYSTEMD_CONFIGURATIONS_FILES_DIR}/multi-user.target.wants/ RENAME vc.service)
