/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CarAccess"
 * 	found in "../schemas/asn/functions/CarAccess.asn"
 * 	`asn1c -gen-PER -S`
 */

#include "TopicInfo.h"

static int
memb_services_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
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
	
	if((size >= 1 && size <= 100)) {
		/* Perform validation of the inner elements */
		return td->check_constraints(td, sptr, ctfailcb, app_key);
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_type_services_constr_5 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 7,  7,  1,  100 }	/* (SIZE(1..100)) */,
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_services_constr_5 GCC_NOTUSED = {
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	{ APC_CONSTRAINED,	 7,  7,  1,  100 }	/* (SIZE(1..100)) */,
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_services_5[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ServiceInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_services_tags_5[] = {
	(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_services_specs_5 = {
	sizeof(struct services),
	offsetof(struct services, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_services_5 = {
	"services",
	"services",
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
	asn_DEF_services_tags_5,
	sizeof(asn_DEF_services_tags_5)
		/sizeof(asn_DEF_services_tags_5[0]) - 1, /* 1 */
	asn_DEF_services_tags_5,	/* Same as above */
	sizeof(asn_DEF_services_tags_5)
		/sizeof(asn_DEF_services_tags_5[0]), /* 2 */
	&asn_PER_type_services_constr_5,
	asn_MBR_services_5,
	1,	/* Single element */
	&asn_SPC_services_specs_5	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_TopicInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct TopicInfo, topic),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Topic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"topic"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TopicInfo, priority),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Priority,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"priority"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TopicInfo, mqttProtocol),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_MqttProtocol,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"mqttProtocol"
		},
	{ ATF_POINTER, 1, offsetof(struct TopicInfo, services),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		0,
		&asn_DEF_services_5,
		memb_services_constraint_1,
		&asn_PER_memb_services_constr_5,
		0,
		"services"
		},
};
static int asn_MAP_TopicInfo_oms_1[] = { 3 };
static ber_tlv_tag_t asn_DEF_TopicInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_TopicInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* topic */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* priority */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* mqttProtocol */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* services */
};
static asn_SEQUENCE_specifics_t asn_SPC_TopicInfo_specs_1 = {
	sizeof(struct TopicInfo),
	offsetof(struct TopicInfo, _asn_ctx),
	asn_MAP_TopicInfo_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_TopicInfo_oms_1,	/* Optional members */
	1, 0,	/* Root/Additions */
	3,	/* Start extensions */
	5	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_TopicInfo = {
	"TopicInfo",
	"TopicInfo",
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
	asn_DEF_TopicInfo_tags_1,
	sizeof(asn_DEF_TopicInfo_tags_1)
		/sizeof(asn_DEF_TopicInfo_tags_1[0]), /* 1 */
	asn_DEF_TopicInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_TopicInfo_tags_1)
		/sizeof(asn_DEF_TopicInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_TopicInfo_1,
	4,	/* Elements count */
	&asn_SPC_TopicInfo_specs_1	/* Additional specs */
};

