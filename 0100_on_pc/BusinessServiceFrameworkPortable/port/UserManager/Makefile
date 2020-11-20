#################################################################
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
# Copyright (C) 2017 Continental Automotive GmbH
# Author:       Florian Schindler
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               Usermanager modules targets in each subdirectory.
#################################################################

SUBDIRS =  src \
           usermanager_tool

all: make_subdirs

# Make subdirectories
make_subdirs:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Making targets in $$subdir"; \
	  (cd $$subdir && $(MAKE) all) || exit 1;  \
	done;



# Clean subdirectories
clean_subdirs:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Cleaning targets in $$subdir"; \
	  (cd $$subdir && $(MAKE) clean) || exit 1;\
	done;

#Clean
clean: clean_subdirs
	$(RM) -r ./docs

#install
install:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Installing Files in $$subdir"; \
	  (cd $$subdir && $(MAKE) install) || exit 1;\
	done;

# create Doxygen documentation:
docs:
	doxygen fsmum_doxygen

# creates, cleans and runs unittest (needs additional packages installed locally):
# precondition is a fully built vcc-tcam
unittest:
	cd test/unittest; \
	$(MAKE) clean; \
	$(MAKE)

clear_unittest:
	cd test/unittest; \
	$(MAKE) clean

run_unittest:
	$(MAKE) unittest; \
	cd test/unittest; \
	$(MAKE) run
