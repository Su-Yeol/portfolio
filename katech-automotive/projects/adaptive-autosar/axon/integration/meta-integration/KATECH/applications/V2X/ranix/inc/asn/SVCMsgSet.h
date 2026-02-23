/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_SVCMsgSet_H_
#define	_SVCMsgSet_H_


#include "asn_application.h"

/* Including external dependencies */
#include "FurtherInfoID.h"
#include "TemporaryID.h"
#include "DDateTime.h"
#include "MsgDetail.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SVCMsgSet */
typedef struct SVCMsgSet {
	FurtherInfoID_t	 furtherInfoID;
	TemporaryID_t	 svcMsgID;
	DDateTime_t	 svcMsgDate;
	MsgDetail_t	 svcMsgDetail;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SVCMsgSet_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SVCMsgSet;
extern asn_SEQUENCE_specifics_t asn_SPC_SVCMsgSet_specs_1;
extern asn_TYPE_member_t asn_MBR_SVCMsgSet_1[4];

#ifdef __cplusplus
}
#endif

#endif	/* _SVCMsgSet_H_ */
#include "asn_internal.h"
