#################################################################
# Author:       Iulian Sirghi
# Date:         29-Aug-2017
# Description:  This makefile is used to clean and make
#               fs_config d-bus introspection generated code
#################################################################

INTROSPECTION_BASE = com.contiautomotive.tcam.FoundationServices

HEADER_DIR=../protected/fscfg_common/generated
SOURCE_DIR=../../src/fscfg_common/generated


XMLS = $(INTROSPECTION_BASE).Config.Resource.xml \
       $(INTROSPECTION_BASE).Config.Resource.xml \
       $(INTROSPECTION_BASE).Config.ProvisionedResource.xml \
       $(INTROSPECTION_BASE).Config.Feature.xml \
       $(INTROSPECTION_BASE).Config.Encoding.xml \
       $(INTROSPECTION_BASE).Config.Source.xml \
       $(INTROSPECTION_BASE).Config.xml \
       $(INTROSPECTION_BASE).Discovery.xml \
       $(INTROSPECTION_BASE).Provisioning.xml



BASES_XML = $(XMLS:$(INTROSPECTION_BASE).%.xml=%.xml)
GEN_BASE = foundation-services-config-generated

GEN_HEADERS = $(GEN_BASE).h
GEN_SOURCES = $(GEN_BASE).c

DEST_HEADERS = $(GEN_HEADERS:%=$(HEADER_DIR)/%)
DEST_SOURCES = $(GEN_SOURCES:%=$(SOURCE_DIR)/%)

all: $(GEN_HEADERS) $(GEN_SOURCES) $(DEST_HEADERS) $(DEST_SOURCES)

$(GEN_SOURCES): $(GEN_HEADERS)

$(GEN_HEADERS): $(XMLS)
	gdbus-codegen --interface-prefix $(INTROSPECTION_BASE) \
	--c-generate-object-manager \
	--generate-c-code foundation-services-config-generated \
	--xml-files $(XMLS)


$(DEST_HEADERS): $(GEN_HEADERS)
	mkdir -p $(HEADER_DIR)
	cp $(patsubst $(HEADER_DIR)/%,%,$@) $@

$(DEST_SOURCES): $(GEN_SOURCES)
	mkdir -p $(SOURCE_DIR)
	cp $(patsubst $(SOURCE_DIR)/%,%,$@) $@

GENERATED_CODE += $(GEN_HEADERS) $(GEN_SOURCES)

clean:
	$(RM) $(DEST_HEADERS)
	$(RM) $(DEST_SOURCES)
	$(RM) $(GENERATED_CODE)
	$(RM) -r $(HEADER_DIR)
	$(RM) -r $(SOURCE_DIR)

install: ;
