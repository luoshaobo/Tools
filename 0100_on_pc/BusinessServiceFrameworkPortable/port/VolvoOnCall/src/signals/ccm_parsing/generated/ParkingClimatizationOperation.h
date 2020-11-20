/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_ParkingClimatizationOperation_H_
#define	_ParkingClimatizationOperation_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NULL.h>
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ParkingClimatizationOperation_PR {
	ParkingClimatizationOperation_PR_NOTHING,	/* No components present */
	ParkingClimatizationOperation_PR_start,
	ParkingClimatizationOperation_PR_stop,
	ParkingClimatizationOperation_PR_setCalendarV1,
	ParkingClimatizationOperation_PR_setTimers,
	ParkingClimatizationOperation_PR_getStatus,
	/* Extensions may appear below */
	
} ParkingClimatizationOperation_PR;

/* ParkingClimatizationOperation */
typedef struct ParkingClimatizationOperation {
	ParkingClimatizationOperation_PR present;
	union ParkingClimatizationOperation_u {
		NULL_t	 start;
		NULL_t	 stop;
		NULL_t	 setCalendarV1;
		NULL_t	 setTimers;
		NULL_t	 getStatus;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ParkingClimatizationOperation_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ParkingClimatizationOperation;

#ifdef __cplusplus
}
#endif

#endif	/* _ParkingClimatizationOperation_H_ */
#include <asn_internal.h>
