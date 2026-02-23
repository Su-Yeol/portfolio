/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSstationinfo_H_
#define	_CITSstationinfo_H_


#include "asn_application.h"

/* Including external dependencies */
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSStationDetail;

/* CITSstationinfo */
typedef struct CITSstationinfo {
	struct CITSStationDetail	*gasoline;	/* OPTIONAL */
	struct CITSStationDetail	*diesel;	/* OPTIONAL */
	struct CITSStationDetail	*lpg;	/* OPTIONAL */
	struct CITSStationDetail	*premium;	/* OPTIONAL */
	struct CITSStationDetail	*hydrogen;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSstationinfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSstationinfo;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSstationinfo_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSstationinfo_1[5];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSStationDetail.h"

#endif	/* _CITSstationinfo_H_ */
#include "asn_internal.h"
