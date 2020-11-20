/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "Features"
 * 	found in "../schemas/asn/features/Features.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "Feature.h"

static int
memb_tags_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	size_t size;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Determine the number of elements */
	size = _A_CSEQUENCE_FROM_VOID(sptr)->count;
	
	if((size >= 1 && size <= 5000)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_type_tags_constr_8 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 13,  13,  1,  5000 }	/* (SIZE(1..5000)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_tags_constr_8 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 13,  13,  1,  5000 }	/* (SIZE(1..5000)) */,
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_tags_8[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),
		0,
		&asn_DEF_Tag,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_tags_tags_8[] = {
	(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_tags_specs_8 = {
	sizeof(struct tags),
	offsetof(struct tags, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_tags_8 = {
	"tags",
	"tags",
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
	asn_DEF_tags_tags_8,
	sizeof(asn_DEF_tags_tags_8)
		/sizeof(asn_DEF_tags_tags_8[0]) - 1, /* 1 */
	asn_DEF_tags_tags_8,	/* Same as above */
	sizeof(asn_DEF_tags_tags_8)
		/sizeof(asn_DEF_tags_tags_8[0]), /* 2 */
	&asn_PER_type_tags_constr_8,
	asn_MBR_tags_8,
	1,	/* Single element */
	&asn_SPC_tags_specs_8	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_Feature_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Feature, name),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UTF8String,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"name"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Feature, enabled),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"enabled"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Feature, visible),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"visible"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Feature, uri),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Uri,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"uri"
		},
	{ ATF_POINTER, 3, offsetof(struct Feature, icon),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Uri,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"icon"
		},
	{ ATF_POINTER, 2, offsetof(struct Feature, description),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UTF8String,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"description"
		},
	{ ATF_POINTER, 1, offsetof(struct Feature, tags),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		0,
		&asn_DEF_tags_8,
		memb_tags_constraint_1,
		&asn_PER_memb_tags_constr_8,
		0,
		"tags"
		},
};
static int asn_MAP_Feature_oms_1[] = { 4, 5, 6 };
static ber_tlv_tag_t asn_DEF_Feature_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Feature_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* name */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* enabled */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* visible */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* uri */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* icon */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* description */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* tags */
};
static asn_SEQUENCE_specifics_t asn_SPC_Feature_specs_1 = {
	sizeof(struct Feature),
	offsetof(struct Feature, _asn_ctx),
	asn_MAP_Feature_tag2el_1,
	7,	/* Count of tags in the map */
	asn_MAP_Feature_oms_1,	/* Optional members */
	3, 0,	/* Root/Additions */
	6,	/* Start extensions */
	8	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Feature = {
	"Feature",
	"Feature",
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
	asn_DEF_Feature_tags_1,
	sizeof(asn_DEF_Feature_tags_1)
		/sizeof(asn_DEF_Feature_tags_1[0]), /* 1 */
	asn_DEF_Feature_tags_1,	/* Same as above */
	sizeof(asn_DEF_Feature_tags_1)
		/sizeof(asn_DEF_Feature_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Feature_1,
	7,	/* Elements count */
	&asn_SPC_Feature_specs_1	/* Additional specs */
};

