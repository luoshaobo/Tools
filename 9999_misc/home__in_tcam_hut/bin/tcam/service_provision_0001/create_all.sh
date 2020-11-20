#!/bin/bash

./create_sp_persist_items.sh bare_key > ./result/bare_key.txt
./create_sp_persist_items.sh enum_key > ./result/enum_key.txt
./create_sp_persist_items.sh full_enum_key > ./result/full_enum_key.txt
./create_sp_persist_items.sh service_value_table > ./result/service_value_table.txt
./create_sp_persist_items.sh value_key_table > ./result/value_key_table.txt
./create_sp_persist_items.sh xml_per > ./result/xml_per.txt
./create_sp_persist_items.sh hpp_per > ./result/hpp_per.txt
