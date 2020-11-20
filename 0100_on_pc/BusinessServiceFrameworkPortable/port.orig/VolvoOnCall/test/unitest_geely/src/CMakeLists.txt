cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(PROJECT_NAME unitest_geely)
project(${PROJECT_NAME})

add_definitions(-D__UT_DLT_DISABLE_PRINTF__ -DVOC_TESTS)
add_definitions(-DVOC_TESTS)
add_definitions(-DUNIT_TESTS) #for FSM

set(CMAKE_CXX_FLAGS "-std=c++11") #weirdness when using enum as key in map on host c++11


set(gly_root_path ../../../..)
set(voc_include_PATH ${gly_root_path}/include)
set(voc_src_PATH ${gly_root_path}/src)

set(fsm_PATH ${gly_root_path}/FoundationServiceManager) 
set(fsm_voc_include_PATH ${fsm_PATH}/voc_framework/include) 
set(fsm_voc_src_PATH ${fsm_PATH}/voc_framework/src) 

file(GLOB asn_gen_files ${fsm_PATH}/voc_framework/src/signals/ccm_parsing/generated/*.c)


file(GLOB unittest_voc_SRC ./*.c*)

set(geely_signal_files ${fsm_PATH}/voc_framework/src/signals/geely_asn_common.cc) 

set(user_manager_PATH ../../../../UserManager)

include_directories(.)
if ( "$ENV{CC}" STREQUAL "/usr/bin/cc" ) # compiling for host machine
    include_directories(/usr/include/glib-2.0)
    include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
else()  # cross compiling for target
    include_directories(../../../../../../release/fs/devel/usr/include/glib-2.0)
    include_directories(../../../../../../release/fs/devel/usr/lib/glib-2.0/include)
endif()
include_directories(${fsm_generated_src_PATH})
include_directories(${voc_include_PATH})
include_directories(${fsm_voc_include_PATH})
include_directories(${voc_include_generated_PATH})
include_directories(${google_protobuf_include_PATH})
include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)

include_directories(
                    ${voc_src_PATH}/../test/unitest_geely/src
                    ${gly_root_path}
                    ${fsm_PATH}/voc_framework/src/signals/ccm_parsing/generated
                    ${fsm_PATH}/fsm_ccm/include
                    )

link_directories(../build_googletest/googlemock)
link_directories(../build_googletest/googlemock/gtest)

add_executable(${PROJECT_NAME} ${unittest_voc_SRC} ${geely_signal_files} ${asn_gen_files} )

target_link_libraries(${PROJECT_NAME}  gtestd gmockd pthread crypto sqlite3)
