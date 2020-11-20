/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CCM-Header"
 * 	found in "../schemas/asn/message/CCM-Header.asn"
 * 	`asn1c -S`
 */

#ifndef	_SessionId_H_
#define	_SessionId_H_


#include <asn_application.h>

/* Including external dependencies */
#include <OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SessionId */
typedef OCTET_STRING_t	 SessionId_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SessionId;
asn_struct_free_f SessionId_free;
asn_struct_print_f SessionId_print;
asn_constr_check_f SessionId_constraint;
ber_type_decoder_f SessionId_decode_ber;
der_type_encoder_f SessionId_encode_der;
xer_type_decoder_f SessionId_decode_xer;
xer_type_encoder_f SessionId_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _SessionId_H_ */
#include <asn_internal.h>
