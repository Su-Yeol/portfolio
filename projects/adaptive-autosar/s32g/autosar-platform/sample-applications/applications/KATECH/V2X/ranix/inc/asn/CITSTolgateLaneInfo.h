/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSTolgateLaneInfo_H_
#define	_CITSTolgateLaneInfo_H_


#include "asn_application.h"

/* Including external dependencies */
#include "UTF8String.h"
#include "NativeInteger.h"
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSTolgateLane;

/* CITSTolgateLaneInfo */
typedef struct CITSTolgateLaneInfo {
	UTF8String_t	 name;
	long	 heading;
	struct CITSTolgateLaneInfo__lanes {
		A_SEQUENCE_OF(struct CITSTolgateLane) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} lanes;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSTolgateLaneInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSTolgateLaneInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSTolgateLaneInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSTolgateLaneInfo_1[3];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSTolgateLane.h"

#endif	/* _CITSTolgateLaneInfo_H_ */
#include "asn_internal.h"
