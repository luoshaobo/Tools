/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#ifndef	_PolicyConstraints_H_
#define	_PolicyConstraints_H_


#include <asn_application.h>

/* Including external dependencies */
#include "SkipCerts.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PolicyConstraints */
typedef struct PolicyConstraints {
	SkipCerts_t	*requireExplicitPolicy	/* OPTIONAL */;
	SkipCerts_t	*inhibitPolicyMapping	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PolicyConstraints_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PolicyConstraints;

#ifdef __cplusplus
}
#endif

#endif	/* _PolicyConstraints_H_ */
#include <asn_internal.h>
