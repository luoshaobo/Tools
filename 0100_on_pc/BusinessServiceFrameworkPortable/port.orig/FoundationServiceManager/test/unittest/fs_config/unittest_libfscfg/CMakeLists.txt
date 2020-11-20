cmake_minimum_required(VERSION 3.5)

#specify generated executable file
set(PROJECT_NAME unittest_libfscfg)
project(${PROJECT_NAME} C CXX)

#specify compiling options
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-ftest-coverage -fprofile-arcs")#this generate .gcda and .gcno files

add_definitions(-D__UT_DLT_DISABLE_PRINTF__)

find_package(PkgConfig)
pkg_check_modules(GLIB_2 REQUIRED glib-2.0)
pkg_check_modules(AUTOMOTIVE_DLT automotive-dlt)

set(fs_config_PATH ../../../../fs_config)
set(fscfg_public_include_PATH ${fs_config_PATH}/interface/public/include)
set(fsm_fscfg_protected_include_PATH ${fs_config_PATH}/interface/protected)
set(fsm_fscfg_private_include_PATH ${fs_config_PATH}/interface/private)
set(fsm_fscfg_protected_exclude_PATH ${fs_config_PATH}/interface/protected/generated)
set(fsm_fscfg_mock_gen_PATH gdbus_proxy_mock/fscfg_common/generated)

include_directories(.)
include_directories(../../googletest/googletest/include)
include_directories(gdbus_proxy_mock)
include_directories(${fscfg_public_include_PATH})
include_directories(${fsm_fscfg_protected_include_PATH})
include_directories(${fsm_fscfg_private_include_PATH})
include_directories(${GLIB_2_INCLUDE_DIRS})

file(GLOB fscfg_INCLUDE_FILES
    ${fscfg_public_include_PATH}/fscfg/*.h
    ${fsm_fscfg_private_include_PATH}/libfscfg/*.h)

file(GLOB fsm_fscfg_common_INCLUDE_FILES
    ${fsm_fscfg_protected_include_PATH}/fscfg_common/*.h)

file(GLOB dbus_mock_PATH_INCLUDE_FILES
     ${fsm_fscfg_mock_gen_PATH}/*.h)

file (GLOB fscfg_lib_SRC_FILES
      ${fs_config_PATH}/src/libfscfg/*.cpp
      ${fs_config_PATH}/src/fscfg_common/*.cpp)

file(GLOB fscfg_mock_GEN_SRC
     ${fsm_fscfg_mock_gen_PATH}/*.cc)

file(GLOB fscfg_mock_SRC
     gdbus_proxy_mock/*.cc
     "*.cc")

set(fscfg_SRC
    ${fscfg_INCLUDE_FILES}
    ${fsm_fscfg_common_INCLUDE_FILES}
    ${dbus_mock_PATH_INCLUDE_FILES}
    ${fscfg_lib_SRC_FILES}
    ${fscfg_mock_SRC}
    ${fscfg_mock_GEN_SRC})

list(REMOVE_ITEM fsm_fscfg_protected_include_PATH ${fsm_fscfg_protected_exclude_PATH})

link_directories(../../build_googletest/googlemock/gtest)
link_directories(../../build_googletest/lib)
link_directories(${GLIB_2_LIBRARY_DIRS})

LINK_LIBRARIES(${GLIB_2_LIBRARIES})

#all dependent file need to add.
add_executable(${PROJECT_NAME} ${fscfg_SRC})

#set all library files which need to link
target_link_libraries(${PROJECT_NAME} 
						gtestd 
						gmockd 
						pthread)