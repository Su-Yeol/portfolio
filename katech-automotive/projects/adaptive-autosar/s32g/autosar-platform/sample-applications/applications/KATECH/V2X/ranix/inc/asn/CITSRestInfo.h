/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSRestInfo_H_
#define	_CITSRestInfo_H_


#include "asn_application.h"

/* Including external dependencies */
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSStationDetail;
struct CITSstationinfo;
struct CITSelectric;
struct CITSParking;

/* CITSRestInfo */
typedef struct CITSRestInfo {
	struct CITSStationDetail	*restinfo;	/* OPTIONAL */
	struct CITSstationinfo	*gas;	/* OPTIONAL */
	struct CITSelectric	*electric;	/* OPTIONAL */
	struct CITSParking	*parking;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSRestInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSRestInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSRestInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSRestInfo_1[4];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSStationDetail.h"
#include "CITSstationinfo.h"
#include "CITSelectric.h"
#include "CITSParking.h"

#endif	/* _CITSRestInfo_H_ */
#include "asn_internal.h"
