/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "FS-StandardTypes"
 * 	found in "../schemas/standard_asn1_types/FS-StandardTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_FSVIN_H_
#define	_FSVIN_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FSVIN */
typedef OCTET_STRING_t	 FSVIN_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_FSVIN;
asn_struct_free_f FSVIN_free;
asn_struct_print_f FSVIN_print;
asn_constr_check_f FSVIN_constraint;
ber_type_decoder_f FSVIN_decode_ber;
der_type_encoder_f FSVIN_encode_der;
xer_type_decoder_f FSVIN_decode_xer;
xer_type_encoder_f FSVIN_encode_xer;
per_type_decoder_f FSVIN_decode_uper;
per_type_encoder_f FSVIN_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _FSVIN_H_ */
#include <asn_internal.h>
