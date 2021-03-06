/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "DatasharingStatusConfiguration.h"

static asn_TYPE_member_t asn_MBR_dscoperations_2[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_DatasharingStatusOperation,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_dscoperations_tags_2[] = {
	(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_dscoperations_specs_2 = {
	sizeof(struct dscoperations),
	offsetof(struct dscoperations, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_dscoperations_2 = {
	"dscoperations",
	"dscoperations",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_dscoperations_tags_2,
	sizeof(asn_DEF_dscoperations_tags_2)
		/sizeof(asn_DEF_dscoperations_tags_2[0]) - 1, /* 1 */
	asn_DEF_dscoperations_tags_2,	/* Same as above */
	sizeof(asn_DEF_dscoperations_tags_2)
		/sizeof(asn_DEF_dscoperations_tags_2[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_dscoperations_2,
	1,	/* Single element */
	&asn_SPC_dscoperations_specs_2	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_dcsoptions_4[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_DatasharingStatusOption,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_dcsoptions_tags_4[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_dcsoptions_specs_4 = {
	sizeof(struct dcsoptions),
	offsetof(struct dcsoptions, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_dcsoptions_4 = {
	"dcsoptions",
	"dcsoptions",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_dcsoptions_tags_4,
	sizeof(asn_DEF_dcsoptions_tags_4)
		/sizeof(asn_DEF_dcsoptions_tags_4[0]) - 1, /* 1 */
	asn_DEF_dcsoptions_tags_4,	/* Same as above */
	sizeof(asn_DEF_dcsoptions_tags_4)
		/sizeof(asn_DEF_dcsoptions_tags_4[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_dcsoptions_4,
	1,	/* Single element */
	&asn_SPC_dcsoptions_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_dcsallowedChannelsFromVehicle_6[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_ChannelConfiguration,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_dcsallowedChannelsFromVehicle_tags_6[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_dcsallowedChannelsFromVehicle_specs_6 = {
	sizeof(struct dcsallowedChannelsFromVehicle),
	offsetof(struct dcsallowedChannelsFromVehicle, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_dcsallowedChannelsFromVehicle_6 = {
	"dcsallowedChannelsFromVehicle",
	"dcsallowedChannelsFromVehicle",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_dcsallowedChannelsFromVehicle_tags_6,
	sizeof(asn_DEF_dcsallowedChannelsFromVehicle_tags_6)
		/sizeof(asn_DEF_dcsallowedChannelsFromVehicle_tags_6[0]) - 1, /* 1 */
	asn_DEF_dcsallowedChannelsFromVehicle_tags_6,	/* Same as above */
	sizeof(asn_DEF_dcsallowedChannelsFromVehicle_tags_6)
		/sizeof(asn_DEF_dcsallowedChannelsFromVehicle_tags_6[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_dcsallowedChannelsFromVehicle_6,
	1,	/* Single element */
	&asn_SPC_dcsallowedChannelsFromVehicle_specs_6	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_dcsallowedChannelsToVehicle_8[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_ChannelConfiguration,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_dcsallowedChannelsToVehicle_tags_8[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_dcsallowedChannelsToVehicle_specs_8 = {
	sizeof(struct dcsallowedChannelsToVehicle),
	offsetof(struct dcsallowedChannelsToVehicle, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_dcsallowedChannelsToVehicle_8 = {
	"dcsallowedChannelsToVehicle",
	"dcsallowedChannelsToVehicle",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_dcsallowedChannelsToVehicle_tags_8,
	sizeof(asn_DEF_dcsallowedChannelsToVehicle_tags_8)
		/sizeof(asn_DEF_dcsallowedChannelsToVehicle_tags_8[0]) - 1, /* 1 */
	asn_DEF_dcsallowedChannelsToVehicle_tags_8,	/* Same as above */
	sizeof(asn_DEF_dcsallowedChannelsToVehicle_tags_8)
		/sizeof(asn_DEF_dcsallowedChannelsToVehicle_tags_8[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_dcsallowedChannelsToVehicle_8,
	1,	/* Single element */
	&asn_SPC_dcsallowedChannelsToVehicle_specs_8	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_DatasharingStatusConfiguration_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DatasharingStatusConfiguration, dscoperations),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		0,
		&asn_DEF_dscoperations_2,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"dscoperations"
		},
	{ ATF_POINTER, 1, offsetof(struct DatasharingStatusConfiguration, dcsoptions),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		0,
		&asn_DEF_dcsoptions_4,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"dcsoptions"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DatasharingStatusConfiguration, dcsallowedChannelsFromVehicle),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		0,
		&asn_DEF_dcsallowedChannelsFromVehicle_6,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"dcsallowedChannelsFromVehicle"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DatasharingStatusConfiguration, dcsallowedChannelsToVehicle),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		0,
		&asn_DEF_dcsallowedChannelsToVehicle_8,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"dcsallowedChannelsToVehicle"
		},
};
static int asn_MAP_DatasharingStatusConfiguration_oms_1[] = { 1 };
static ber_tlv_tag_t asn_DEF_DatasharingStatusConfiguration_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_DatasharingStatusConfiguration_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* dscoperations */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* dcsoptions */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* dcsallowedChannelsFromVehicle */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* dcsallowedChannelsToVehicle */
};
static asn_SEQUENCE_specifics_t asn_SPC_DatasharingStatusConfiguration_specs_1 = {
	sizeof(struct DatasharingStatusConfiguration),
	offsetof(struct DatasharingStatusConfiguration, _asn_ctx),
	asn_MAP_DatasharingStatusConfiguration_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_DatasharingStatusConfiguration_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	3,	/* Start extensions */
	5	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DatasharingStatusConfiguration = {
	"DatasharingStatusConfiguration",
	"DatasharingStatusConfiguration",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DatasharingStatusConfiguration_tags_1,
	sizeof(asn_DEF_DatasharingStatusConfiguration_tags_1)
		/sizeof(asn_DEF_DatasharingStatusConfiguration_tags_1[0]), /* 1 */
	asn_DEF_DatasharingStatusConfiguration_tags_1,	/* Same as above */
	sizeof(asn_DEF_DatasharingStatusConfiguration_tags_1)
		/sizeof(asn_DEF_DatasharingStatusConfiguration_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DatasharingStatusConfiguration_1,
	4,	/* Elements count */
	&asn_SPC_DatasharingStatusConfiguration_specs_1	/* Additional specs */
};

