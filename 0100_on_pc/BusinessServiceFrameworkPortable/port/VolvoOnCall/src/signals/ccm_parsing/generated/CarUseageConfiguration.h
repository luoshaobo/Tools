/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "BasicCarControl"
 * 	found in "../schemas/asn/functions/BasicCarControl.asn"
 * 	`asn1c -gen-PER -S`
 */

#ifndef	_CarUseageConfiguration_H_
#define	_CarUseageConfiguration_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CarUsageOperation;
struct CarUsageOption;
struct ChannelConfiguration;

/* CarUseageConfiguration */
typedef struct CarUseageConfiguration {
	struct cucoperations {
		A_SEQUENCE_OF(struct CarUsageOperation) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} cucoperations;
	struct cucoptions {
		A_SEQUENCE_OF(struct CarUsageOption) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *cucoptions;
	struct cucallowedChannelsFromVehicle {
		A_SEQUENCE_OF(struct ChannelConfiguration) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} cucallowedChannelsFromVehicle;
	struct cucallowedChannelsToVehicle {
		A_SEQUENCE_OF(struct ChannelConfiguration) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} cucallowedChannelsToVehicle;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CarUseageConfiguration_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CarUseageConfiguration;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CarUsageOperation.h"
#include "CarUsageOption.h"
#include "ChannelConfiguration.h"

#endif	/* _CarUseageConfiguration_H_ */
#include <asn_internal.h>
