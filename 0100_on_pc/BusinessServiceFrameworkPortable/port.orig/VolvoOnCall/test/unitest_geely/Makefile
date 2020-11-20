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

all: unitest_geely

clean_marben:
	if file ../../../../marben/ASN-Runtime-lib/C++/Runtime/aip_lib/src/aipbtk.o | grep -qi 'arm'; then \
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make runtime_clean; \
	fi;

build_marben: clean_marben
	cd ../../../../marben/ASN-Runtime-lib/C++/Runtime && make

build_unittest_geely: build_googletest build_marben
	cmake -DCMAKE_BUILD_TYPE=Debug -H./src -Bbuild_unittest_geely

build_unittest_geely/unitest_geely: build_unittest_geely src/main.cc src/unittest_cvc_vds.cc
	$(MAKE) -C build_unittest_geely

unitest_geely: build_unittest_geely/unitest_geely
	cp build_unittest_geely/unitest_geely .

build_googletest: googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./googletest -Bbuild_googletest
	$(MAKE) -C build_googletest

googletest:
	git clone https://github.com/google/googletest.git

clean:
	$(RM) *.bin *.pem *.shared_secret
	$(RM) unitest_geely
	$(RM) -r build_unittest_geely
	$(RM) -r build_googletest
	$(RM) -r googletest

install:
	echo "Not installing unittests"
