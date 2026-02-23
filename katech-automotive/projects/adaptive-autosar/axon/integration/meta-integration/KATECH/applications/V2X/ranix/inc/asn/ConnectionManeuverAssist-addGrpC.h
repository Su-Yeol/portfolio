/*
 * From ASN.1 module "AddGrpC"
 */

#ifndef	_ConnectionManeuverAssist_addGrpC_H_
#define	_ConnectionManeuverAssist_addGrpC_H_


#include "asn_application.h"

/* Including external dependencies */
#include "VehicleToLanePositionList.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct NodeOffsetPointXY;

/* ConnectionManeuverAssist-addGrpC */
typedef struct ConnectionManeuverAssist_addGrpC {
	VehicleToLanePositionList_t	 vehicleToLanePositions;
	struct NodeOffsetPointXY	*rsuDistanceFromAnchor;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ConnectionManeuverAssist_addGrpC_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ConnectionManeuverAssist_addGrpC;
extern asn_SEQUENCE_specifics_t asn_SPC_ConnectionManeuverAssist_addGrpC_specs_1;
extern asn_TYPE_member_t asn_MBR_ConnectionManeuverAssist_addGrpC_1[2];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "NodeOffsetPointXY.h"

#endif	/* _ConnectionManeuverAssist_addGrpC_H_ */
#include "asn_internal.h"
