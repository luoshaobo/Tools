/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "MSD-ASN1-V2"
 * 	found in "../schemas/asn/en-standards/msd_v2.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_AdditionalData_H_
#define	_AdditionalData_H_


#include <asn_application.h>

/* Including external dependencies */
#include <RELATIVE-OID.h>
#include <OCTET_STRING.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AdditionalData */
typedef struct AdditionalData {
	RELATIVE_OID_t	 oid;
	OCTET_STRING_t	 data;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} AdditionalData_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_AdditionalData;

#ifdef __cplusplus
}
#endif

#endif	/* _AdditionalData_H_ */
#include <asn_internal.h>
