/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-DEL-001"
 * 	found in "../schemas/asn/delegate/CA-DEL-001.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "CA-DEL-001.h"

static ber_tlv_tag_t asn_DEF_CA_DEL_001_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SEQUENCE_specifics_t asn_SPC_CA_DEL_001_specs_1 = {
	sizeof(struct CA_DEL_001),
	offsetof(struct CA_DEL_001, _asn_ctx),
	0,	/* No top level tags */
	0,	/* No tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_CA_DEL_001 = {
	"CA-DEL-001",
	"CA-DEL-001",
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
	asn_DEF_CA_DEL_001_tags_1,
	sizeof(asn_DEF_CA_DEL_001_tags_1)
		/sizeof(asn_DEF_CA_DEL_001_tags_1[0]), /* 1 */
	asn_DEF_CA_DEL_001_tags_1,	/* Same as above */
	sizeof(asn_DEF_CA_DEL_001_tags_1)
		/sizeof(asn_DEF_CA_DEL_001_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	&asn_SPC_CA_DEL_001_specs_1	/* Additional specs */
};

