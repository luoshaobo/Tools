/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "EntryPoint"
 * 	found in "../schemas/asn/entry_point/EntryPoint.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "EntryPoint.h"

static asn_TYPE_member_t asn_MBR_validLocations_9[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),
		0,
		&asn_DEF_Country,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_validLocations_tags_9[] = {
	(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_validLocations_specs_9 = {
	sizeof(struct validLocations),
	offsetof(struct validLocations, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_validLocations_9 = {
	"validLocations",
	"validLocations",
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
	asn_DEF_validLocations_tags_9,
	sizeof(asn_DEF_validLocations_tags_9)
		/sizeof(asn_DEF_validLocations_tags_9[0]) - 1, /* 1 */
	asn_DEF_validLocations_tags_9,	/* Same as above */
	sizeof(asn_DEF_validLocations_tags_9)
		/sizeof(asn_DEF_validLocations_tags_9[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_validLocations_9,
	1,	/* Single element */
	&asn_SPC_validLocations_specs_9	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_EntryPoint_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, thiz),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Uri,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"thiz"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, specificationVersion),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"specificationVersion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, implementationVersion),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"implementationVersion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, clientUri),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Uri,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"clientUri"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, host),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UTF8String,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"host"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EntryPoint, port),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"port"
		},
	{ ATF_POINTER, 2, offsetof(struct EntryPoint, signalServiceUri),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Uri,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"signalServiceUri"
		},
	{ ATF_POINTER, 1, offsetof(struct EntryPoint, validLocations),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		0,
		&asn_DEF_validLocations_9,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"validLocations"
		},
};
static int asn_MAP_EntryPoint_oms_1[] = { 6, 7 };
static ber_tlv_tag_t asn_DEF_EntryPoint_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_EntryPoint_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* thiz */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* specificationVersion */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* implementationVersion */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* clientUri */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* host */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* port */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* signalServiceUri */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 } /* validLocations */
};
static asn_SEQUENCE_specifics_t asn_SPC_EntryPoint_specs_1 = {
	sizeof(struct EntryPoint),
	offsetof(struct EntryPoint, _asn_ctx),
	asn_MAP_EntryPoint_tag2el_1,
	8,	/* Count of tags in the map */
	asn_MAP_EntryPoint_oms_1,	/* Optional members */
	2, 0,	/* Root/Additions */
	7,	/* Start extensions */
	9	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_EntryPoint = {
	"EntryPoint",
	"EntryPoint",
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
	asn_DEF_EntryPoint_tags_1,
	sizeof(asn_DEF_EntryPoint_tags_1)
		/sizeof(asn_DEF_EntryPoint_tags_1[0]), /* 1 */
	asn_DEF_EntryPoint_tags_1,	/* Same as above */
	sizeof(asn_DEF_EntryPoint_tags_1)
		/sizeof(asn_DEF_EntryPoint_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_EntryPoint_1,
	8,	/* Elements count */
	&asn_SPC_EntryPoint_specs_1	/* Additional specs */
};

