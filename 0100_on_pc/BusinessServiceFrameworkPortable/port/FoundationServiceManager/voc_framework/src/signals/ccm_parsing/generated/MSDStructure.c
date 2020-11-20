/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "MSD-ASN1-V2"
 * 	found in "../schemas/asn/en-standards/msd_v2.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "MSDStructure.h"

static int
timestamp_6_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	
	/* Constraint check succeeded */
	return 0;
}

/*
 * This type is implemented using NativeInteger,
 * so here we adjust the DEF accordingly.
 */
static void
timestamp_6_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeInteger.free_struct;
	td->print_struct   = asn_DEF_NativeInteger.print_struct;
	td->check_constraints = asn_DEF_NativeInteger.check_constraints;
	td->ber_decoder    = asn_DEF_NativeInteger.ber_decoder;
	td->der_encoder    = asn_DEF_NativeInteger.der_encoder;
	td->xer_decoder    = asn_DEF_NativeInteger.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeInteger.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeInteger.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeInteger.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeInteger.per_constraints;
	td->elements       = asn_DEF_NativeInteger.elements;
	td->elements_count = asn_DEF_NativeInteger.elements_count;
     /* td->specifics      = asn_DEF_NativeInteger.specifics;	// Defined explicitly */
}

static void
timestamp_6_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	timestamp_6_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

static int
timestamp_6_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

static asn_dec_rval_t
timestamp_6_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

static asn_enc_rval_t
timestamp_6_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

static asn_dec_rval_t
timestamp_6_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

static asn_enc_rval_t
timestamp_6_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static asn_dec_rval_t
timestamp_6_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

static asn_enc_rval_t
timestamp_6_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	timestamp_6_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static int
memb_messageIdentifier_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_timestamp_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	
	/* Constraint check succeeded */
	return 0;
}

static int
memb_vehicleDirection_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_numberOfPassengers_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 255)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_type_timestamp_constr_6 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 32, -1,  0,  4294967295 }	/* (0..4294967295) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_messageIdentifier_constr_2 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 8,  8,  0,  255 }	/* (0..255) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_timestamp_constr_6 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 32, -1,  0,  4294967295 }	/* (0..4294967295) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_vehicleDirection_constr_8 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 8,  8,  0,  255 }	/* (0..255) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_numberOfPassengers_constr_11 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 8,  8,  0,  255 }	/* (0..255) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_INTEGER_specifics_t asn_SPC_timestamp_specs_6 = {
	0,	0,	0,	0,	0,
	0,	/* Native long size */
	1	/* Unsigned representation */
};
static ber_tlv_tag_t asn_DEF_timestamp_tags_6[] = {
	(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_timestamp_6 = {
	"timestamp",
	"timestamp",
	timestamp_6_free,
	timestamp_6_print,
	timestamp_6_constraint,
	timestamp_6_decode_ber,
	timestamp_6_encode_der,
	timestamp_6_decode_xer,
	timestamp_6_encode_xer,
	timestamp_6_decode_uper,
	timestamp_6_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_timestamp_tags_6,
	sizeof(asn_DEF_timestamp_tags_6)
		/sizeof(asn_DEF_timestamp_tags_6[0]) - 1, /* 1 */
	asn_DEF_timestamp_tags_6,	/* Same as above */
	sizeof(asn_DEF_timestamp_tags_6)
		/sizeof(asn_DEF_timestamp_tags_6[0]), /* 2 */
	&asn_PER_type_timestamp_constr_6,
	0, 0,	/* No members */
	&asn_SPC_timestamp_specs_6	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_MSDStructure_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, messageIdentifier),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_messageIdentifier_constraint_1,
		&asn_PER_memb_messageIdentifier_constr_2,
		0,
		"messageIdentifier"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, control),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ControlType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"control"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, vehicleIdentificationNumber),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VIN,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"vehicleIdentificationNumber"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, vehiclePropulsionStorageType),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehiclePropulsionStorageType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"vehiclePropulsionStorageType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, timestamp),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_timestamp_6,
		memb_timestamp_constraint_1,
		&asn_PER_memb_timestamp_constr_6,
		0,
		"timestamp"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, vehicleLocation),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehicleLocation,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"vehicleLocation"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MSDStructure, vehicleDirection),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_vehicleDirection_constraint_1,
		&asn_PER_memb_vehicleDirection_constr_8,
		0,
		"vehicleDirection"
		},
	{ ATF_POINTER, 3, offsetof(struct MSDStructure, recentVehicleLocationN1),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehicleLocationDelta,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"recentVehicleLocationN1"
		},
	{ ATF_POINTER, 2, offsetof(struct MSDStructure, recentVehicleLocationN2),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_VehicleLocationDelta,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"recentVehicleLocationN2"
		},
	{ ATF_POINTER, 1, offsetof(struct MSDStructure, numberOfPassengers),
		(ASN_TAG_CLASS_CONTEXT | (9 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_numberOfPassengers_constraint_1,
		&asn_PER_memb_numberOfPassengers_constr_11,
		0,
		"numberOfPassengers"
		},
};
static int asn_MAP_MSDStructure_oms_1[] = { 7, 8, 9 };
static ber_tlv_tag_t asn_DEF_MSDStructure_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_MSDStructure_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* messageIdentifier */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* control */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* vehicleIdentificationNumber */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* vehiclePropulsionStorageType */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* timestamp */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* vehicleLocation */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* vehicleDirection */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* recentVehicleLocationN1 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 }, /* recentVehicleLocationN2 */
    { (ASN_TAG_CLASS_CONTEXT | (9 << 2)), 9, 0, 0 } /* numberOfPassengers */
};
static asn_SEQUENCE_specifics_t asn_SPC_MSDStructure_specs_1 = {
	sizeof(struct MSDStructure),
	offsetof(struct MSDStructure, _asn_ctx),
	asn_MAP_MSDStructure_tag2el_1,
	10,	/* Count of tags in the map */
	asn_MAP_MSDStructure_oms_1,	/* Optional members */
	3, 0,	/* Root/Additions */
	9,	/* Start extensions */
	11	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_MSDStructure = {
	"MSDStructure",
	"MSDStructure",
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
	asn_DEF_MSDStructure_tags_1,
	sizeof(asn_DEF_MSDStructure_tags_1)
		/sizeof(asn_DEF_MSDStructure_tags_1[0]), /* 1 */
	asn_DEF_MSDStructure_tags_1,	/* Same as above */
	sizeof(asn_DEF_MSDStructure_tags_1)
		/sizeof(asn_DEF_MSDStructure_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_MSDStructure_1,
	10,	/* Elements count */
	&asn_SPC_MSDStructure_specs_1	/* Additional specs */
};
