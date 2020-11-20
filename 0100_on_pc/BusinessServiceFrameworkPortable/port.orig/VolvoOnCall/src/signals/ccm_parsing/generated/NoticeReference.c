/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#include "NoticeReference.h"

static asn_TYPE_member_t asn_MBR_noticeNumbers_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_noticeNumbers_tags_3[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_noticeNumbers_specs_3 = {
	sizeof(struct noticeNumbers),
	offsetof(struct noticeNumbers, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_noticeNumbers_3 = {
	"noticeNumbers",
	"noticeNumbers",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_noticeNumbers_tags_3,
	sizeof(asn_DEF_noticeNumbers_tags_3)
		/sizeof(asn_DEF_noticeNumbers_tags_3[0]), /* 1 */
	asn_DEF_noticeNumbers_tags_3,	/* Same as above */
	sizeof(asn_DEF_noticeNumbers_tags_3)
		/sizeof(asn_DEF_noticeNumbers_tags_3[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_noticeNumbers_3,
	1,	/* Single element */
	&asn_SPC_noticeNumbers_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_NoticeReference_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NoticeReference, organization),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_DisplayText,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"organization"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct NoticeReference, noticeNumbers),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_noticeNumbers_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"noticeNumbers"
		},
};
static ber_tlv_tag_t asn_DEF_NoticeReference_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_NoticeReference_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (12 << 2)), 0, 0, 0 }, /* utf8String */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, 0, 0 }, /* noticeNumbers */
    { (ASN_TAG_CLASS_UNIVERSAL | (22 << 2)), 0, 0, 0 }, /* ia5String */
    { (ASN_TAG_CLASS_UNIVERSAL | (26 << 2)), 0, 0, 0 }, /* visibleString */
    { (ASN_TAG_CLASS_UNIVERSAL | (30 << 2)), 0, 0, 0 } /* bmpString */
};
static asn_SEQUENCE_specifics_t asn_SPC_NoticeReference_specs_1 = {
	sizeof(struct NoticeReference),
	offsetof(struct NoticeReference, _asn_ctx),
	asn_MAP_NoticeReference_tag2el_1,
	5,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_NoticeReference = {
	"NoticeReference",
	"NoticeReference",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_NoticeReference_tags_1,
	sizeof(asn_DEF_NoticeReference_tags_1)
		/sizeof(asn_DEF_NoticeReference_tags_1[0]), /* 1 */
	asn_DEF_NoticeReference_tags_1,	/* Same as above */
	sizeof(asn_DEF_NoticeReference_tags_1)
		/sizeof(asn_DEF_NoticeReference_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_NoticeReference_1,
	2,	/* Elements count */
	&asn_SPC_NoticeReference_specs_1	/* Additional specs */
};

