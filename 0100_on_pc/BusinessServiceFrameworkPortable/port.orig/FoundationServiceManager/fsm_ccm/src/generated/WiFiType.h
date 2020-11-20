/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "FS-StandardTypes"
 * 	found in "../schemas/asn/standard_asn1_types/FS-StandardTypes.asn"
 * 	`asn1c -S`
 */

#ifndef	_WiFiType_H_
#define	_WiFiType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum WiFiType {
	WiFiType_any	= 0,
	WiFiType_workshop	= 1,
	WiFiType_vehicleHotspot	= 2
	/*
	 * Enumeration is extensible
	 */
} e_WiFiType;

/* WiFiType */
typedef long	 WiFiType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_WiFiType;
asn_struct_free_f WiFiType_free;
asn_struct_print_f WiFiType_print;
asn_constr_check_f WiFiType_constraint;
ber_type_decoder_f WiFiType_decode_ber;
der_type_encoder_f WiFiType_encode_der;
xer_type_decoder_f WiFiType_decode_xer;
xer_type_encoder_f WiFiType_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _WiFiType_H_ */
#include <asn_internal.h>
