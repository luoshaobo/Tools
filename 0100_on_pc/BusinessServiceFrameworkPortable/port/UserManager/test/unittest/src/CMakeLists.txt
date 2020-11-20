cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME unittest_user_manager)
project(${PROJECT_NAME} CXX)

find_package(PkgConfig)
pkg_check_modules(GLIB glib-2.0)
pkg_check_modules(LIBSSL libssl)
pkg_check_modules(SQLITE3 sqlite3)


add_definitions(-D__UT_DLT_DISABLE_PRINTF__)
add_definitions(-std=c++11)

set(proj_PATH ../../../../../../)
set(fsm_PATH ../../../../FoundationServiceManager/)
set(otp_include_PATH ${proj_PATH}/release/fs/devel/opt/conti/include)
set(usr_mgr_PATH ../../../)
set(fsm_include_PATH ${fsm_PATH}/include)

set(user_manager_include_PATH ${usr_mgr_PATH}/include)
set(user_manager_src_PATH ${usr_mgr_PATH}/src)
set(user_manager_stubs_PATH ./stubs)

file(GLOB user_manager_INCLUDE_FILES
          ${user_manager_include_PATH}/*.h)

file (GLOB user_manager_SRC_FILES
		${user_manager_src_PATH}/*.h
                ${user_manager_src_PATH}/*.cpp
                ${user_manager_src_PATH}/*.cc)

file (GLOB user_manager_persitstence_SRC_FILES
               ${user_manager_stubs_PATH}/persistence/*.h
               ${user_manager_stubs_PATH}/persistence/*.cpp)

file(GLOB unittest_user_manager_SRC_FILES
	 "*.h"
	 "*.cpp")

include_directories(.)
include_directories(../googletest/googletest/include)
include_directories(${user_manager_src_PATH})
include_directories(${user_manager_include_PATH})
include_directories(${otp_include_PATH}/tp)
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${LIBSSL_INCLUDE_DIRS})
include_directories(${SQLITE3_INCLUDE_DIRS})

link_directories(../build_googletest/googlemock/gtest)

add_executable(${PROJECT_NAME}
               ${unittest_user_manager_SRC_FILES}
               ${user_manager_SRC_FILES}
               ${user_manager_INCLUDE_FILES}
               ${user_manager_persitstence_SRC_FILES}
               )

target_link_libraries(${PROJECT_NAME}
                       gtestd
                       pthread
                       ${LIBSSL_LIBRARIES}
                       ${SQLITE3_LIBRARIES}
                       crypto)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ..)
