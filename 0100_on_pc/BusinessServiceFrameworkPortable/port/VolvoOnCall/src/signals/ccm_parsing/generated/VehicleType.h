/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "MSD-ASN1-V2"
 * 	found in "../schemas/asn/en-standards/msd_v2.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_VehicleType_H_
#define	_VehicleType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum VehicleType {
	VehicleType_passengerVehicleClassM1	= 1,
	VehicleType_busesAndCoachesClassM2	= 2,
	VehicleType_busesAndCoachesClassM3	= 3,
	VehicleType_lightCommercialVehiclesClassN1	= 4,
	VehicleType_heavyDutyVehiclesClassN2	= 5,
	VehicleType_heavyDutyVehiclesClassN3	= 6,
	VehicleType_motorcyclesClassL1e	= 7,
	VehicleType_motorcyclesClassL2e	= 8,
	VehicleType_motorcyclesClassL3e	= 9,
	VehicleType_motorcyclesClassL4e	= 10,
	VehicleType_motorcyclesClassL5e	= 11,
	VehicleType_motorcyclesClassL6e	= 12,
	VehicleType_motorcyclesClassL7e	= 13
	/*
	 * Enumeration is extensible
	 */
} e_VehicleType;

/* VehicleType */
typedef long	 VehicleType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_VehicleType;
asn_struct_free_f VehicleType_free;
asn_struct_print_f VehicleType_print;
asn_constr_check_f VehicleType_constraint;
ber_type_decoder_f VehicleType_decode_ber;
der_type_encoder_f VehicleType_encode_der;
xer_type_decoder_f VehicleType_decode_xer;
xer_type_encoder_f VehicleType_encode_xer;
per_type_decoder_f VehicleType_decode_uper;
per_type_encoder_f VehicleType_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _VehicleType_H_ */
#include <asn_internal.h>
