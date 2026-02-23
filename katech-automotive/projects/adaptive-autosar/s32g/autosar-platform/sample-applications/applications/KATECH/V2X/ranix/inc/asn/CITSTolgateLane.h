/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSTolgateLane_H_
#define	_CITSTolgateLane_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "CITSTolgateType.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSTolgateLane */
typedef struct CITSTolgateLane {
	long	 laneno;
	CITSTolgateType_t	 type;
	long	 visible;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSTolgateLane_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSTolgateLane;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSTolgateLane_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSTolgateLane_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSTolgateLane_H_ */
#include "asn_internal.h"
