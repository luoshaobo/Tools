/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "EntryPoint"
 * 	found in "../schemas/asn/entry_point/EntryPoint.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_Country_H_
#define	_Country_H_


#include <asn_application.h>

/* Including external dependencies */
#include <UTF8String.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Country */
typedef UTF8String_t	 Country_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Country;
asn_struct_free_f Country_free;
asn_struct_print_f Country_print;
asn_constr_check_f Country_constraint;
ber_type_decoder_f Country_decode_ber;
der_type_encoder_f Country_encode_der;
xer_type_decoder_f Country_decode_xer;
xer_type_encoder_f Country_encode_xer;
per_type_decoder_f Country_decode_uper;
per_type_encoder_f Country_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _Country_H_ */
#include <asn_internal.h>
