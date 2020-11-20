/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "MSD-ASN1-V2"
 * 	found in "../schemas/asn/en-standards/msd_v2.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "ControlType.h"

static asn_TYPE_member_t asn_MBR_ControlType_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ControlType, automaticActivation),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"automaticActivation"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ControlType, testCall),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"testCall"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ControlType, positionCanBeTrusted),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"positionCanBeTrusted"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ControlType, vehicleType),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehicleType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"vehicleType"
		},
};
static ber_tlv_tag_t asn_DEF_ControlType_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ControlType_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* automaticActivation */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* testCall */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* positionCanBeTrusted */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* vehicleType */
};
static asn_SEQUENCE_specifics_t asn_SPC_ControlType_specs_1 = {
	sizeof(struct ControlType),
	offsetof(struct ControlType, _asn_ctx),
	asn_MAP_ControlType_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ControlType = {
	"ControlType",
	"ControlType",
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
	asn_DEF_ControlType_tags_1,
	sizeof(asn_DEF_ControlType_tags_1)
		/sizeof(asn_DEF_ControlType_tags_1[0]), /* 1 */
	asn_DEF_ControlType_tags_1,	/* Same as above */
	sizeof(asn_DEF_ControlType_tags_1)
		/sizeof(asn_DEF_ControlType_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ControlType_1,
	4,	/* Elements count */
	&asn_SPC_ControlType_specs_1	/* Additional specs */
};

