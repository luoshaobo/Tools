/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "CarUsageOption.h"

static asn_per_constraints_t asn_PER_type_CarUsageOption_constr_1 GCC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  0,  0,  0,  0 }	/* (0..0,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_CarUsageOption_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CarUsageOption, choice.noparam),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"noparam"
		},
};
static asn_TYPE_tag2member_t asn_MAP_CarUsageOption_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* noparam */
};
static asn_CHOICE_specifics_t asn_SPC_CarUsageOption_specs_1 = {
	sizeof(struct CarUsageOption),
	offsetof(struct CarUsageOption, _asn_ctx),
	offsetof(struct CarUsageOption, present),
	sizeof(((struct CarUsageOption *)0)->present),
	asn_MAP_CarUsageOption_tag2el_1,
	1,	/* Count of tags in the map */
	0,
	1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_CarUsageOption = {
	"CarUsageOption",
	"CarUsageOption",
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
	&asn_PER_type_CarUsageOption_constr_1,
	asn_MBR_CarUsageOption_1,
	1,	/* Elements count */
	&asn_SPC_CarUsageOption_specs_1	/* Additional specs */
};

