/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "FS-StandardTypes"
 * 	found in "../schemas/standard_asn1_types/FS-StandardTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_ResponseInfo_H_
#define	_ResponseInfo_H_


#include <asn_application.h>

/* Including external dependencies */
#include "SuccessInfo.h"
#include "ErrorInfo.h"
#include "AcknowledgeInfo.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ResponseInfo_PR {
	ResponseInfo_PR_NOTHING,	/* No components present */
	ResponseInfo_PR_success,
	ResponseInfo_PR_error,
	ResponseInfo_PR_acknowledge,
	/* Extensions may appear below */
	
} ResponseInfo_PR;

/* ResponseInfo */
typedef struct ResponseInfo {
	ResponseInfo_PR present;
	union ResponseInfo_u {
		SuccessInfo_t	 success;
		ErrorInfo_t	 error;
		AcknowledgeInfo_t	 acknowledge;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ResponseInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ResponseInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _ResponseInfo_H_ */
#include <asn_internal.h>