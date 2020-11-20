/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_BasicCarControl_H_
#define	_BasicCarControl_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Resources;
struct Functions;

/* BasicCarControl */
typedef struct BasicCarControl {
	struct Resources	*resources	/* OPTIONAL */;
	struct functions {
		A_SEQUENCE_OF(struct Functions) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *functions;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BasicCarControl_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BasicCarControl;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "Resources.h"
#include "Functions.h"

#endif	/* _BasicCarControl_H_ */
#include <asn_internal.h>
