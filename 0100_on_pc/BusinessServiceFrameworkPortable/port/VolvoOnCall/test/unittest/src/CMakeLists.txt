cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME unittest_voc)
project(${PROJECT_NAME})

add_definitions(-D__UT_DLT_DISABLE_PRINTF__ -DVOC_TESTS) # undefined, terimal output logs 
add_definitions(-DVOC_TESTS)
add_definitions(-DUNIT_TESTS) #for FSM

set (CMAKE_CXX_FLAGS "-std=c++11 -ftest-coverage -fprofile-arcs")

# Vehicle Comm
set(vehicle_comm_PATH  ../../../../VehicleComm)

set(vehicle_comm_include_PATH
    ${vehicle_comm_PATH}/include
    ${vehicle_comm_PATH}/interface
    ${vehicle_comm_PATH}/ts_comm
    ${vehicle_comm_PATH}/proxy_host
    ${vehicle_comm_PATH}/proxy
   )

#LocalConfig
set(lc_PATH  ../../../../LocalConfig)
set(lc_include_PATH  ${lc_PATH} ${lc_PATH}/interface ${lc_PATH}/include)
set(lc_src_PATH ${lc_PATH}/src)

# BLEManager
set(ble_manager_PATH  ../../../../BLEManager)
set(ble_manager_include_PATH ${ble_manager_PATH}/include)
set(ble_manager_interface_PATH ${ble_manager_PATH}/interface)

# OTP conti persistence:
set(conti_persistence_PATH ../../../../../conti/conti-persistence/pcl/include/public)

# IPCommandBroker
set(ipcb_PATH  ../../../../IPCommandBroker)
set(ipcb_include_PATH ${ipcb_PATH}/include)
set(ipcb_interface_PATH ${ipcb_PATH}/interface)

# VoC
set(voc_include_PATH ../../../include)
set(voc_include_generated_PATH ../../../src/signals/ccm_parsing/generated)
set(voc_src_PATH ../../../src)
set(voc_unit_test_dlt_PATH ../../../test/unittest/src/dlt)
set(voc_unit_test_src_PATH ../../../test/unittest/src)
set(voc_persist_mgr_PATH ../../../voc_persist_data_mgr/include)

# FSM
set(fsm_PATH ../../../../FoundationServiceManager)
set(user_manager_PATH ../../../../UserManager)
set(fsm_include_PATH  ${fsm_PATH}/fsm_ccm/include ${fsm_PATH}/transfermanager/include ${fsm_PATH}/voc_framework/include)
set(user_manager_include_PATH ${user_manager_PATH}/include)
set(fsm_ccm_src_PATH ${fsm_PATH}/fsm_ccm/src)
set(fsm_generated_src_PATH ${fsm_PATH}/fsm_ccm/src/generated)
set(user_manager_src_PATH ${user_manager_PATH}/src)
set(fsm_ut_PATH ${fsm_PATH}/test/unittest)
set(usermanager_ut_src_PATH ${user_manager_PATH}/test/unittest/src)
set(fsm_voc_include_PATH ${fsm_PATH}/voc_framework/include)
set(fsm_configfs_include_PATH ${fsm_PATH}/fs_config/interface/public/include)
set(fsm_voc_src_PATH ${fsm_PATH}/voc_framework/src)
set(fsm_fs_config_include_priv_PATH ${fsm_PATH}/fs_config/interface/private)
set(fsm_fs_config_src_PATH ${fsm_PATH}/fs_config/src/fscfgd)
set(fsm_fs_config_common_PATH ${fsm_PATH}/fs_config/src/fscfg_common)
set(fsm_signals_marben_gen_PATH ${fsm_PATH}/voc_framework/src/signals/marben_parsing/marben_gen_cplus)
set(fsm_src_vds_marben_adapt_PATH ${fsm_PATH}/voc_framework/src/signal_adapt/vds_marben_adapt)
set(fsm_src_vds_marben_adapt_PATH ${fsm_PATH}/voc_framework/src/signal_adapt/vds_marben_adapt)
set(fsm_src_vds_gly_adapt_PATH ${fsm_PATH}/voc_framework/src/signal_adapt/gly_signal_adapt)
set(fsm_voc_signal_source_include_PATH ${fsm_voc_include_PATH}/voc_framework/signal_sources)


# VolvoPositioningManager
set(vpm_PATH ../../../../VolvoPositioningManager)
set(vpm_interface_PATH ${vpm_PATH}/interface)

#Marben
set(marben_PATH ../../../../../marben/ASN-Runtime-lib/C++/Runtime)
set(marben_include_PATH_1 ${marben_PATH}/aip_lib/incl)
set(marben_include_PATH_2 ${marben_PATH}/asc_lib/incl)
set(marben_include_PATH_3 ${marben_PATH}/asb_lib/incl)
set(marben_include_PATH_4 ${marben_PATH}/asg_lib/incl)
set(marben_include_PATH_5 ${marben_PATH}/asp_lib/incl)
set(marben_include_PATH_6 ${marben_PATH}/asx_lib/incl)

#Marben parsers
set(marben_generated_PATH ../../../../MarbenGenerated)
set(marben_generated_bcc_PATH ${marben_generated_PATH}/fsapi-basic_car_control_schema-v1p10_4323/cpp/)
set(marben_generated_ast_PATH ${marben_generated_PATH}/fsapi-assistance_call_ip011-schema-v1p13_6409/cpp/)

# Other include paths
set(communicationmanager_include_PATH ../../../../CommunicationManager/include)
set(serviceframework_include_PATH ../../../../ServiceFramework/include)
set(google_protobuf_include_PATH ../../../../../opensource/protobuf/src)
set(conti_psapcall_include_PATH ../../../../../conti/conti-psapcall/src/inc)
set(conti_nadif_include_PATH ../../../../../conti/conti-nadif/src/inc)
set(conti_halif_include_PATH ../../../../../conti/conti-halif/include)
set(conti_nadif_config_include_PATH ../../../../../conti/conti-nadif/src/config)
set(conti_pm_include_PATH ../../../../../conti/conti-power-mgr/include)
set(serviceframework_PATH ../../../../ServiceFramework)


include_directories(.)
if ( "$ENV{CC}" STREQUAL "/usr/bin/cc" ) # compiling for host machine
    include_directories(/usr/include/glib-2.0)
    include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
else()  # cross compiling for target
    include_directories(../../../../../../release/fs/devel/usr/include/glib-2.0)
    include_directories(../../../../../../release/fs/devel/usr/lib/glib-2.0/include)
endif()
include_directories(../../../../../../release/fs/devel/usr/include)
include_directories(../../../../../../release/fs/devel/usr/include/vehiclecomm)

include_directories(${conti_persistence_PATH})
include_directories(${vehicle_comm_include_PATH})
include_directories(${lc_include_PATH})
include_directories(${ble_manager_include_PATH})
include_directories(${ble_manager_interface_PATH})
include_directories(${fsm_include_PATH})
include_directories(${fsm_generated_src_PATH})
include_directories(${user_manager_include_PATH})
include_directories(${voc_include_PATH})
include_directories(${fsm_voc_include_PATH})
include_directories(${fsm_voc_signal_source_include_PATH})
include_directories(${fsm_configfs_include_PATH})
include_directories(${voc_include_generated_PATH})
include_directories(${fsm_fs_config_include_priv_PATH})
include_directories(${communicationmanager_include_PATH}/libclientcommunicationmanager)
include_directories(${communicationmanager_include_PATH}/communicationmanager)
include_directories(${communicationmanager_include_PATH}/libinterprocesscommunication)
include_directories(${serviceframework_include_PATH})
include_directories(${google_protobuf_include_PATH})
include_directories(${conti_psapcall_include_PATH})
include_directories(${conti_nadif_include_PATH})
include_directories(${conti_halif_include_PATH})
include_directories(${conti_nadif_config_include_PATH})
include_directories(../googletest/googletest/include)
include_directories(../googletest/googlemock/include)
include_directories(
  ${marben_include_PATH_1}
  ${marben_include_PATH_2}
  ${marben_include_PATH_3}
  ${marben_include_PATH_4}
  ${marben_include_PATH_5}
  ${marben_include_PATH_6})
include_directories(${marben_generated_bcc_PATH})
include_directories(${marben_generated_ast_PATH})

include_directories(${fsm_signals_marben_gen_PATH})
include_directories(${fsm_src_vds_marben_adapt_PATH})

include_directories(${vpm_interface_PATH})
include_directories(${voc_unit_test_dlt_PATH})
include_directories(${conti_pm_include_PATH})
include_directories(${voc_unit_test_src_PATH})
include_directories(${voc_persist_mgr_PATH})
include_directories(${ipcb_include_PATH})
include_directories(${ipcb_interface_PATH})

 set(voc_SRC
    # ${voc_src_PATH}/voc.cc
    # ${voc_src_PATH}/voc_utils.cc
    # ${voc_src_PATH}/features/basic_car_control_feature.cc
    # ${voc_src_PATH}/features/car_access_feature.cc
    # ${voc_src_PATH}/signals/aes_ble_da_key_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_cert_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_cert_actor_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_cert_nearfield_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_ca_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_car_actor_cert_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_csr_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_csr_actor_voc_frame.cc
    # ${voc_src_PATH}/signals/aes_csr_nearfield_voc_frame.cc
    # ${voc_src_PATH}/signals/basic_car_control.cc
    # ${voc_src_PATH}/signals/basic_car_control_signal.cc
    # ${voc_src_PATH}/signals/bcc_cl_001_signal.cc
    # ${voc_src_PATH}/signals/bcc_cl_002_signal.cc
    # ${voc_src_PATH}/signals/bcc_cl_010_signal.cc
    # ${voc_src_PATH}/signals/bcc_cl_011_signal.cc
    # ${voc_src_PATH}/signals/bcc_cl_020_signal.cc
    # ${voc_src_PATH}/signals/ca_cat_001_signal.cc
    # ${voc_src_PATH}/signals/ca_cat_002_signal.cc
    # ${voc_src_PATH}/signals/ca_del_001_signal.cc
    # ${voc_src_PATH}/signals/ca_del_002_signal.cc
    # ${voc_src_PATH}/signals/ca_dpa_001_signal.cc
    # ${voc_src_PATH}/signals/ca_dpa_002_signal.cc
    # ${voc_src_PATH}/signals/ca_dpa_020_signal.cc
    # ${voc_src_PATH}/signals/ca_dpa_021_signal.cc
    # ${voc_src_PATH}/signals/device_pairing_id.cc
    # ${voc_src_PATH}/signals/ecdhe_approval_voc_frame.cc
    # ${voc_src_PATH}/signals/ecdhe_request_voc_frame.cc
    # ${voc_src_PATH}/signals/voc_frame_codec.cc
    # ${voc_src_PATH}/transactions/admin_confirmation_transaction.cc
    # ${voc_src_PATH}/transactions/car_locator_advert_transaction.cc
    # ${voc_src_PATH}/transactions/catalogue_upload_transaction.cc
    # ${voc_src_PATH}/transactions/device_pairing_transaction.cc
    # ${voc_src_PATH}/transactions/key_confirmation_transaction.cc
    # ${voc_src_PATH}/transactions/list_paired_devices_transaction.cc
    # ${voc_src_PATH}/transactions/privacy_notification_transaction.cc
    # ${voc_src_PATH}/transactions/role_selected_notification_transaction.cc
      ${voc_src_PATH}/signals/geely/gly_vds_dlagent_signal.cc
      ${voc_src_PATH}/signals/geely/gly_vds_mta_signal.cc
      ${voc_src_PATH}/mta/mta_transaction.cc
      ${voc_src_PATH}/download_agent/car_downloadagent_transaction.cc
      ${voc_unit_test_src_PATH}/fake/fake_powermgr.cc
      ${voc_unit_test_src_PATH}/fake/fake_vdmsg_signal.cc
      ${voc_unit_test_src_PATH}/fake/fake_vdm_asn.cc
      ${voc_unit_test_src_PATH}/fake/fake_voc_persist_data_manager.cc
      ${voc_unit_test_src_PATH}/fake/fake_vocmo_signal_source.cc
      ${voc_unit_test_src_PATH}/fake/fake_vehicle_comm_signal_source.cc
      ${voc_unit_test_src_PATH}/fake/fake_vc_proxy.cc
      ${voc_unit_test_src_PATH}/fake/fake_control.cc
      ${voc_unit_test_src_PATH}/fake/fake_persistence_client.cc
      ${voc_unit_test_src_PATH}/fake/fake_common.cc
      ${voc_unit_test_src_PATH}/fake/fake_vds_marben_adapt_gen.cc
      ${voc_unit_test_src_PATH}/fake/fake_ipcb_signal_source.cc
      ${voc_unit_test_src_PATH}/fake/fake_ipcb_service.cc
      ${voc_unit_test_src_PATH}/fake/fake_downloadagent_fileio.cc
    )

file(GLOB generated_sources ../../../src/signals/ccm_parsing/generated/*.c)

file(GLOB fsm_ccm_generated_sources ${fsm_generated_src_PATH}/*.c)

set(usermanager_SRC
  ${user_manager_src_PATH}/usermanager.cc
  ${user_manager_src_PATH}/keystore.cc
  ${user_manager_src_PATH}/usermanager_interface.cc
    )

set(fsm_ccm_SRC
    ${fsm_ccm_src_PATH}/fsm_ccm.cc
    )

# set (fsm_generated_SRC
    # ${fsm_generated_src_PATH}/AddressCCM.c
    # ${fsm_generated_src_PATH}/CCM-Header.c
    # ${fsm_generated_src_PATH}/CCM-Payload.c
    # ${fsm_generated_src_PATH}/ContentType.c
    # ${fsm_generated_src_PATH}/Envelope.c
    # ${fsm_generated_src_PATH}/ReceiptOptions.c
    # ${fsm_generated_src_PATH}/Timestamp.c
    # ${fsm_generated_src_PATH}/TimeToLive.c
    # ${fsm_generated_src_PATH}/Recipients.c
    # ${fsm_generated_src_PATH}/DeliveryOptions.c
    # ${fsm_generated_src_PATH}/TransactionId.c
    # ${fsm_generated_src_PATH}/NanoSeconds.c
    # ${fsm_generated_src_PATH}/TimeToLiveSeconds.c
    # ${fsm_generated_src_PATH}/TimeToLiveMilliSeconds.c
    # ${fsm_generated_src_PATH}/StoreAndForwardOptions.c
    # ${fsm_generated_src_PATH}/SubjectKeyIdentifierCCM.c
    # ${fsm_generated_src_PATH}/SmsDeliveryOptions.c
    # ${fsm_generated_src_PATH}/INT32.c)

set(fsm_voc_SRC
  ${fsm_voc_src_PATH}/signals/timeout_signal.cc
  ${fsm_voc_src_PATH}/signal_sources/timeout_signal_source.cc
  ${fsm_voc_src_PATH}/signal_sources/timeout_receiver.cc
  ${fsm_voc_src_PATH}/signals/signal.cc
  ${fsm_voc_src_PATH}/transactions/transaction.cc
  ${fsm_voc_src_PATH}/transactions/transaction_id.cc
  ${fsm_voc_src_PATH}/transactions/smart_transaction.cc
  ${fsm_voc_src_PATH}/signal_adapt/gly_signal_adapt/dlagent_signal_adapt.cc
  ${fsm_voc_src_PATH}/signal_adapt/gly_signal_adapt/mta_signal_adapt.cc
  ${fsm_voc_src_PATH}/signal_adapt/gly_signal_adapt/signal_adapt_base.cc
  #${fsm_voc_src_PATH}/signal_adapt/vds_marben_adapt/vds_marben_adapt_gen.cc
    # ${fsm_voc_src_PATH}/features/feature.cc
    # ${fsm_voc_src_PATH}/features/feature_config.cc
    # ${fsm_voc_src_PATH}/signals/assistance_call.cc
    # ${fsm_voc_src_PATH}/signals/car_access_signal.cc
    # ${fsm_voc_src_PATH}/signals/ccm.cc
    # ${fsm_voc_src_PATH}/signals/ccm_codec.cc
    # ${fsm_voc_src_PATH}/signals/ccm_signal.cc
    # ${fsm_voc_src_PATH}/signals/cloud_resource.cc
    # ${fsm_voc_src_PATH}/signals/entry_point_signal.cc
    # ${fsm_voc_src_PATH}/signals/features_signal.cc
    # ${fsm_voc_src_PATH}/signals/location_data.cc
    # ${fsm_voc_src_PATH}/signals/marben_codec.cc
    # ${fsm_voc_src_PATH}/signals/response_info_data.cc
    # ${fsm_voc_src_PATH}/signals/signal_factory.cc
    # ${fsm_voc_src_PATH}/signals/xml_codec.cc
    # ${fsm_voc_src_PATH}/signals/xml_payload.cc
    # ${fsm_voc_src_PATH}/signal_sources/ble_signal_source.cc
    # ${fsm_voc_src_PATH}/signal_sources/car_access_signal_source.cc
    # ${fsm_voc_src_PATH}/signal_sources/internal_signal_source.cc
    # ${fsm_voc_src_PATH}/signal_sources/signal_source.cc
    # ${fsm_voc_src_PATH}/signal_sources/ssm_signal_source.cc
    # ${fsm_voc_src_PATH}/signal_sources/vocmo_signal_source.cc
    # ${fsm_voc_src_PATH}/../../test/unittest/voc_framework/src/coma_mock.cc
  )

# set(fsm_fs_config_fscfgd_SRC
    # ${fsm_fs_config_src_PATH}/expiry_notification_signal.cpp)

# set(lc_SRC
    # ${lc_src_PATH}/local_config.cpp)

# set(fsm_fs_config_common_SRC
    # ${fsm_fs_config_common_PATH}/bus_names.cpp)

  set(marben_generated_SRC
     ${marben_generated_bcc_PATH}/bcccmp.cpp
     ${marben_generated_bcc_PATH}/bcccopy.cpp
     ${marben_generated_bcc_PATH}/bccctor.cpp
     ${marben_generated_bcc_PATH}/bccdtor.cpp
     ${marben_generated_bcc_PATH}/bcctable.cpp
     ${marben_generated_ast_PATH}/astcmp.cpp
     ${marben_generated_ast_PATH}/astcopy.cpp
     ${marben_generated_ast_PATH}/astctor.cpp
     ${marben_generated_ast_PATH}/astdtor.cpp
     ${marben_generated_ast_PATH}/asttable.cpp)

  set(serviceframework_SRC
     ${serviceframework_PATH}/src/timestamp.cpp

     )


set(unittest_voc_SRC
  main.cc
  sample1.cc
  sample1_unittest.cc
  unittest_smart_transaction.cc
  unittest_mta_transaction.cc
  unittest_dla_transaction.cc
    # ble_manager_mock.cc
    # fs_config_mock.cc
    # test_classes.cc
    # unittest_base_feature.cc
    # unittest_base_transaction.cc
    # unittest_ccm_signals.cc
    # unittest_config.cc
    # unittest_voc_frames.cc
    # unittest_ipcb_signals.cc
    # unittest_signal_source.cc
    # unittest_transactions.cc
    # unittest_car_access_feature.cc
    # generate_cloudmock_content.cc
    # local_config_stub.cpp
  )

link_directories(../build_googletest/googlemock)
link_directories(../build_googletest/googlemock/gtest)
link_directories(../build_googletest/lib)
link_directories(../../../../../../tools/ModuleTest/GTestLibs)

link_directories(../lib)
link_directories(/usr/bin)

add_executable(${PROJECT_NAME}
         ${unittest_voc_SRC}
         ${fsm_voc_SRC}
         ${fsm_ccm_SRC}
         ${fsm_ccm_generated_sources}
         ${usermanager_SRC}
	 ${serviceframework_SRC}
         ${voc_SRC}
         # ${generated_sources}
         # ${fsm_generated_SRC}
         # ${usermanager_pcl_stubs_SRC}
         # ${fsm_fs_config_fscfgd_SRC}
         # ${fsm_fs_config_common_SRC}
         # ${marben_generated_SRC}
         )

ADD_LIBRARY(marben_runtime STATIC IMPORTED)
SET_TARGET_PROPERTIES(marben_runtime PROPERTIES IMPORTED_LOCATION ${marben_PATH}/lib/marben_runtime.a)

target_link_libraries(${PROJECT_NAME}
            marben_runtime
            gtestd
            gmockd
            crypto
            dl
            pthread
            sqlite3
            )
