/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSSVCLog_H_
#define	_CITSSVCLog_H_


#include "asn_application.h"

/* Including external dependencies */
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct MsgRcvCount;
struct SVCMsgSet;
struct DispMsgSet;

/* CITSSVCLog */
typedef struct CITSSVCLog {
	struct MsgRcvCount	*rMsgCnt;	/* OPTIONAL */
	struct CITSSVCLog__svcMsg {
		A_SEQUENCE_OF(struct SVCMsgSet) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *svcMsg;
	struct CITSSVCLog__dispMsg {
		A_SEQUENCE_OF(struct DispMsgSet) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *dispMsg;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSSVCLog_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSSVCLog;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSSVCLog_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSSVCLog_1[3];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "MsgRcvCount.h"
#include "SVCMsgSet.h"
#include "DispMsgSet.h"

#endif	/* _CITSSVCLog_H_ */
#include "asn_internal.h"
