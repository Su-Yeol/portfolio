/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetBrakeSystemStatus_H_
#define	_SetBrakeSystemStatus_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"
#include "ENUMERATED.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SetBrakeSystemStatus__brakeAppliedStatus {
	SetBrakeSystemStatus__brakeAppliedStatus_unavailable	= 0,
	SetBrakeSystemStatus__brakeAppliedStatus_leftFront	= 1,
	SetBrakeSystemStatus__brakeAppliedStatus_leftRear	= 2,
	SetBrakeSystemStatus__brakeAppliedStatus_rightFront	= 3,
	SetBrakeSystemStatus__brakeAppliedStatus_rightRear	= 4
} e_SetBrakeSystemStatus__brakeAppliedStatus;
typedef enum SetBrakeSystemStatus__tractionControlStatus {
	SetBrakeSystemStatus__tractionControlStatus_unavailable	= 0,
	SetBrakeSystemStatus__tractionControlStatus_off	= 1,
	SetBrakeSystemStatus__tractionControlStatus_on	= 2,
	SetBrakeSystemStatus__tractionControlStatus_engaged	= 3
} e_SetBrakeSystemStatus__tractionControlStatus;
typedef enum SetBrakeSystemStatus__antiLockBrakeStatus {
	SetBrakeSystemStatus__antiLockBrakeStatus_unavailable	= 0,
	SetBrakeSystemStatus__antiLockBrakeStatus_off	= 1,
	SetBrakeSystemStatus__antiLockBrakeStatus_on	= 2,
	SetBrakeSystemStatus__antiLockBrakeStatus_engaged	= 3
} e_SetBrakeSystemStatus__antiLockBrakeStatus;
typedef enum SetBrakeSystemStatus__stabilityControlStatus {
	SetBrakeSystemStatus__stabilityControlStatus_unavailable	= 0,
	SetBrakeSystemStatus__stabilityControlStatus_off	= 1,
	SetBrakeSystemStatus__stabilityControlStatus_on	= 2,
	SetBrakeSystemStatus__stabilityControlStatus_engaged	= 3
} e_SetBrakeSystemStatus__stabilityControlStatus;
typedef enum SetBrakeSystemStatus__brakeBoostApplied {
	SetBrakeSystemStatus__brakeBoostApplied_unavailable	= 0,
	SetBrakeSystemStatus__brakeBoostApplied_off	= 1,
	SetBrakeSystemStatus__brakeBoostApplied_on	= 2
} e_SetBrakeSystemStatus__brakeBoostApplied;
typedef enum SetBrakeSystemStatus__auxiliaryBrakeStatus {
	SetBrakeSystemStatus__auxiliaryBrakeStatus_unavailable	= 0,
	SetBrakeSystemStatus__auxiliaryBrakeStatus_off	= 1,
	SetBrakeSystemStatus__auxiliaryBrakeStatus_on	= 2,
	SetBrakeSystemStatus__auxiliaryBrakeStatus_reserved	= 3
} e_SetBrakeSystemStatus__auxiliaryBrakeStatus;

/* SetBrakeSystemStatus */
typedef struct SetBrakeSystemStatus {
	BIT_STRING_t	 brakeAppliedStatus;
	ENUMERATED_t	 tractionControlStatus;
	ENUMERATED_t	 antiLockBrakeStatus;
	ENUMERATED_t	 stabilityControlStatus;
	ENUMERATED_t	 brakeBoostApplied;
	ENUMERATED_t	 auxiliaryBrakeStatus;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SetBrakeSystemStatus_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_tractionControlStatus_8;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_antiLockBrakeStatus_13;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_stabilityControlStatus_18;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_brakeBoostApplied_23;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_auxiliaryBrakeStatus_27;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_SetBrakeSystemStatus;
extern asn_SEQUENCE_specifics_t asn_SPC_SetBrakeSystemStatus_specs_1;
extern asn_TYPE_member_t asn_MBR_SetBrakeSystemStatus_1[6];

#ifdef __cplusplus
}
#endif

#endif	/* _SetBrakeSystemStatus_H_ */
#include "asn_internal.h"
