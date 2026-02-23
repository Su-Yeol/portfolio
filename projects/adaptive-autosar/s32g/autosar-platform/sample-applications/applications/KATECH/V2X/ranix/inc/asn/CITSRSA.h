/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSRSA_H_
#define	_CITSRSA_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "OCTET_STRING.h"
#include "LinkID.h"
#include "DectLaneID.h"
#include "StopID.h"
#include "CITSText.h"
#include "MovementPhaseState.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSRSA */
typedef struct CITSRSA {
	long	*msgID;	/* OPTIONAL */
	OCTET_STRING_t	*objID;	/* OPTIONAL */
	long	*percent;	/* OPTIONAL */
	long	*limitspeed;	/* OPTIONAL */
	long	*lanewidth;	/* OPTIONAL */
	LinkID_t	*linkID;	/* OPTIONAL */
	DectLaneID_t	*eventlane;	/* OPTIONAL */
	StopID_t	*stopID;	/* OPTIONAL */
	CITSText_t	*text;	/* OPTIONAL */
	MovementPhaseState_t	*eventState;	/* OPTIONAL */
	long	*detectedObjCounts;	/* OPTIONAL */
	CITSText_t	*sendUniqueId;	/* OPTIONAL */
	long	*intersectionID;	/* OPTIONAL */
	long	*signalGroupID;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSRSA_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSRSA;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSRSA_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSRSA_1[14];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSRSA_H_ */
#include "asn_internal.h"
