#################################################################
# Copyright (C) 2018 Continental Automotive GmbH
#
# Licensed under the "Supplier unrestricted white box" category, as
# described in the 20160322-1_SW-amendment TCAM.pdf agreement
# between Volvo Cars and Continental Automotive GmbH.
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#################################################################


SUBDIRS = voc_persist_data_mgr/src voc_persist_data_mgr src
##ifeq ($(ENABLE_SIGNAL_INJECTION),true)
    SUBDIRS += src/voc_dbus_client
##endif

all: make_subdirs

clean: clean_subdirs

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
	  (cd $$subdir && $(MAKE) clean) || exit 1;  \
	done;

#Clean
clean: clean_subdirs
	$(RM) -r ./docs

#install
install:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Installing Files in $$subdir"; \
	  (cd $$subdir && $(MAKE) install) || exit 1;  \
	done;

#Create Doxygen documentation:
docs:
	doxygen voc_doxygen
