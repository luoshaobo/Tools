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

set(CMAKE_VERBOSE_MAKEFILE ON)
message("WHEREAMI: ${CMAKE_CURRENT_SOURCE_DIR}")

#load PkgConfig cmake module for pkg-config functionality
find_package(PkgConfig REQUIRED)
if(LOGGING MATCHES "STDOUT_AND_DLT" OR LOGGING MATCHES "DLT_ONLY")
  pkg_check_modules(MODULE_DLT REQUIRED automotive-dlt)
endif()


#DEBUG
message("INCLUDES: ${MODULE_PROTOBUF_INCLUDE_DIRS} ${MODULE_DLT_INCLUDE_DIRS}")
message("LIB_DIRS: ${MODULE_PROTOBUF_LIBRARY_DIRS} ${MODULE_DLT_LIBRARY_DIRS}")
message("LIBS: ${MODULE_PROTOBUF_LIBRARIES} ${MODULE_DLT_LIBRARIES}")
message("CFLAGS: ${MODULE_PROTOBUF_CFLAGS} ${MODULE_DLT_CFLAGS}")
message("VERSION: ${MODULE_PROTOBUF_VERSION} ${MODULE_DLT_VERSION}")

# add cflags from relevant libs, but do NOT add the ones
# from DLT (they cause nasty errors and also are not required according to
# https://github.com/GENIVI/dlt-daemon/blob/master/doc/dlt_cheatsheet.txt)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} ${MODULE_PROTOBUF_CFLAGS}"
)


#file(GLOB SOURCES *.cpp)
set(SOURCES
  atomiccounter.cpp
  binaryringbuffer.cpp
  comalog.cpp
  dynamiclibrary.cpp
  dynamiclibrary_POSIX.cpp
  eventhandler.cpp
  eventstatemachinecontrol.cpp
  eventstatemachineresult.cpp
  ilifecycle.cpp
  ipaddress.cpp
  istatemachine.cpp
  mutex.cpp
  mutex_POSIX.cpp
  networkinterface.cpp
  runnable.cpp
  semaphore.cpp
  semaphore_POSIX.cpp
  socket.cpp
  statemachinemaster.cpp
  stringutils.cpp
  subject.cpp
  thread.cpp
  threadeventqueue.cpp
  thread_POSIX.cpp
  timeelapse.cpp
  timer.cpp
  timestamp.cpp
)

# separate public headers
set(PUBLIC_HEADERS
  ../include/operatingsystem.h
)
# public headers in special dir. hierarchy (e.g. ipc/protobuf/*.h)
set(PUBLIC_HEADER_DIRS
 ../include/
)
#add_definitions(
#    -DCOMALOG_DEBUG_PRINTS -DCOMALOG_DLT_CONTEXT=coma_ctx
#)


######################################
# Library serviceframework
add_library(serviceframework SHARED ${SOURCES})

if(LOGGING MATCHES "STDOUT_AND_DLT" OR LOGGING MATCHES "DLT_ONLY")
  target_link_libraries(serviceframework
    ${MODULE_PROTOBUF_LIBRARIES}
    ${MODULE_DLT_LIBRARIES}
    dl
  )
else()
  target_link_libraries(serviceframework
    ${MODULE_PROTOBUF_LIBRARIES}
    dl
  )
endif()

target_include_directories(serviceframework PUBLIC
  ../include/
  ${MODULE_DLT_INCLUDE_DIRS}
)

#more concise variant of add_definitions( -Dxxxx )
if(LOGGING MATCHES "STDOUT_AND_DLT")
  target_compile_definitions(serviceframework PUBLIC
    COMALOG_DEBUG_PRINTS
    #COMALOG_DLT_CONTEXT=coma_ctx
  )
elseif(LOGGING MATCHES "STDOUT_ONLY")
  target_compile_definitions(serviceframework PUBLIC
    COMALOG_DEBUG_PRINTS
    COMALOG_NO_DLT
  )
else() #default: DLT on
  target_compile_definitions(serviceframework PUBLIC
    #COMALOG_DLT_CONTEXT=coma_ctx
  )
endif()
#
######################################

install (
  TARGETS serviceframework
  LIBRARY DESTINATION ${SERV_EXPORTED_LIB_LOCATION} COMPONENT lib
)
install(
  FILES ${PUBLIC_HEADERS}
  DESTINATION ${SERV_EXPORTED_HEADER_LOCATION} COMPONENT includes
)
install(
  DIRECTORY ${PUBLIC_HEADER_DIRS}
  DESTINATION ${SERV_EXPORTED_HEADER_LOCATION} COMPONENT includes
)
install(
  FILES serviceframework.pc
  DESTINATION ${SERV_EXPORTED_PKG_CONFIG_PATH} COMPONENT pkgconfig
)
