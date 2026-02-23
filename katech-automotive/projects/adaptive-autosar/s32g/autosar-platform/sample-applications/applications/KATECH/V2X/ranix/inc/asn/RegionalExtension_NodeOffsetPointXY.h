/*
 * From ASN.1 module "DSRC"
 */

#ifndef	_RegionalExtension_NodeOffsetPointXT_H_
#define	_RegionalExtension_NodeOffsetPointXT_H_


#include "asn_application.h"

/* Including external dependencies */
#include "RegionId.h"
#include "ANY.h"
#include "asn_ioc.h"
#include "OPEN_TYPE.h"
#include "constr_CHOICE.h"
#include "constr_SEQUENCE.h"
#include "NodeOffsetPointXY-addGrpB.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */

typedef enum Reg_NodeOffsetPointXY__regExtValue_PR {
	Reg_NodeOffsetPointXY__regExtValue_PR_NOTHING,	/* No components present */
	Reg_NodeOffsetPointXY__regExtValue_PR_NodeOffsetPointXY_addGrpB
} Reg_NodeOffsetPointXY__regExtValue_PR;

typedef struct Reg_NodeOffsetPointXY {
	RegionId_t	 regionId;
	struct Reg_NodeOffsetPointXY__regExtValue {
		Reg_NodeOffsetPointXY__regExtValue_PR present;
		union Reg_NodeOffsetPointXY__regExtValue_u {
			NodeOffsetPointXY_addGrpB_t	 NodeOffsetPointXY_addGrpB;
		} choice;

		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} regExtValue;

	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Reg_NodeOffsetPointXY_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Reg_NodeOffsetPointXY;
extern asn_SEQUENCE_specifics_t asn_SPC_Reg_NodeOffsetPointXY_specs_142;
extern asn_TYPE_member_t asn_MBR_Reg_NodeOffsetPointXY_142[2];

#ifdef __cplusplus
}
#endif

#endif	/* _RegionalExtension_NodeOffsetPointXT_H_ */
#include "asn_internal.h"
