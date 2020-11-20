/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "../schemas/asn/RFC-3280/PKIX1Explicit88.asn"
 * 	`asn1c -fwide-types -S`
 */

#ifndef	_OrganizationalUnitName_H_
#define	_OrganizationalUnitName_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OrganizationalUnitName */
typedef PrintableString_t	 OrganizationalUnitName_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_OrganizationalUnitName;
asn_struct_free_f OrganizationalUnitName_free;
asn_struct_print_f OrganizationalUnitName_print;
asn_constr_check_f OrganizationalUnitName_constraint;
ber_type_decoder_f OrganizationalUnitName_decode_ber;
der_type_encoder_f OrganizationalUnitName_encode_der;
xer_type_decoder_f OrganizationalUnitName_decode_xer;
xer_type_encoder_f OrganizationalUnitName_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _OrganizationalUnitName_H_ */
#include <asn_internal.h>
