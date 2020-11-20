/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Implicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#ifndef	_CRLReason_H_
#define	_CRLReason_H_


#include <asn_application.h>

/* Including external dependencies */
#include <ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CRLReason {
	CRLReason_unspecified	= 0,
	CRLReason_keyCompromise	= 1,
	CRLReason_cACompromise	= 2,
	CRLReason_affiliationChanged	= 3,
	CRLReason_superseded	= 4,
	CRLReason_cessationOfOperation	= 5,
	CRLReason_certificateHold	= 6,
	CRLReason_removeFromCRL	= 8,
	CRLReason_privilegeWithdrawn	= 9,
	CRLReason_aACompromise	= 10
} e_CRLReason;

/* CRLReason */
typedef ENUMERATED_t	 CRLReason_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CRLReason;
asn_struct_free_f CRLReason_free;
asn_struct_print_f CRLReason_print;
asn_constr_check_f CRLReason_constraint;
ber_type_decoder_f CRLReason_decode_ber;
der_type_encoder_f CRLReason_encode_der;
xer_type_decoder_f CRLReason_decode_xer;
xer_type_encoder_f CRLReason_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _CRLReason_H_ */
#include <asn_internal.h>
