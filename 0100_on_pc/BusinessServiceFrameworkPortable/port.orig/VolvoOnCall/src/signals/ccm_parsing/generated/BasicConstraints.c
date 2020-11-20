/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#include "BasicConstraints.h"

static int
memb_pathLenConstraint_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	/* Check if the sign bit is present */
	value = st->buf ? ((st->buf[0] & 0x80) ? -1 : 1) : 0;
	
	if((value >= 0)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_BasicConstraints_1[] = {
	{ ATF_POINTER, 2, offsetof(struct BasicConstraints, cA),
		(ASN_TAG_CLASS_UNIVERSAL | (1 << 2)),
		0,
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cA"
		},
	{ ATF_POINTER, 1, offsetof(struct BasicConstraints, pathLenConstraint),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		memb_pathLenConstraint_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pathLenConstraint"
		},
};
static ber_tlv_tag_t asn_DEF_BasicConstraints_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BasicConstraints_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (1 << 2)), 0, 0, 0 }, /* cA */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, 0, 0 } /* pathLenConstraint */
};
static asn_SEQUENCE_specifics_t asn_SPC_BasicConstraints_specs_1 = {
	sizeof(struct BasicConstraints),
	offsetof(struct BasicConstraints, _asn_ctx),
	asn_MAP_BasicConstraints_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BasicConstraints = {
	"BasicConstraints",
	"BasicConstraints",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BasicConstraints_tags_1,
	sizeof(asn_DEF_BasicConstraints_tags_1)
		/sizeof(asn_DEF_BasicConstraints_tags_1[0]), /* 1 */
	asn_DEF_BasicConstraints_tags_1,	/* Same as above */
	sizeof(asn_DEF_BasicConstraints_tags_1)
		/sizeof(asn_DEF_BasicConstraints_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BasicConstraints_1,
	2,	/* Elements count */
	&asn_SPC_BasicConstraints_specs_1	/* Additional specs */
};

