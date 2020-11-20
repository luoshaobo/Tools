cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fsm_fscfg)
project(${PROJECT_NAME} CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)
pkg_check_modules(GLIB_2 REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX_2 REQUIRED gio-unix-2.0)

set(fsm_fscfg_include_PATH ../../interface/public/include)
set(fsm_fscfg_protected_include_PATH ../../interface/protected)
set(fsm_fscfg_private_include_PATH ../../interface/private)

file(GLOB fscfg_INCLUDE_FILES
    ${fsm_fscfg_include_PATH}/fscfg/*.h
    ${fsm_fscfg_private_include_PATH}/libfscfg/*.h)
file(GLOB fsm_fscfg_common_INCLUDE_FILES
    ${fsm_fscfg_protected_include_PATH}/fscfg_common/*.h)
file(GLOB fscfg_local_SRC *.cpp)

set(fscfg_SRC
    ${fscfg_INCLUDE_FILES}
    ${fsm_fscfg_common_INCLUDE_FILES}
    ${fscfg_local_SRC})

include_directories(${fsm_fscfg_stubs_PATH})
include_directories(${fsm_fscfg_include_PATH})
include_directories(${fsm_fscfg_protected_include_PATH})
include_directories(${GLIB_2_INCLUDE_DIRS} ${GIO_UNIX_2_INCLUDE_DIRS} ${fsm_fscfg_private_include_PATH})

LINK_DIRECTORIES(${GLIB_2_LIBRARY_DIRS} ${GIO_UNIX_2_LIBRARY_DIRS})
LINK_LIBRARIES(${GLIB_2_LIBRARIES} ${GIO_UNIX_2_LIBRARIES})

add_library(fsm_fscfg SHARED ${fscfg_SRC})
set_target_properties(fsm_fscfg PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} fscfg_common)
