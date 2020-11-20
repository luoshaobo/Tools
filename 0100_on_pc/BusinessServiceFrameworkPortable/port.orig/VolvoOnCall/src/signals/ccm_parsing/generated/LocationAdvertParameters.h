/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_LocationAdvertParameters_H_
#define	_LocationAdvertParameters_H_


#include <asn_application.h>

/* Including external dependencies */
#include "LocationAdvertKind.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LocationAdvertParameters */
typedef struct LocationAdvertParameters {
	struct kind {
		A_SEQUENCE_OF(LocationAdvertKind_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} kind;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} LocationAdvertParameters_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_LocationAdvertParameters;

#ifdef __cplusplus
}
#endif

#endif	/* _LocationAdvertParameters_H_ */
#include <asn_internal.h>
