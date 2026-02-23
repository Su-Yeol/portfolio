/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSPVD_H_
#define	_CITSPVD_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "Acceleration.h"
#include "DectLaneID.h"
#include "EventDrowsy.h"
#include "DeviceMgmt.h"
#include "TransitStatus.h"
#include "ITIScodes.h"
#include "LaneID.h"
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "CITSText.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSADAS;
struct DDateTime;
struct CITSVehicleValue;
struct CITSSVCLog;
struct CITSGYRO;

/* CITSPVD */
typedef struct CITSPVD {
	long	*msgID;	/* OPTIONAL */
	Acceleration_t	*acceleration;	/* OPTIONAL */
	DectLaneID_t	*curlane;	/* OPTIONAL */
	struct CITSADAS	*adas;	/* OPTIONAL */
	EventDrowsy_t	*drowsy;	/* OPTIONAL */
	DeviceMgmt_t	*devicemgmt;	/* OPTIONAL */
	TransitStatus_t	*transitStatus;	/* OPTIONAL */
	struct CITSPVD__worklanes {
		A_SEQUENCE_OF(LaneID_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *worklanes;
	struct DDateTime	*workstart;	/* OPTIONAL */
	struct DDateTime	*workend;	/* OPTIONAL */
	long	*vehiclecnt;	/* OPTIONAL */
	struct CITSVehicleValue	*vehicleInfo;	/* OPTIONAL */
	ITIScodes_t	*typeEvent;	/* OPTIONAL */
	struct CITSPVD__description {
		A_SEQUENCE_OF(ITIScodes_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *description;
	struct CITSSVCLog	*serviceLogAll;	/* OPTIONAL */
	struct CITSGYRO	*xyzvalue;	/* OPTIONAL */
	struct CITSPVD__text {
		A_SEQUENCE_OF(CITSText_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *text;
	struct CITSPVD__subtext {
		A_SEQUENCE_OF(CITSText_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *subtext;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSPVD_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSPVD;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSPVD_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSPVD_1[18];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSADAS.h"
#include "DDateTime.h"
#include "CITSVehicleValue.h"
#include "CITSSVCLog.h"
#include "CITSGYRO.h"

#endif	/* _CITSPVD_H_ */
#include "asn_internal.h"
