/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BCC-LocationTypes"
 * 	found in "../schemas/asn/common/BCC-LocationTypes.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_GnssFixStatus_H_
#define	_GnssFixStatus_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum GnssFixStatus {
	GnssFixStatus_noFix	= 0,
	GnssFixStatus_timeFix	= 1,
	GnssFixStatus_twoDFix	= 2,
	GnssFixStatus_threeDdFix	= 3
	/*
	 * Enumeration is extensible
	 */
} e_GnssFixStatus;

/* GnssFixStatus */
typedef long	 GnssFixStatus_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_GnssFixStatus;
asn_struct_free_f GnssFixStatus_free;
asn_struct_print_f GnssFixStatus_print;
asn_constr_check_f GnssFixStatus_constraint;
ber_type_decoder_f GnssFixStatus_decode_ber;
der_type_encoder_f GnssFixStatus_encode_der;
xer_type_decoder_f GnssFixStatus_decode_xer;
xer_type_encoder_f GnssFixStatus_encode_xer;
per_type_decoder_f GnssFixStatus_decode_uper;
per_type_encoder_f GnssFixStatus_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _GnssFixStatus_H_ */
#include <asn_internal.h>
