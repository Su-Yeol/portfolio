/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSStationDetail_H_
#define	_CITSStationDetail_H_


#include "asn_application.h"

/* Including external dependencies */
#include "UTF8String.h"
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSStationDetail */
typedef struct CITSStationDetail {
	UTF8String_t	*name;	/* OPTIONAL */
	long	*capacity;	/* OPTIONAL */
	long	*value;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSStationDetail_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSStationDetail;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSStationDetail_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSStationDetail_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSStationDetail_H_ */
#include "asn_internal.h"
