/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-CAT-002"
 * 	found in "../schemas/asn/catalogue/CA-CAT-002.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "CA-CAT-002.h"

static asn_TYPE_member_t asn_MBR_CA_CAT_002_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CA_CAT_002, responseInfo),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ResponseInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"responseInfo"
		},
};
static ber_tlv_tag_t asn_DEF_CA_CAT_002_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CA_CAT_002_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 } /* responseInfo */
};
static asn_SEQUENCE_specifics_t asn_SPC_CA_CAT_002_specs_1 = {
	sizeof(struct CA_CAT_002),
	offsetof(struct CA_CAT_002, _asn_ctx),
	asn_MAP_CA_CAT_002_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	0,	/* Start extensions */
	2	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CA_CAT_002 = {
	"CA-CAT-002",
	"CA-CAT-002",
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
	asn_DEF_CA_CAT_002_tags_1,
	sizeof(asn_DEF_CA_CAT_002_tags_1)
		/sizeof(asn_DEF_CA_CAT_002_tags_1[0]), /* 1 */
	asn_DEF_CA_CAT_002_tags_1,	/* Same as above */
	sizeof(asn_DEF_CA_CAT_002_tags_1)
		/sizeof(asn_DEF_CA_CAT_002_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_CA_CAT_002_1,
	1,	/* Elements count */
	&asn_SPC_CA_CAT_002_specs_1	/* Additional specs */
};

