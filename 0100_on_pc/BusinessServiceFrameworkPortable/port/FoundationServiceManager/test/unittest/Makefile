#######################################################################################
# Copyright (C) 2017 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#
# Author:       Iulian Sirghi
#
# Date:         13-Jan-2017
#
# Description:  Downloads automatically googletest's git from the Internet repository
#               builds googletest, unittest_fsm_ccm, unittest_user_manager with CMake.
#               The following packages are required on your local virtual machine:
#               cmake, openssl, libssl-dev, sqlite3, libsqlite3-dev
#               Install them with:
#               sudo apt-get install cmake openssl libssl-dev sqlite3 libsqlite3-dev
#######################################################################################
all: \
	unittest_libfscfg 
	#unittest_voc_framework 
	#unittest_libfscfg
	#unittest_fsm_ccm \
	#unittest_transfermanager

clean_marben:
	if file ../../../../marben/ASN-Runtime-lib/C++/Runtime/aip_lib/src/aipbtk.o | grep -qi 'arm'; then \
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make runtime_clean; \
	fi;

build_marben: clean_marben
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make

# unittest_fsm_ccm: build_unittest_fsm_ccm ccm/unittest_fsm_ccm.cpp
	# $(MAKE) -C build_unittest_fsm_ccm

build_unittest_libfscfg/unittest_libfscfg: build_unittest_libfscfg fs_config/unittest_libfscfg/unittest_libfscfg.cc
	$(MAKE) -C build_unittest_libfscfg

build_unittest_libfscfg: build_googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./fs_config/unittest_libfscfg -Bbuild_unittest_libfscfg

unittest_libfscfg: build_unittest_libfscfg/unittest_libfscfg
	cp build_unittest_libfscfg/unittest_libfscfg .

# Enable transfermanager unittest if and when have COMA stub available.
# unittest_transfermanager: build_unittest_transfermanager transfermanager/unittest_transfermanager.cpp
	# $(MAKE) -C build_unittest_transfermanager

# build_unittest_transfermanager: build_googletest
	# cmake -DCMAKE_BUILD_TYPE=Debug -H./transfermanager -Bbuild_unittest_transfermanager

# build_unittest_voc_framework: build_googletest build_marben
	# cmake -DCMAKE_BUILD_TYPE=Debug -H./voc_framework/src -Bbuild_unittest_voc_framework

# build_unittest_voc_framework/unittest_voc_framework: build_unittest_voc_framework voc_framework/src/main.cc voc_framework/src/unittest_ble_signal_source.cc
	# $(MAKE) -C build_unittest_voc_framework

# unittest_voc_framework: build_unittest_voc_framework/unittest_voc_framework
	# cp build_unittest_voc_framework/unittest_voc_framework .

# build_unittest_fsm_ccm: build_googletest build_marben
	# cmake -DCMAKE_BUILD_TYPE=Debug -H./ccm -Bbuild_unittest_fsm_ccm

build_googletest: googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./googletest -Bbuild_googletest
	$(MAKE) -C build_googletest

googletest:
	git clone https://github.com/google/googletest.git

run:
	./unittest_libfscfg
	# ./unittest_fsm_ccm
	# ./unittest_voc_framework
	# ./unittest_transfermanager

clean: clean_marben
	$(RM) *.bin
	$(RM) unittest_libfscfg
	# $(RM) unittest_fsm_ccm
	# $(RM) unittest_voc_framework
	# $(RM) unittest_transfermanager
	$(RM) -r build_unittest_libfscfg
	# $(RM) -r build_unittest_fsm_ccm
	# $(RM) -r build_unittest_transfermanager
	# $(RM) -r build_unittest_voc_framework
	$(RM) -r build_googletest
	$(RM) -r *.sqlite3  # generated test databases for http cache db
	$(RM) -r googletest