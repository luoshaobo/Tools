/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#ifndef	_CRLDistributionPoints_H_
#define	_CRLDistributionPoints_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct DistributionPoint;

/* CRLDistributionPoints */
typedef struct CRLDistributionPoints {
	A_SEQUENCE_OF(struct DistributionPoint) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CRLDistributionPoints_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CRLDistributionPoints;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "DistributionPoint.h"

#endif	/* _CRLDistributionPoints_H_ */
#include <asn_internal.h>
