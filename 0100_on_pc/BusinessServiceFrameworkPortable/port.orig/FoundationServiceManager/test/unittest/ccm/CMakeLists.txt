cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME unittest_fsm_ccm)
project(${PROJECT_NAME})

add_definitions(-D__UT_DLT_DISABLE_PRINTF__)
add_definitions(-DUNIT_TESTS)
add_definitions(-DSAVE_SIGNAL_FILES)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(marben_PATH ../../../../../marben/ASN-Runtime-lib/C++/Runtime)
set(marben_include_PATH_1 ${marben_PATH}/aip_lib/incl)
set(marben_include_PATH_2 ${marben_PATH}/asc_lib/incl)
set(marben_include_PATH_3 ${marben_PATH}/asb_lib/incl)
set(marben_include_PATH_4 ${marben_PATH}/asg_lib/incl)
set(marben_include_PATH_5 ${marben_PATH}/asp_lib/incl)
set(marben_include_PATH_6 ${marben_PATH}/asx_lib/incl)
#Marben parsers
set(marben_generated_PATH ../../../../MarbenGenerated)
set(marben_generated_ast_PATH ${marben_generated_PATH}/fsapi-assistance_call_ip011-schema-v1p13_6409/cpp/)

set(fsm_include_PATH ../../../fsm_ccm/include)
set(usermanager_include_PATH ../../../../UserManager/include)
set(unittest_utilites_PATH ../../../../UserManager/test/unittest/src/)
set(fsm_ccm_PATH ../../../fsm_ccm/src)
set(voc_framework_include_PATH ../../../voc_framework/include)
set(voc_framework_PATH ../../../voc_framework/src)
set(ccm_generated_PATH ../../../fsm_ccm/src/generated)
set(signals_generated_PATH ../../../voc_framework/src/signals/ccm_parsing/generated)
set(stubs_PATH ./stub)

set(marben_generated_SRC
    ${marben_generated_ast_PATH}/astcmp.cpp
    ${marben_generated_ast_PATH}/astcopy.cpp
    ${marben_generated_ast_PATH}/astctor.cpp
    ${marben_generated_ast_PATH}/asttable.cpp
    ${marben_generated_ast_PATH}/astdtor.cpp)

file(GLOB fsm_ccm_SRC
        ${fsm_ccm_PATH}/fsm_ccm.cc)

file(GLOB voc_framework_SRC
  ${voc_framework_PATH}/signals/assistance_call.cc
  ${voc_framework_PATH}/signals/basic_car_control.cc
  ${voc_framework_PATH}/signals/ccm.cc
  ${voc_framework_PATH}/signals/ccm_codec.cc
  ${voc_framework_PATH}/signals/ccm_signal.cc
  ${voc_framework_PATH}/signals/car_access_signal.cc
  ${voc_framework_PATH}/signals/marben_codec.cc
  ${voc_framework_PATH}/signals/signal.cc
  ${voc_framework_PATH}/signals/signal_factory.cc
  ${voc_framework_PATH}/signals/xml_codec.cc
  ${voc_framework_PATH}/signals/xml_payload.cc
  ${voc_framework_PATH}/transactions/transaction_id.cc
  )

file(GLOB user_manager_INCLUDE_FILES
  ${usermanager_include_PATH}/*.h)

file (GLOB stubs_SRC_FILES
  ${stubs_PATH}/*.h
  ${stubs_PATH}/*.cc)

file (GLOB unittest_utilities_SRC_FILES
  ${unittest_utilites_PATH}/utilities.h
  ${unittest_utilites_PATH}/utilities.cpp)

file(GLOB ccm_generated_SRC
    ${ccm_generated_PATH}/AddressCCM.c
    ${ccm_generated_PATH}/CCM-Header.c
    ${ccm_generated_PATH}/CCM-Payload.c
    ${ccm_generated_PATH}/ReceiptOptions.c
    ${ccm_generated_PATH}/Timestamp.c
    ${ccm_generated_PATH}/TimeToLive.c
    ${ccm_generated_PATH}/Recipients.c
    ${ccm_generated_PATH}/DeliveryOptions.c
    ${ccm_generated_PATH}/TransactionId.c
    ${ccm_generated_PATH}/NanoSeconds.c
    ${ccm_generated_PATH}/TimeToLiveSeconds.c
    ${ccm_generated_PATH}/TimeToLiveMilliSeconds.c
    ${ccm_generated_PATH}/StoreAndForwardOptions.c
    ${ccm_generated_PATH}/SubjectKeyIdentifierCCM.c
    ${ccm_generated_PATH}/SmsDeliveryOptions.c
    ${ccm_generated_PATH}/INT32.c)


file(GLOB signals_generated_SRC
  ${signals_generated_PATH}/*.c)

file(GLOB unittest_fsm_ccm_SRC
        "*.h"
        "*.cpp"
        "*.cc")

include_directories(.)
include_directories(${ccm_generated_PATH})
include_directories(${signals_generated_PATH})
include_directories(
  ${marben_include_PATH_1}
  ${marben_include_PATH_2}
  ${marben_include_PATH_3}
  ${marben_include_PATH_4}
  ${marben_include_PATH_5}
  ${marben_include_PATH_6})
include_directories(${fsm_include_PATH})
include_directories(${voc_framework_include_PATH})
include_directories(${usermanager_include_PATH})
include_directories(${unittest_utilites_PATH})
include_directories(${stubs_PATH})
include_directories(../googletest/googletest/include)
include_directories(${marben_generated_ast_PATH})
link_directories(../build_googletest/googlemock/gtest)

add_executable(${PROJECT_NAME}
               ${unittest_fsm_ccm_SRC}
               ${fsm_ccm_SRC}
               ${voc_framework_SRC}
               ${ccm_generated_SRC}
               ${signals_generated_SRC}
               ${user_manager_INCLUDE_FILES}
               ${unittest_utilities_SRC_FILES}
               ${stubs_SRC_FILES}
               ${marben_generated_SRC}
               )

ADD_LIBRARY(marben_runtime STATIC IMPORTED)
SET_TARGET_PROPERTIES(marben_runtime PROPERTIES IMPORTED_LOCATION ${marben_PATH}/lib/marben_runtime.a)

target_link_libraries(${PROJECT_NAME} marben_runtime gtestd pthread ssl crypto)
set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ..)
