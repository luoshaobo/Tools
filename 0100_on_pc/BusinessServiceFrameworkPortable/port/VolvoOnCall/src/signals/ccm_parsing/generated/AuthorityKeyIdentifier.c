/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#include "AuthorityKeyIdentifier.h"

static asn_TYPE_member_t asn_MBR_AuthorityKeyIdentifier_1[] = {
	{ ATF_POINTER, 3, offsetof(struct AuthorityKeyIdentifier, keyIdentifier),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_KeyIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyIdentifier"
		},
	{ ATF_POINTER, 2, offsetof(struct AuthorityKeyIdentifier, authorityCertIssuer),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"authorityCertIssuer"
		},
	{ ATF_POINTER, 1, offsetof(struct AuthorityKeyIdentifier, authorityCertSerialNumber),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_CertificateSerialNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"authorityCertSerialNumber"
		},
};
static ber_tlv_tag_t asn_DEF_AuthorityKeyIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AuthorityKeyIdentifier_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* keyIdentifier */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* authorityCertIssuer */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* authorityCertSerialNumber */
};
static asn_SEQUENCE_specifics_t asn_SPC_AuthorityKeyIdentifier_specs_1 = {
	sizeof(struct AuthorityKeyIdentifier),
	offsetof(struct AuthorityKeyIdentifier, _asn_ctx),
	asn_MAP_AuthorityKeyIdentifier_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AuthorityKeyIdentifier = {
	"AuthorityKeyIdentifier",
	"AuthorityKeyIdentifier",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AuthorityKeyIdentifier_tags_1,
	sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1)
		/sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1[0]), /* 1 */
	asn_DEF_AuthorityKeyIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1)
		/sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AuthorityKeyIdentifier_1,
	3,	/* Elements count */
	&asn_SPC_AuthorityKeyIdentifier_specs_1	/* Additional specs */
};

