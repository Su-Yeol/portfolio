/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetVehicleSize_H_
#define	_SetVehicleSize_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SetVehicleSize */
typedef struct SetVehicleSize {
	long	 vehicleWidth;
	long	 vehicleLength;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SetVehicleSize_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SetVehicleSize;
extern asn_SEQUENCE_specifics_t asn_SPC_SetVehicleSize_specs_1;
extern asn_TYPE_member_t asn_MBR_SetVehicleSize_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _SetVehicleSize_H_ */
#include "asn_internal.h"
