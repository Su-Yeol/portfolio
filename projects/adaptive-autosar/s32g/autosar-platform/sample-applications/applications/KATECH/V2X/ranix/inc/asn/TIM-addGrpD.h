/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_TIM_addGrpD_H_
#define	_TIM_addGrpD_H_


#include "asn_application.h"

/* Including external dependencies */
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSTIM;

/* TIM-addGrpD */
typedef struct TIM_addGrpD {
	struct TIM_addGrpD__cits {
		A_SEQUENCE_OF(struct CITSTIM) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *cits;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} TIM_addGrpD_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_TIM_addGrpD;
extern asn_SEQUENCE_specifics_t asn_SPC_TIM_addGrpD_specs_1;
extern asn_TYPE_member_t asn_MBR_TIM_addGrpD_1[1];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSTIM.h"

#endif	/* _TIM_addGrpD_H_ */
#include "asn_internal.h"
