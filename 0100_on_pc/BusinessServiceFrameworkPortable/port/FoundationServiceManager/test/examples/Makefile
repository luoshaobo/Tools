########################################################################################################################
# Author:       Iulian Sirghi
# Date:         29-Jan-2018
# Description:  This makefile is used to clean and make FSM-related example applications.
#
########################################################################################################################

SUBDIRS = external_diag_example


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

# Clean
clean: clean_subdirs

