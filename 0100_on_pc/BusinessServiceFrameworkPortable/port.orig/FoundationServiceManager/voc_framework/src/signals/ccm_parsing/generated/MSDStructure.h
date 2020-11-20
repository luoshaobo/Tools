/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "MSD-ASN1-V2"
 * 	found in "../schemas/asn/en-standards/msd_v2.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_MSDStructure_H_
#define	_MSDStructure_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include "ControlType.h"
#include "VIN.h"
#include "VehiclePropulsionStorageType.h"
#include "VehicleLocation.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct VehicleLocationDelta;

/* MSDStructure */
typedef struct MSDStructure {
	long	 messageIdentifier;
	ControlType_t	 control;
	VIN_t	 vehicleIdentificationNumber;
	VehiclePropulsionStorageType_t	 vehiclePropulsionStorageType;
	unsigned long	 timestamp;
	VehicleLocation_t	 vehicleLocation;
	long	 vehicleDirection;
	struct VehicleLocationDelta	*recentVehicleLocationN1	/* OPTIONAL */;
	struct VehicleLocationDelta	*recentVehicleLocationN2	/* OPTIONAL */;
	long	*numberOfPassengers	/* OPTIONAL */;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MSDStructure_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_timestamp_6;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_MSDStructure;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "VehicleLocationDelta.h"

#endif	/* _MSDStructure_H_ */
#include <asn_internal.h>