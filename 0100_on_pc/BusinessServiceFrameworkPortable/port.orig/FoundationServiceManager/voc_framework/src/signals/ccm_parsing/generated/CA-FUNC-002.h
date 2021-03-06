/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-FUNC-002"
 * 	found in "../schemas/asn/discovery/CA-FUNC-002.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_CA_FUNC_002_H_
#define	_CA_FUNC_002_H_


#include <asn_application.h>

/* Including external dependencies */
#include "Resources-CA.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct TopicInfo;

/* CA-FUNC-002 */
typedef struct CA_FUNC_002 {
	Resources_CA_t	 resources;
	struct uplinkTopics {
		A_SEQUENCE_OF(struct TopicInfo) list;

		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} uplinkTopics;
	struct downlinkTopics {
		A_SEQUENCE_OF(struct TopicInfo) list;

		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} downlinkTopics;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */

	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CA_FUNC_002_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CA_FUNC_002;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "TopicInfo.h"

#endif	/* _CA_FUNC_002_H_ */
#include <asn_internal.h>
