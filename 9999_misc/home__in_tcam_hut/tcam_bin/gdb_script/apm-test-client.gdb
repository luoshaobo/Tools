target remote :12346

set sysroot /home/uidv4956/tcam_root/tcam_src_root.latest/release/fs/devel
#dir /home/uidv4956/tcam_root/tcam_src_root_03.19.16.01/release/fs/devel/usr/lib

#b rmc_global.cpp:141
#b gly_vds_rmc_signal.cc:40

b main
#b voc.cc:173
c 

#sharedlibrary libfsm_vocframework.so
#b fsm::customerInfoMessaging_serviceCb

#sharedlibrary libserviceframework.so
#b StatemachineMaster2::startStatemachine
#b statemachinemaster2.cpp:268
#b voc_user.cc:689
#b theft_notification.cc:262
b apm_test.c:448
