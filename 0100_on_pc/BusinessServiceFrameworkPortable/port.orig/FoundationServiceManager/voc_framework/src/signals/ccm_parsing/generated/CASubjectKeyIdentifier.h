/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CA-CommonTypes"
 * 	found in "../schemas/common/CA-CommonTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_CASubjectKeyIdentifier_H_
#define	_CASubjectKeyIdentifier_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CASubjectKeyIdentifier */
typedef OCTET_STRING_t	 CASubjectKeyIdentifier_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CASubjectKeyIdentifier;
asn_struct_free_f CASubjectKeyIdentifier_free;
asn_struct_print_f CASubjectKeyIdentifier_print;
asn_constr_check_f CASubjectKeyIdentifier_constraint;
ber_type_decoder_f CASubjectKeyIdentifier_decode_ber;
der_type_encoder_f CASubjectKeyIdentifier_encode_der;
xer_type_decoder_f CASubjectKeyIdentifier_decode_xer;
xer_type_encoder_f CASubjectKeyIdentifier_encode_xer;
per_type_decoder_f CASubjectKeyIdentifier_decode_uper;
per_type_encoder_f CASubjectKeyIdentifier_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _CASubjectKeyIdentifier_H_ */
#include <asn_internal.h>
