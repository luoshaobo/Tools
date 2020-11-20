cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fsm_local_config)
project(${PROJECT_NAME} CXX)
set (CMAKE_CXX_STANDARD 11)

set (service_discovery_PATH ${fsm_PATH}/service_discovery)
set (local_config_src_PATH ${service_discovery_PATH}/daemon)
set (local_config_include_PATH ${local_config_src_PATH})

file(GLOB local_config_local_SRC
    ${local_config_src_PATH}/FsLocalConfig.cpp)

file(GLOB local_config_INCLUDE_FILES
   ${xmlif_include_PATH}/FsLocalConfig.hpp)

include_directories(${local_config_include_PATH})
include_directories(${fsm_fscfg_stubs_PATH})

set(local_config_SRC ${local_config_INCLUDE_FILES} ${local_config_local_SRC})
add_library(libfsm_local_config SHARED ${local_config_SRC})
set_target_properties(libfsm_local_config PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
