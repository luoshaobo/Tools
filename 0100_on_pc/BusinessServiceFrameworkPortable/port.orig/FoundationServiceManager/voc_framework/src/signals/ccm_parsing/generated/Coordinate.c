/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-LocationTypes"
 * 	found in "../schemas/asn/common/BCC-LocationTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "Coordinate.h"

static int
memb_latitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	
	if(1 /* No applicable constraints whatsoever */) {
		/* Nothing is here. See below */
	}
	
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

static int
memb_longitude_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	
	if(1 /* No applicable constraints whatsoever */) {
		/* Nothing is here. See below */
	}
	
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

static asn_per_constraints_t asn_PER_memb_latitude_constr_2 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_longitude_constr_3 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_Coordinate_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Coordinate, latitude),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeReal,
		memb_latitude_constraint_1,
		&asn_PER_memb_latitude_constr_2,
		0,
		"latitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Coordinate, longitude),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeReal,
		memb_longitude_constraint_1,
		&asn_PER_memb_longitude_constr_3,
		0,
		"longitude"
		},
};
static ber_tlv_tag_t asn_DEF_Coordinate_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Coordinate_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* latitude */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* longitude */
};
static asn_SEQUENCE_specifics_t asn_SPC_Coordinate_specs_1 = {
	sizeof(struct Coordinate),
	offsetof(struct Coordinate, _asn_ctx),
	asn_MAP_Coordinate_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	1,	/* Start extensions */
	3	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Coordinate = {
	"Coordinate",
	"Coordinate",
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
	asn_DEF_Coordinate_tags_1,
	sizeof(asn_DEF_Coordinate_tags_1)
		/sizeof(asn_DEF_Coordinate_tags_1[0]), /* 1 */
	asn_DEF_Coordinate_tags_1,	/* Same as above */
	sizeof(asn_DEF_Coordinate_tags_1)
		/sizeof(asn_DEF_Coordinate_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Coordinate_1,
	2,	/* Elements count */
	&asn_SPC_Coordinate_specs_1	/* Additional specs */
};
