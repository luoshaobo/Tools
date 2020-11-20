/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "CCM-Header"
 * 	found in "../schemas/asn/message/CCM-Header.asn"
 * 	`asn1c -S`
 */

#ifndef	_SmsDeliveryOptions_H_
#define	_SmsDeliveryOptions_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SmsDeliveryOptions {
	SmsDeliveryOptions_sendSmsAsBackup	= 0,
	SmsDeliveryOptions_sendSmsAsBackupAndRetain	= 1
	/*
	 * Enumeration is extensible
	 */
} e_SmsDeliveryOptions;

/* SmsDeliveryOptions */
typedef long	 SmsDeliveryOptions_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SmsDeliveryOptions;
asn_struct_free_f SmsDeliveryOptions_free;
asn_struct_print_f SmsDeliveryOptions_print;
asn_constr_check_f SmsDeliveryOptions_constraint;
ber_type_decoder_f SmsDeliveryOptions_decode_ber;
der_type_encoder_f SmsDeliveryOptions_encode_der;
xer_type_decoder_f SmsDeliveryOptions_decode_xer;
xer_type_encoder_f SmsDeliveryOptions_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _SmsDeliveryOptions_H_ */
#include <asn_internal.h>
