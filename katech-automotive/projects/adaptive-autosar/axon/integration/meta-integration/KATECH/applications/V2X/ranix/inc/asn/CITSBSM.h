/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSBSM_H_
#define	_CITSBSM_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ITIScodes.h"
#include "LaneID.h"
#include "DectLaneID.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSADAS;

/* CITSBSM */
typedef struct CITSBSM {
	struct CITSADAS	*adas;	/* OPTIONAL */
	ITIScodes_t	*typeEvent;	/* OPTIONAL */
	LaneID_t	*laneID;	/* OPTIONAL */
	DectLaneID_t	*curlane;	/* OPTIONAL */
	OCTET_STRING_t	*cooperateID;	/* OPTIONAL */
	IA5String_t	*url;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSBSM_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSBSM;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSBSM_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSBSM_1[6];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSADAS.h"

#endif	/* _CITSBSM_H_ */
#include "asn_internal.h"
