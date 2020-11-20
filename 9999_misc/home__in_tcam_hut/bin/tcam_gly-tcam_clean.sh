#!/bin/bash

find -name "build" | xargs rm -fr
find -name ".d" | xargs rm -fr
find -name ".git" | xargs rm -fr

find -name "*.dll" | xargs rm -f
find -name "*.lib" | xargs rm -f

find -name "*.o" | xargs rm -f
find -name "*.so" | xargs rm -f
find -name "lib*.so.*" | xargs rm -f
find -name "*.a" | xargs rm -f
find -name "lib*.a.*" | xargs rm -f
find -name "*.deb" | xargs rm -f

find -type f -name "*-client" | xargs rm -f
find -type f -name "*_client" | xargs rm -f
find -type f -name "*-test" | xargs rm -f
find -type f -name "*_test" | xargs rm -f
find -type f -name "*-generated.*" | xargs rm -f
find -type f -name "*_generated.*" | xargs rm -f

find -type f -name "voc" | xargs rm -f
find -type f -name "voc_dbus_client" | xargs rm -f
find -type f -name "voc_diag_gdbus_client_test" | xargs rm -f
find -type f -name "protoc.exe" | xargs rm -f
find -type f -name "fscfgd" | xargs rm -f
find -type f -name "dbcmdtool" | xargs rm -f
find -type f -name "ccm_test" | xargs rm -f
find -type f -name "thermal-mgr" | xargs rm -f
find -type f -name "tswthm" | xargs rm -f
find -type f -name "tswthm_test_client" | xargs rm -f
find -type f -name "volvo_conn_manager" | xargs rm -f
find -type f -name "vpom" | xargs rm -f
find -type f -name "sim-manager" | xargs rm -f
find -type f -name "tcam-diag" | xargs rm -f
find -type f -name "memory_statistics" | xargs rm -f
find -type f -name "tcam-dc-ethernet" | xargs rm -f
find -type f -name "tcam-ua-android" | xargs rm -f
find -type f -name "tcam-ua-config" | xargs rm -f
find -type f -name "tcam-ua-config-test" | xargs rm -f
find -type f -name "tcam-ua-vuc" | xargs rm -f
find -type f -name "tcam-ua-vuc-test" | xargs rm -f
find -type f -name "usermanager_tool" | xargs rm -f
find -type f -name "vcnm_gdbus_client" | xargs rm -f
find -type f -name "mdns-repeater" | xargs rm -f
find -type f -name "vpom_gdbus_client" | xargs rm -f















