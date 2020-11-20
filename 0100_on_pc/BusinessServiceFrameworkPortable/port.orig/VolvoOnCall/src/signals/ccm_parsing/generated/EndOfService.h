/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-CommonTypes"
 * 	found in "../schemas/asn/common/BCC-CommonTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_EndOfService_H_
#define	_EndOfService_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum EndOfService {
	EndOfService_serviceFinishedNormally	= 0,
	EndOfService_timeout	= 1
	/*
	 * Enumeration is extensible
	 */
} e_EndOfService;

/* EndOfService */
typedef long	 EndOfService_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EndOfService;
asn_struct_free_f EndOfService_free;
asn_struct_print_f EndOfService_print;
asn_constr_check_f EndOfService_constraint;
ber_type_decoder_f EndOfService_decode_ber;
der_type_encoder_f EndOfService_encode_der;
xer_type_decoder_f EndOfService_decode_xer;
xer_type_encoder_f EndOfService_encode_xer;
per_type_decoder_f EndOfService_decode_uper;
per_type_encoder_f EndOfService_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _EndOfService_H_ */
#include <asn_internal.h>