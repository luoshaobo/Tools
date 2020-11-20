/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CCM-Header"
 * 	found in "../schemas/asn/message/CCM-Header.asn"
 * 	`asn1c -S`
 */

#ifndef	_StoreAndForwardOptions_H_
#define	_StoreAndForwardOptions_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum StoreAndForwardOptions {
	StoreAndForwardOptions_storeAlways	= 0,
	StoreAndForwardOptions_storeLatest	= 1,
	StoreAndForwardOptions_dontStore	= 2
	/*
	 * Enumeration is extensible
	 */
} e_StoreAndForwardOptions;

/* StoreAndForwardOptions */
typedef long	 StoreAndForwardOptions_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_StoreAndForwardOptions;
asn_struct_free_f StoreAndForwardOptions_free;
asn_struct_print_f StoreAndForwardOptions_print;
asn_constr_check_f StoreAndForwardOptions_constraint;
ber_type_decoder_f StoreAndForwardOptions_decode_ber;
der_type_encoder_f StoreAndForwardOptions_encode_der;
xer_type_decoder_f StoreAndForwardOptions_decode_xer;
xer_type_encoder_f StoreAndForwardOptions_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _StoreAndForwardOptions_H_ */
#include <asn_internal.h>