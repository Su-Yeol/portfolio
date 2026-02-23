/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_MsgDetail_H_
#define	_MsgDetail_H_


#include "asn_application.h"

/* Including external dependencies */
#include "MsgChannel.h"
#include "MsgType.h"
#include "DispLocation.h"
#include "ITIScodes.h"
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MsgDetail */
typedef struct MsgDetail {
	MsgChannel_t	 svcMsgSndChl;
	MsgType_t	 svcMsgType;
	DispLocation_t	 svcDispLocation;
	ITIScodes_t	 svcMsgEvent;
	long	*rcvMsgID;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MsgDetail_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MsgDetail;
extern asn_SEQUENCE_specifics_t asn_SPC_MsgDetail_specs_1;
extern asn_TYPE_member_t asn_MBR_MsgDetail_1[5];

#ifdef __cplusplus
}
#endif

#endif	/* _MsgDetail_H_ */
#include "asn_internal.h"
