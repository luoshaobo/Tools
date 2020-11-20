/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "FS-StandardTypes"
 * 	found in "../schemas/asn/standard_asn1_types/FS-StandardTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "WiFiChannelConfig.h"

static asn_TYPE_member_t asn_MBR_wifimessages_4[] = {
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
static ber_tlv_tag_t asn_DEF_wifimessages_tags_4[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_wifimessages_specs_4 = {
	sizeof(struct wifimessages),
	offsetof(struct wifimessages, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_wifimessages_4 = {
	"wifimessages",
	"wifimessages",
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
	asn_DEF_wifimessages_tags_4,
	sizeof(asn_DEF_wifimessages_tags_4)
		/sizeof(asn_DEF_wifimessages_tags_4[0]) - 1, /* 1 */
	asn_DEF_wifimessages_tags_4,	/* Same as above */
	sizeof(asn_DEF_wifimessages_tags_4)
		/sizeof(asn_DEF_wifimessages_tags_4[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_wifimessages_4,
	1,	/* Single element */
	&asn_SPC_wifimessages_specs_4	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_WiFiChannelConfig_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct WiFiChannelConfig, allowed),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"allowed"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct WiFiChannelConfig, wifiType),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_WiFiType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"wifiType"
		},
	{ ATF_POINTER, 1, offsetof(struct WiFiChannelConfig, wifimessages),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		0,
		&asn_DEF_wifimessages_4,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"wifimessages"
		},
};
static int asn_MAP_WiFiChannelConfig_oms_1[] = { 2 };
static ber_tlv_tag_t asn_DEF_WiFiChannelConfig_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_WiFiChannelConfig_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* allowed */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* wifiType */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* wifimessages */
};
static asn_SEQUENCE_specifics_t asn_SPC_WiFiChannelConfig_specs_1 = {
	sizeof(struct WiFiChannelConfig),
	offsetof(struct WiFiChannelConfig, _asn_ctx),
	asn_MAP_WiFiChannelConfig_tag2el_1,
	3,	/* Count of tags in the map */
	asn_MAP_WiFiChannelConfig_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	2,	/* Start extensions */
	4	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_WiFiChannelConfig = {
	"WiFiChannelConfig",
	"WiFiChannelConfig",
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
	asn_DEF_WiFiChannelConfig_tags_1,
	sizeof(asn_DEF_WiFiChannelConfig_tags_1)
		/sizeof(asn_DEF_WiFiChannelConfig_tags_1[0]), /* 1 */
	asn_DEF_WiFiChannelConfig_tags_1,	/* Same as above */
	sizeof(asn_DEF_WiFiChannelConfig_tags_1)
		/sizeof(asn_DEF_WiFiChannelConfig_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_WiFiChannelConfig_1,
	3,	/* Elements count */
	&asn_SPC_WiFiChannelConfig_specs_1	/* Additional specs */
};

