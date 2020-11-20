cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fs_config)
project(${PROJECT_NAME} CXX)
set (CMAKE_CXX_STANDARD 11)

add_definitions(-DFSM_FSCFG_TEST)

get_filename_component(fsm_fscfg_stubs_PATH src/stubs ABSOLUTE)
get_filename_component(fsm_PATH ../ ABSOLUTE)

add_subdirectory(src/fscfg_common)
add_subdirectory(src/libfscfg)
add_subdirectory(src/fscfgd)
add_subdirectory(src/fscfg_client)
add_subdirectory(src/dependencies)
