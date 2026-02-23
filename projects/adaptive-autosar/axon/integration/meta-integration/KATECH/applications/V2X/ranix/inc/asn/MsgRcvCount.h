/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_MsgRcvCount_H_
#define	_MsgRcvCount_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MsgRcvCount */
typedef struct MsgRcvCount {
	long	 rBSM_WAVECV2X;
	long	 rSPAT_WAVECV2X;
	long	 rRTCM_WAVECV2X;
	long	 rMAP_WAVECV2X;
	long	 rRSA_WAVECV2X;
	long	 rTIM_WAVECV2X;
	long	 rSPAT_5GLTE;
	long	 rRTCM_5GLTE;
	long	 rMAP_5GLTE;
	long	 rRSA_5GLTE;
	long	 rTIM_5GLTE;
	long	 rDisplay;
	long	 rOutVehicle;
	long	 rInVehicle;
	long	 rADAS;
	long	 rDrowsy;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MsgRcvCount_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MsgRcvCount;
extern asn_SEQUENCE_specifics_t asn_SPC_MsgRcvCount_specs_1;
extern asn_TYPE_member_t asn_MBR_MsgRcvCount_1[16];

#ifdef __cplusplus
}
#endif

#endif	/* _MsgRcvCount_H_ */
#include "asn_internal.h"
