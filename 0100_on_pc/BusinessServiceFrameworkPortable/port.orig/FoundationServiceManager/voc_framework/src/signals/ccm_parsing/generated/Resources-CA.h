/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-FUNC-002"
 * 	found in "../schemas/asn/discovery/CA-FUNC-002.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_Resources_CA_H_
#define	_Resources_CA_H_


#include <asn_application.h>

/* Including external dependencies */
#include "Address.h"
#include "Topic.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Resources-CA */
typedef struct Resources_CA {
    Address_t	 thisAddress;
	Topic_t	 catalogueUl;
	Topic_t	 catalogueDl;
	Topic_t	 delegateUl;
	Topic_t	 delegateDl;
	Topic_t	 devicePairingUl;
	Topic_t	 devicePairingDl;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */

	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Resources_CA_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Resources_CA;

#ifdef __cplusplus
}
#endif

#endif	/* _Resources_CA_H_ */
#include <asn_internal.h>
