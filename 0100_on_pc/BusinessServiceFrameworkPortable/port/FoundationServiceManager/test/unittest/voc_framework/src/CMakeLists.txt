cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME unittest_voc_framework)
project(${PROJECT_NAME})

add_definitions(-D__UT_DLT_DISABLE_PRINTF__)
add_definitions(-DVOC_TESTS)
add_definitions(-DUNIT_TESTS)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(coma_PATH ../../../../../CommunicationManager)
set(fsm_PATH ../../../../../FoundationServiceManager)
set(volvo_on_call_PATH ../../../../../VolvoOnCall)
set(fsm_ccm_PATH ${fsm_PATH}/fsm_ccm)
set(fsm_ccm_generated_PATH ${fsm_ccm_PATH}/src/generated)
set(usermanager_PATH ../../../../../UserManager)
set(voc_framework_PATH ${fsm_PATH}/voc_framework)
set(fs_config_PATH ${fsm_PATH}/fs_config)

# Include paths
set(transfermanager_include_PATH ${voc_framework_PATH}/../transfermanager/include)
set(communicationmanager_include_PATH ${voc_framework_PATH}/../../CommunicationManager/include)
set(usermanager_include_PATH ${voc_framework_PATH}/../../UserManager/include)
set(serviceframework_include_PATH ${voc_framework_PATH}/../../ServiceFramework/include)
set(google_protobuf_include_PATH ${voc_framework_PATH}/../../../opensource/protobuf/src)
set(conti_psapcall_include_PATH ${voc_framework_PATH}/../../../conti/conti-psapcall/src/inc)
set(conti_nadif_include_PATH ${voc_framework_PATH}/../../../conti/conti-nadif/src/inc)
set(conti_nadif_config_include_PATH ${voc_framework_PATH}/../../../conti/conti-nadif/src/config)
set(marben_PATH_at_link ../../../../../marben/ASN-Runtime-lib/C++/Runtime) #the directory depth is differnt at linka dn at compile
set(marben_PATH ../../../../../../marben/ASN-Runtime-lib/C++/Runtime)
set(marben_include_PATH_1 ${marben_PATH}/aip_lib/incl)
set(marben_include_PATH_2 ${marben_PATH}/asc_lib/incl)
set(marben_include_PATH_3 ${marben_PATH}/asb_lib/incl)
set(marben_include_PATH_4 ${marben_PATH}/asg_lib/incl)
set(marben_include_PATH_5 ${marben_PATH}/asp_lib/incl)
set(marben_include_PATH_6 ${marben_PATH}/asx_lib/incl)

include_directories(.)
include_directories(${fsm_PATH}/voc_framework/include)
include_directories(${fsm_PATH}/fsm_ccm/include)
include_directories(${fsm_PATH}/fsm_ccm/src/generated)
include_directories(${fsm_PATH}/test/unittest/ccm)
include_directories(${voc_framework_PATH}/include)
include_directories(${volvo_on_call_PATH}/include)
include_directories(${volvo_on_call_PATH}/src/signals/ccm_parsing/generated)
include_directories(${voc_framework_PATH}/src/signals/ccm_parsing/generated)
include_directories(${fs_config_PATH}/interface/public/include)
include_directories(${usermanager_include_PATH})
include_directories(${usermanager_PATH}/test/unittest/src)
include_directories(${transfermanager_include_PATH})
include_directories(${communicationmanager_include_PATH}/libclientcommunicationmanager)
include_directories(${communicationmanager_include_PATH}/communicationmanager)
include_directories(${communicationmanager_include_PATH}/libinterprocesscommunication)
include_directories(${serviceframework_include_PATH})
include_directories(${google_protobuf_include_PATH})
include_directories(${conti_psapcall_include_PATH})
include_directories(${conti_nadif_include_PATH})
include_directories(${conti_nadif_config_include_PATH})
include_directories(../../googletest/googletest/include)
include_directories(../../googletest/googlemock/include)
include_directories(
  ${marben_include_PATH_1}
  ${marben_include_PATH_2}
  ${marben_include_PATH_3}
  ${marben_include_PATH_4}
  ${marben_include_PATH_5}
  ${marben_include_PATH_6})

link_directories(../build_googletest/googlemock)
link_directories(../build_googletest/googlemock/gtest)

file(GLOB generated_sources ../../../../voc_framework/src/signals/ccm_parsing/generated/*.c)

set (fsm_ccm_generated_SRC
    ${fsm_ccm_generated_PATH}/AddressCCM.c
    ${fsm_ccm_generated_PATH}/CCM-Header.c
    ${fsm_ccm_generated_PATH}/CCM-Payload.c
    ${fsm_ccm_generated_PATH}/ReceiptOptions.c
    ${fsm_ccm_generated_PATH}/Timestamp.c
    ${fsm_ccm_generated_PATH}/TimeToLive.c
    ${fsm_ccm_generated_PATH}/Recipients.c
    ${fsm_ccm_generated_PATH}/DeliveryOptions.c
    ${fsm_ccm_generated_PATH}/TransactionId.c
    ${fsm_ccm_generated_PATH}/NanoSeconds.c
    ${fsm_ccm_generated_PATH}/TimeToLiveSeconds.c
    ${fsm_ccm_generated_PATH}/TimeToLiveMilliSeconds.c
    ${fsm_ccm_generated_PATH}/StoreAndForwardOptions.c
    ${fsm_ccm_generated_PATH}/SubjectKeyIdentifierCCM.c
    ${fsm_ccm_generated_PATH}/SmsDeliveryOptions.c
    ${fsm_ccm_generated_PATH}/INT32.c
    )

set (volvo_on_call_ccm_generated_SRC
    ${volvo_on_call_PATH}/src/signals/ccm_parsing/generated/BCC-CL-002.c
    ${volvo_on_call_PATH}/src/signals/ccm_parsing/generated/LocationAdvertKind.c
    ${volvo_on_call_PATH}/src/signals/ccm_parsing/generated/LocationAdvertParameters.c
    ${volvo_on_call_PATH}/src/signals/ccm_parsing/generated/Adverts.c
    )

set (volvo_on_call_SRC
    ${volvo_on_call_PATH}/src/signals/bcc_cl_002_signal.cc
    )

set(unittest_voc_framework_SRC
    fs_config_stub.cc
    coma_mock.cc
    ../../ccm/stub/keystore_stub.cc
    main.cc
    utils.cc
    unittest_ble_signal_source.cc
    unittest_vocmo_signal_source.cc
    unittest_internal_signal_source.cc
    )


set(voc_framework_SRC
    ${voc_framework_PATH}/src/features/feature.cc
    ${voc_framework_PATH}/src/features/feature_config.cc
    ${voc_framework_PATH}/src/signals/car_access_signal.cc
    ${voc_framework_PATH}/src/signals/cloud_resource.cc
    ${voc_framework_PATH}/src/signals/ccm.cc
    ${voc_framework_PATH}/src/signals/ccm_codec.cc
    ${voc_framework_PATH}/src/signals/ccm_signal.cc
    ${voc_framework_PATH}/src/signals/entry_point_signal.cc
    ${voc_framework_PATH}/src/signals/features_signal.cc
    ${voc_framework_PATH}/src/signals/location_data.cc
    ${voc_framework_PATH}/src/signals/marben_codec.cc
    ${voc_framework_PATH}/src/signals/response_info_data.cc
    ${voc_framework_PATH}/src/signals/signal.cc
    ${voc_framework_PATH}/src/signals/signal_factory.cc
    ${voc_framework_PATH}/src/signals/timeout_signal.cc
    ${voc_framework_PATH}/src/signals/xml_codec.cc
    ${voc_framework_PATH}/src/signals/xml_payload.cc
    ${voc_framework_PATH}/src/signal_sources/ble_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/car_access_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/vocmo_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/internal_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/ssm_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/timeout_signal_source.cc
    ${voc_framework_PATH}/src/signal_sources/timeout_receiver.cc
    ${voc_framework_PATH}/src/transactions/smart_transaction.cc
    ${voc_framework_PATH}/src/transactions/transaction.cc
    ${voc_framework_PATH}/src/transactions/transaction_id.cc
    )

set(fsm_ccm_SRC
    ${fsm_ccm_PATH}/src/fsm_ccm.cc
    )

set(usermanager_SRC
    ${fsm_PATH}/test/unittest/ccm/certUtils.cpp
    ${fsm_PATH}/test/unittest/ccm/stub/keystore_stub.cc
    ${fsm_PATH}/test/unittest/ccm/stub/usermanager_stub.cc
    )

add_executable(${PROJECT_NAME}
               ${unittest_voc_framework_SRC}
               ${voc_framework_SRC}
               ${fsm_ccm_SRC}
               ${fsm_ccm_generated_SRC}
               ${volvo_on_call_ccm_generated_SRC}
               ${volvo_on_call_SRC}
               ${generated_sources}
               ${usermanager_SRC}
               )

ADD_LIBRARY(marben_runtime STATIC IMPORTED)
SET_TARGET_PROPERTIES(marben_runtime PROPERTIES IMPORTED_LOCATION ${marben_PATH_at_link}/lib/marben_runtime.a)

target_link_libraries(${PROJECT_NAME}
                      gtestd
                      gmockd
                      pthread
                      crypto
                      marben_runtime
                      )
