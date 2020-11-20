cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME fscfgd)
project(${PROJECT_NAME} CXX)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig)
pkg_check_modules(GLIB_2 REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX_2 REQUIRED gio-unix-2.0)
pkg_check_modules(LIBXML2 libxml-2.0)

# LOCAL
set(fsm_fscfg_include_PATH ../../interface/public/include)
set(fsm_fscfgd_priv_include_PATH ../../interface/private)
set(fsm_fscfg_protected_include_PATH ../../interface/protected)
set(fsm_fscfg_test_PATH ../../test/fscfgd)
# FSM PACKAGES
set(fsm_transfer_manager_include_PATH ${fsm_PATH}/transfermanager/include)
set(fsm_persist_include_PATH ${fsm_PATH}/fsm_persist_data_mgr/include)
set(fsm_xmlif_include_PATH ${fsm_PATH}/fsm_ccm/include)
set(coma_stub_include_PATH  ${fsm_fscfg_stubs_PATH}/coma/include)

file(GLOB fscfg_INCLUDE_FILES
    ${fsm_fscfg_include_PATH}/fscfg/*.h)
file(GLOB fsm_fscfg_common_INCLUDE_FILES
    ${fsm_fscfg_protected_include_PATH}/fscfg_common/*.h)
file(GLOB fsm_fscfgd_priv_INCLUDE_FILES
    ${fsm_fscfgd_priv_include_PATH}/fscfgd/*.h)
file(GLOB fsm_fscfg_test_INCLUDE_FILES
    ${fsm_fscfg_test_PATH}/*.h)
file(GLOB fsm_fscfg_test_SRC_FILES
    ${fsm_fscfg_test_PATH}/*.cpp)
file(GLOB fscfg_local_SRC
     "*.cpp")

include_directories(${fsm_fscfg_test_PATH})
include_directories(${fsm_fscfg_stubs_PATH})
include_directories(${fsm_fscfg_include_PATH})
include_directories(${fsm_fscfgd_priv_include_PATH})
include_directories(${fsm_fscfg_protected_include_PATH})

include_directories(${fsm_transfer_manager_include_PATH})
include_directories(${fsm_xmlif_include_PATH})
include_directories(${fsm_local_config_include_PATH})
include_directories(${fsm_persist_include_PATH})
include_directories(${coma_stub_include_PATH})
include_directories(${GLIB_2_INCLUDE_DIRS})
include_directories(${GIO_UNIX_2_INCLUDE_DIRS})
include_directories(${LIBXML2_INCLUDE_DIRS})

link_directories(${GLIB_2_LIBRARY_DIRS})
link_directories(${GIO_UNIX_2_LIBRARY_DIRS})
link_directories(${LIBXML2_LIBRARY_DIRS})

link_libraries(${GLIB_2_LIBRARIES})
link_libraries(${GIO_UNIX_2_LIBRARIES})
link_libraries(${LIBXML2_LIBRARIES})

set(fscfgd_SRC
    ${fscfg_INCLUDE_FILES}
    ${fsm_fscfg_common_INCLUDE_FILES}
    ${fsm_fscfgd_priv_INCLUDE_FILES}
    ${fsm_fscfg_test_INCLUDE_FILES}
    ${fsm_fscfg_test_SRC_FILES}
    ${fscfg_local_SRC})

add_executable(${PROJECT_NAME} ${fscfgd_SRC})

target_link_libraries(${PROJECT_NAME} pthread fscfg_common libfsm_transfermanager libfsm_xmlif fscfg_stubs)
