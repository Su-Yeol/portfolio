/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSVehicleValue_H_
#define	_CITSVehicleValue_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSVehicleValue */
typedef struct CITSVehicleValue {
	long	*rpm;	/* OPTIONAL */
	long	*gears;	/* OPTIONAL */
	long	*mileage;	/* OPTIONAL */
	long	*percent;	/* OPTIONAL */
	long	*liter;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSVehicleValue_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSVehicleValue;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSVehicleValue_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSVehicleValue_1[5];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSVehicleValue_H_ */
#include "asn_internal.h"
