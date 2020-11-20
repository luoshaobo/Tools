cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fscfg_client)
project(${PROJECT_NAME} CXX)
set (CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)
pkg_check_modules(GLIB_2 REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX_2 REQUIRED gio-unix-2.0)

set(fsm_fscfg_include_PATH ../../interface/public/include)

file(GLOB fscfg_client_SRC
     "*.cpp")

add_executable(${PROJECT_NAME} ${fscfg_client_SRC})

include_directories(${fsm_fscfg_stubs_PATH})
include_directories(${fsm_fscfg_include_PATH})
include_directories(${GLIB_2_INCLUDE_DIRS} ${GIO_UNIX_2_INCLUDE_DIRS})

link_directories(${GLIB_2_LIBRARY_DIRS} ${GIO_UNIX_2_LIBRARY_DIRS})
link_libraries(${GLIB_2_LIBRARIES} ${GIO_UNIX_2_LIBRARIES})


target_link_libraries(${PROJECT_NAME} fsm_fscfg)
