/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-LocationTypes"
 * 	found in "../schemas/asn/common/BCC-LocationTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_SatelliteSystem_H_
#define	_SatelliteSystem_H_


#include <asn_application.h>

/* Including external dependencies */
#include <BIT_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SatelliteSystem {
	SatelliteSystem_gps	= 0,
	SatelliteSystem_glonass	= 1,
	SatelliteSystem_galileo	= 2,
	SatelliteSystem_beidou	= 3
} e_SatelliteSystem;

/* SatelliteSystem */
typedef BIT_STRING_t	 SatelliteSystem_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SatelliteSystem;
asn_struct_free_f SatelliteSystem_free;
asn_struct_print_f SatelliteSystem_print;
asn_constr_check_f SatelliteSystem_constraint;
ber_type_decoder_f SatelliteSystem_decode_ber;
der_type_encoder_f SatelliteSystem_encode_der;
xer_type_decoder_f SatelliteSystem_decode_xer;
xer_type_encoder_f SatelliteSystem_encode_xer;
per_type_decoder_f SatelliteSystem_decode_uper;
per_type_encoder_f SatelliteSystem_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _SatelliteSystem_H_ */
#include <asn_internal.h>
