#*
#* Copyright (C) 2018 Continental Automotive AG and Subsidiary
#*
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*

SUBDIRS = gdbus gpsdata src
SUBDIRS += gdbus/client_proxy gdbus/client_test
SUBDIRS += gdbus/client_proxy
SUBDIRS += libdrsensor
SUBDIRS += gyro
INSTALL = /usr/bin/install
INCLUDES = `find $(PWD)/interface -type f -name *.hpp`

all: make_subdirs

clean: clean_subdirs

# Make subdirectories
make_subdirs:
	@list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Making targets in $$subdir"; \
		(cd $$subdir && $(MAKE) all) || exit $$?; \
	done;


# Clean subdirectories
clean_subdirs:
	@list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Cleaning targets in $$subdir"; \
		(cd $$subdir && $(MAKE) clean) || exit $$?; \
	done;

#install
install:
	$(INSTALL) -m 744 -dv $(buildroot)/usr/include/vpom
	$(INSTALL) -m 755 $(INCLUDES) $(buildroot)/usr/include/vpom
	$(INSTALL) -m 744 -dv $(buildroot)/usr/lib/pkgconfig
	$(INSTALL) -m 755 vpom_client.pc $(buildroot)/usr/lib/pkgconfig
	@list='$(SUBDIRS)'; \
	for subdir in $$list; do \
		echo "Installing Files in $$subdir"; \
		(cd $$subdir && $(MAKE) install) || exit $$?; \
	done;
