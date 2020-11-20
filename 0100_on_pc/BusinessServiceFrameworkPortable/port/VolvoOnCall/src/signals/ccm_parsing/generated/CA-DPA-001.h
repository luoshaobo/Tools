/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-DPA-001"
 * 	found in "../schemas/asn/device_pairing/CA-DPA-001.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_CA_DPA_001_H_
#define	_CA_DPA_001_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DevicePairingId.h"
#include <GeneralizedTime.h>
#include <NULL.h>
#include <constr_CHOICE.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum command_PR {
	command_PR_NOTHING,	/* No components present */
	command_PR_start,
	command_PR_stop,
	/* Extensions may appear below */
	
} command_PR;

/* Forward declarations */
struct VehicleInfo;

/* CA-DPA-001 */
typedef struct CA_DPA_001 {
	DevicePairingId_t	 id;
	struct command {
		command_PR present;
		union CA_DPA_001__command_u {
			NULL_t	 start;
			NULL_t	 stop;
			/*
			 * This type is extensible,
			 * possible extensions are below.
			 */
		} choice;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} command;
	GeneralizedTime_t	*remoteConnectionTimeout	/* OPTIONAL */;
	GeneralizedTime_t	*pairingTimeout	/* OPTIONAL */;
	struct VehicleInfo	*vehicleInfo	/* OPTIONAL */;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CA_DPA_001_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CA_DPA_001;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "VehicleInfo.h"

#endif	/* _CA_DPA_001_H_ */
#include <asn_internal.h>