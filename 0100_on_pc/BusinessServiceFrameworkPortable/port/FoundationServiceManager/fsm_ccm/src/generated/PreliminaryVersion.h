/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CCM-CommonTypes"
 * 	found in "../schemas/asn/common/CCM-CommonTypes.asn"
 * 	`asn1c -S`
 */

#ifndef	_PreliminaryVersion_H_
#define	_PreliminaryVersion_H_


#include <asn_application.h>

/* Including external dependencies */
#include "INT32.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PreliminaryVersion */
typedef INT32_t	 PreliminaryVersion_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PreliminaryVersion;
asn_struct_free_f PreliminaryVersion_free;
asn_struct_print_f PreliminaryVersion_print;
asn_constr_check_f PreliminaryVersion_constraint;
ber_type_decoder_f PreliminaryVersion_decode_ber;
der_type_encoder_f PreliminaryVersion_encode_der;
xer_type_decoder_f PreliminaryVersion_decode_xer;
xer_type_encoder_f PreliminaryVersion_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _PreliminaryVersion_H_ */
#include <asn_internal.h>