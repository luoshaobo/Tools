/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-CL-020"
 * 	found in "../schemas/asn/car_locator/BCC-CL-020.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_BCC_CL_020_H_
#define	_BCC_CL_020_H_


#include <asn_application.h>

/* Including external dependencies */
#include "Location.h"
#include "MinimumSetOfLocationData.h"
#include <constr_CHOICE.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

// Working around asn1c problems,
// code does not compile if includeing both bcc-cl-011
// and bcc-cl-020 in one place.
#ifndef LOCATION_PR_DEFINED
#define LOCATION_PR_DEFINED
/* Dependencies */
typedef enum location_PR {
	location_PR_NOTHING,	/* No components present */
	location_PR_extendedLocation,
	location_PR_minimumLocation,
	/* Extensions may appear below */
	
} location_PR;
#endif

/* BCC-CL-020 */
typedef struct BCC_CL_020 {
	struct location {
		location_PR present;
		union BCC_CL_020__location_u {
			Location_t	 extendedLocation;
			MinimumSetOfLocationData_t	 minimumLocation;
			/*
			 * This type is extensible,
			 * possible extensions are below.
			 */
		} choice;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} location;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BCC_CL_020_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BCC_CL_020;

#ifdef __cplusplus
}
#endif

#endif	/* _BCC_CL_020_H_ */
#include <asn_internal.h>