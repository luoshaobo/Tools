#######################################################################################
# Copyright (C) 2017 Continental Automotive GmbH
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
all: unittest_user_manager


unittest_user_manager: build_googletest build_unittest_user_manager
	$(MAKE) -C build_unittest_user_manager

build_unittest_user_manager: build_googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./src -Bbuild_unittest_user_manager

build_googletest: googletest
	cmake -DCMAKE_BUILD_TYPE=Debug -H./googletest -Bbuild_googletest
	$(MAKE) -C build_googletest

googletest:
	git clone https://github.com/google/googletest.git

run:
	./unittest_user_manager

clean:
	$(RM) unittest_user_manager
	$(RM) -r build_unittest_user_manager
	$(RM) -r build_googletest
	$(RM) -r googletest
