#!/bin/bash

#./create_sp_persist_items.sh bare_key > ./result/bare_key.txt                            # just for test
./create_sp_persist_items.sh enum_key > ./result/enum_key.txt                            # config_resource.hpp
#./create_sp_persist_items.sh full_enum_key > ./result/full_enum_key.txt                  # just for test
./create_sp_persist_items.sh service_value_table > ./result/service_value_table.txt      # for mtp_signal_adapt.cc
./create_sp_persist_items.sh value_key_table > ./result/value_key_table.txt              # for service_provision.cc
./create_sp_persist_items.sh xml_per > ./result/xml_per.txt                              # for persistence_config_service_provision.xml
./create_sp_persist_items.sh hpp_per > ./result/hpp_per.txt                              # config_resource.hpp
