cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fsm_xmlif)
project(${PROJECT_NAME} CXX)
set (CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)
pkg_check_modules(LIBXML2 libxml-2.0)

set (xmlif_PATH ${fsm_PATH}/fsm_ccm)
set (xmlif_src_PATH ${xmlif_PATH}/src)
set (xmlif_include_PATH ${xmlif_PATH}/include)

file(GLOB xmlif_local_SRC
    ${xmlif_src_PATH}/xml.cpp)

file(GLOB xmlif_INCLUDE_FILES
   ${xmlif_include_PATH}/xml.hpp)

include_directories(${xmlif_include_PATH})
include_directories(${fsm_fscfg_stubs_PATH}/dlt)
include_directories(${fsm_PATH}/fsm_persist_data_mgr/include)
include_directories(${LIBXML2_INCLUDE_DIRS})

set(xmlif_SRC ${xmlif_INCLUDE_FILES} ${xmlif_local_SRC})
add_library(libfsm_xmlif SHARED ${xmlif_local_SRC})
set_target_properties(libfsm_xmlif PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
