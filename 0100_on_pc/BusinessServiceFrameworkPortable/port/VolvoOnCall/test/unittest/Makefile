#################################################################
# Copyright (C) 2017 Continental Automotive GmbH
#
# Author:       Iulian Sirghi
#
# Date:         13-Jan-2017
#
# Description:  Grabs googletest's git
#               builds googletest, ccm, unittest_ccm with CMake
#               "cmake" and "libssl-dev" packages are required
#               on your local machine, i.e. call:
#               sudo apt-get install cmake libglib2.0-dev libsqlite3-dev libssl-dev sqlite3
#               Should compile on Ubuntu x64: call "make".
#               For target call:
# make CC=../../../../../release/fs/devel/usr/bin/arm-cas-linux-gnueabi-gcc  CXX=../../../../../release/fs/devel/usr/bin/arm-cas-linux-gnueabi-g++ LD=../../../../../release/fs/devel/usr/bin/arm-cas-linux-gnueabi-ld
#               In case of weird build issues call "make clean" before build
#
#################################################################
all: unittest_voc

clean_marben:
	if file ../../../../marben/ASN-Runtime-lib/C++/Runtime/aip_lib/src/aipbtk.o | grep -qi 'arm'; then \
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make runtime_clean; \
	fi;

build_marben: clean_marben
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make

build_unittest_voc: build_googletest build_marben
	cmake -DCMAKE_BUILD_TYPE=Debug -H./src -Bbuild_unittest_voc

#build_unittest_voc/unittest_voc: build_unittest_voc src/main.cc src/unittest_signal_source.cc src/unittest_base_transaction.cc src/unittest_base_feature.cc src/unittest_ccm_signals.cc
build_unittest_voc/unittest_voc: build_unittest_voc src/main.cc src/sample1_unittest.cc src/unittest_smart_transaction.cc
	$(MAKE) -C build_unittest_voc

unittest_voc: build_unittest_voc/unittest_voc
	cp build_unittest_voc/unittest_voc .

build_googletest: googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./googletest -Bbuild_googletest
	$(MAKE) -C build_googletest

googletest:
	git clone https://github.com/google/googletest.git

clean:
	$(RM) *.bin *.pem *.shared_secret
	$(RM) unittest_voc
	$(RM) -r build_unittest_voc
	$(RM) -r build_googletest
	$(RM) -r googletest