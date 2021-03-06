/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-CL-011"
 * 	found in "../schemas/asn/car_locator/BCC-CL-011.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "BCC-CL-011.h"

static asn_per_constraints_t asn_PER_type_location_constr_3 GCC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  1,  1,  0,  1 }	/* (0..1,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_location_3[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct location, choice.extendedLocation),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Location,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"extendedLocation"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct location, choice.minimumLocation),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_MinimumSetOfLocationData,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"minimumLocation"
		},
};
static asn_TYPE_tag2member_t asn_MAP_location_tag2el_3[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* extendedLocation */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* minimumLocation */
};
static asn_CHOICE_specifics_t asn_SPC_location_specs_3 = {
	sizeof(struct location),
	offsetof(struct location, _asn_ctx),
	offsetof(struct location, present),
	sizeof(((struct location *)0)->present),
	asn_MAP_location_tag2el_3,
	2,	/* Count of tags in the map */
	0,
	2	/* Extensions start */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_location_3 = {
	"location",
	"location",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	CHOICE_decode_uper,
	CHOICE_encode_uper,
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_type_location_constr_3,
	asn_MBR_location_3,
	2,	/* Elements count */
	&asn_SPC_location_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_BCC_CL_011_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BCC_CL_011, responseInfo),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ResponseInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"responseInfo"
		},
	{ ATF_POINTER, 1, offsetof(struct BCC_CL_011, location),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_location_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"location"
		},
};
static int asn_MAP_BCC_CL_011_oms_1[] = { 1 };
static ber_tlv_tag_t asn_DEF_BCC_CL_011_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BCC_CL_011_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* responseInfo */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* location */
};
static asn_SEQUENCE_specifics_t asn_SPC_BCC_CL_011_specs_1 = {
	sizeof(struct BCC_CL_011),
	offsetof(struct BCC_CL_011, _asn_ctx),
	asn_MAP_BCC_CL_011_tag2el_1,
	2,	/* Count of tags in the map */
	asn_MAP_BCC_CL_011_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	1,	/* Start extensions */
	3	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BCC_CL_011 = {
	"BCC-CL-011",
	"BCC-CL-011",
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
	asn_DEF_BCC_CL_011_tags_1,
	sizeof(asn_DEF_BCC_CL_011_tags_1)
		/sizeof(asn_DEF_BCC_CL_011_tags_1[0]), /* 1 */
	asn_DEF_BCC_CL_011_tags_1,	/* Same as above */
	sizeof(asn_DEF_BCC_CL_011_tags_1)
		/sizeof(asn_DEF_BCC_CL_011_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BCC_CL_011_1,
	2,	/* Elements count */
	&asn_SPC_BCC_CL_011_specs_1	/* Additional specs */
};

