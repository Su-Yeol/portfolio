/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSTIM_H_
#define	_CITSTIM_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "LinkID.h"
#include "StopID.h"
#include "DectLaneID.h"
#include "RIDEAlarm.h"
#include "CITSText.h"
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSWeatherInfo;
struct CITSRestInfo;
struct CITSTolgateLaneInfo;

/* CITSTIM */
typedef struct CITSTIM {
	long	*msgID;	/* OPTIONAL */
	struct CITSTIM__text {
		A_SEQUENCE_OF(CITSText_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *text;
	struct CITSTIM__subtext {
		A_SEQUENCE_OF(CITSText_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *subtext;
	long	*zipcode;	/* OPTIONAL */
	LinkID_t	*linkID;	/* OPTIONAL */
	StopID_t	*stopID;	/* OPTIONAL */
	DectLaneID_t	*fromlaneID;	/* OPTIONAL */
	DectLaneID_t	*tolaneID;	/* OPTIONAL */
	RIDEAlarm_t	*alarm;	/* OPTIONAL */
	struct CITSWeatherInfo	*weatherInfo;	/* OPTIONAL */
	struct CITSRestInfo	*servicearea;	/* OPTIONAL */
	struct CITSTolgateLaneInfo	*tolgate;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSTIM_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSTIM;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSTIM_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSTIM_1[12];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSWeatherInfo.h"
#include "CITSRestInfo.h"
#include "CITSTolgateLaneInfo.h"

#endif	/* _CITSTIM_H_ */
#include "asn_internal.h"
