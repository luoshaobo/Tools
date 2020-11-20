/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-CL-001"
 * 	found in "../schemas/asn/car_locator/BCC-CL-001.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_Adverts_H_
#define	_Adverts_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Adverts {
	Adverts_honk	= 0,
	Adverts_flash	= 1,
	Adverts_honkAndFlash	= 2
	/*
	 * Enumeration is extensible
	 */
} e_Adverts;

/* Adverts */
typedef long	 Adverts_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Adverts;
asn_struct_free_f Adverts_free;
asn_struct_print_f Adverts_print;
asn_constr_check_f Adverts_constraint;
ber_type_decoder_f Adverts_decode_ber;
der_type_encoder_f Adverts_encode_der;
xer_type_decoder_f Adverts_decode_xer;
xer_type_encoder_f Adverts_encode_xer;
per_type_decoder_f Adverts_decode_uper;
per_type_encoder_f Adverts_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _Adverts_H_ */
#include <asn_internal.h>
