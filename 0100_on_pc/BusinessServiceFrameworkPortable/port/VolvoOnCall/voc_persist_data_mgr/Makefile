#################################################################
# Copyright (C) 2018 Continental Automotive GmbH
# Author:       linhaixia
# Date:         01-08-2018
# Description:  This makefile is used to clean and make
#               VolvoOnCall targets in each subdirectory.
#################################################################

SUBDIRS = src

all: make_subdirs

clean: clean_subdirs

# Make subdirectories
make_subdirs:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Making targets in $$subdir"; \
	  (cd $$subdir && $(MAKE) all) || exit $$?;\
	done;


# Clean subdirectories
clean_subdirs:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Cleaning targets in $$subdir"; \
	  (cd $$subdir && $(MAKE) clean) || exit $$?; \
	done;

#install
install:
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  echo "Installing Files in $$subdir"; \
	  (cd $$subdir && $(MAKE) install) || exit $$?; \
	done;
