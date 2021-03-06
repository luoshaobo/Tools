/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "Features"
 * 	found in "../schemas/asn/features/Features.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_Tag_H_
#define	_Tag_H_


#include <asn_application.h>

/* Including external dependencies */
#include <UTF8String.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Tag */
typedef UTF8String_t	 Tag_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Tag;
asn_struct_free_f Tag_free;
asn_struct_print_f Tag_print;
asn_constr_check_f Tag_constraint;
ber_type_decoder_f Tag_decode_ber;
der_type_encoder_f Tag_encode_der;
xer_type_decoder_f Tag_decode_xer;
xer_type_encoder_f Tag_encode_xer;
per_type_decoder_f Tag_decode_uper;
per_type_encoder_f Tag_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _Tag_H_ */
#include <asn_internal.h>
