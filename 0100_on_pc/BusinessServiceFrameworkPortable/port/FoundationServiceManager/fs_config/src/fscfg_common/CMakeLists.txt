cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME fscfg_common)
project(${PROJECT_NAME} C CXX)

find_package(PkgConfig)
pkg_check_modules(GLIB_2 REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX_2 REQUIRED gio-unix-2.0)

set(fsm_fscfg_public_include_PATH ../../interface/public/include)
set(fsm_fscfg_protected_include_PATH ../../interface/protected)

file(GLOB fsm_fscfg_common_INCLUDE_FILES
    ${fsm_fscfg_protected_include_PATH}/fscfg_common/*.h)

file(GLOB fscfg_common_SRC
     "*.c")

 file(GLOB fscfg_common_CXX_SRC
      "*.cpp")

file(GLOB fscfg_common_GEN_SRC
       "./generated/*.c")

INCLUDE_DIRECTORIES(${fsm_fscfg_stubs_PATH})
INCLUDE_DIRECTORIES(${fsm_fscfg_public_include_PATH})
INCLUDE_DIRECTORIES(${fsm_fscfg_protected_include_PATH})
INCLUDE_DIRECTORIES(${fsm_fscfg_protected_include_PATH}/fscfg_common/)
INCLUDE_DIRECTORIES(${fsm_fscfg_protected_include_PATH}/fscfg_common/generated)
INCLUDE_DIRECTORIES(${GLIB_2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GIO_UNIX_2_INCLUDE_DIRS})

LINK_DIRECTORIES(${GLIB_2_LIBRARY_DIRS})
LINK_DIRECTORIES(${GIO_UNIX_2_LIBRARY_DIRS})

add_library(fscfg_common SHARED ${fsm_fscfg_common_INCLUDE_FILES} ${fscfg_common_SRC} ${fscfg_common_CXX_SRC} ${fscfg_common_GEN_SRC})
set_target_properties(fscfg_common PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
