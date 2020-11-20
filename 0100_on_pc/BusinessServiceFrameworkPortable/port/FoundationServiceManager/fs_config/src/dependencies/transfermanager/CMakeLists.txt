cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fsm_transfermanager)
project(${PROJECT_NAME} CXX)
set (CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)
pkg_check_modules(LIBSQLITE3 sqlite3)

set (transfermanager_PATH ${fsm_PATH}/transfermanager)
set (transfermanager_src_PATH ${transfermanager_PATH}/src)
set (transfermanager_include_PATH ${transfermanager_PATH}/include)

file(GLOB transfermanager_local_SRC
    ${transfermanager_src_PATH}/*.h
    ${transfermanager_src_PATH}/*.cpp
    ${transfermanager_src_PATH}/*.cc)

file(GLOB transfermanager_INCLUDE_FILES
   ${fsm_transfermanager_include_PATH}/*.h)

include_directories(${transfermanager_include_PATH})
include_directories(${fsm_fscfg_stubs_PATH}/coma/include)
include_directories(${fsm_PATH}/fsm_persist_data_mgr/include)
include_directories(${LIBSQLITE3_INCLUDE_DIRS})

set(transfermanager_SRC ${transfermanager_INCLUDE_FILES} ${transfermanager_local_SRC})
add_library(libfsm_transfermanager SHARED ${transfermanager_local_SRC})
set_target_properties(libfsm_transfermanager PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
