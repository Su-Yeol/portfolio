/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_DispMsgSet_H_
#define	_DispMsgSet_H_


#include "asn_application.h"

/* Including external dependencies */
#include "TemporaryID.h"
#include "DDateTime.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DispMsgSet */
typedef struct DispMsgSet {
	TemporaryID_t	 svcMsgID;
	DDateTime_t	 dispTime;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DispMsgSet_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DispMsgSet;
extern asn_SEQUENCE_specifics_t asn_SPC_DispMsgSet_specs_1;
extern asn_TYPE_member_t asn_MBR_DispMsgSet_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _DispMsgSet_H_ */
#include "asn_internal.h"
