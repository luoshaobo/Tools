/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "FS-StandardTypes"
 * 	found in "../schemas/standard_asn1_types/FS-StandardTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "BleChannelConfig.h"

static asn_TYPE_member_t asn_MBR_blemessages_3[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_ContentType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_blemessages_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_blemessages_specs_3 = {
	sizeof(struct blemessages),
	offsetof(struct blemessages, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_blemessages_3 = {
	"blemessages",
	"blemessages",
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
	asn_DEF_blemessages_tags_3,
	sizeof(asn_DEF_blemessages_tags_3)
		/sizeof(asn_DEF_blemessages_tags_3[0]) - 1, /* 1 */
	asn_DEF_blemessages_tags_3,	/* Same as above */
	sizeof(asn_DEF_blemessages_tags_3)
		/sizeof(asn_DEF_blemessages_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_blemessages_3,
	1,	/* Single element */
	&asn_SPC_blemessages_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_BleChannelConfig_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BleChannelConfig, allowed),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"allowed"
		},
	{ ATF_POINTER, 1, offsetof(struct BleChannelConfig, blemessages),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		0,
		&asn_DEF_blemessages_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"blemessages"
		},
};
static int asn_MAP_BleChannelConfig_oms_1[] = { 1 };
static ber_tlv_tag_t asn_DEF_BleChannelConfig_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BleChannelConfig_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* allowed */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* blemessages */
};
static asn_SEQUENCE_specifics_t asn_SPC_BleChannelConfig_specs_1 = {
	sizeof(struct BleChannelConfig),
	offsetof(struct BleChannelConfig, _asn_ctx),
	asn_MAP_BleChannelConfig_tag2el_1,
	2,	/* Count of tags in the map */
	asn_MAP_BleChannelConfig_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	1,	/* Start extensions */
	3	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BleChannelConfig = {
	"BleChannelConfig",
	"BleChannelConfig",
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
	asn_DEF_BleChannelConfig_tags_1,
	sizeof(asn_DEF_BleChannelConfig_tags_1)
		/sizeof(asn_DEF_BleChannelConfig_tags_1[0]), /* 1 */
	asn_DEF_BleChannelConfig_tags_1,	/* Same as above */
	sizeof(asn_DEF_BleChannelConfig_tags_1)
		/sizeof(asn_DEF_BleChannelConfig_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BleChannelConfig_1,
	2,	/* Elements count */
	&asn_SPC_BleChannelConfig_specs_1	/* Additional specs */
};
