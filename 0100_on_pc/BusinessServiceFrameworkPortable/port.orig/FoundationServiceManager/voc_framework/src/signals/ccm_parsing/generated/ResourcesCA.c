/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CarAccess"
 * 	found in "../schemas/functions/CarAccess.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "ResourcesCA.h"

static asn_TYPE_member_t asn_MBR_ResourcesCA_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, thiz),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Address,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"thiz"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, catalogueUl),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"catalogueUl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, catalogueDl),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"catalogueDl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, delegateUl),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"delegateUl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, delegateDl),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"delegateDl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, devicePairingUl),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"devicePairingUl"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourcesCA, devicePairingDl),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"devicePairingDl"
		},
};
static ber_tlv_tag_t asn_DEF_ResourcesCA_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ResourcesCA_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* thiz */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* catalogueUl */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* catalogueDl */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* delegateUl */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* delegateDl */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* devicePairingUl */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* devicePairingDl */
};
static asn_SEQUENCE_specifics_t asn_SPC_ResourcesCA_specs_1 = {
	sizeof(struct ResourcesCA),
	offsetof(struct ResourcesCA, _asn_ctx),
	asn_MAP_ResourcesCA_tag2el_1,
	7,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	6,	/* Start extensions */
	8	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ResourcesCA = {
	"ResourcesCA",
	"ResourcesCA",
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
	asn_DEF_ResourcesCA_tags_1,
	sizeof(asn_DEF_ResourcesCA_tags_1)
		/sizeof(asn_DEF_ResourcesCA_tags_1[0]), /* 1 */
	asn_DEF_ResourcesCA_tags_1,	/* Same as above */
	sizeof(asn_DEF_ResourcesCA_tags_1)
		/sizeof(asn_DEF_ResourcesCA_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ResourcesCA_1,
	7,	/* Elements count */
	&asn_SPC_ResourcesCA_specs_1	/* Additional specs */
};

