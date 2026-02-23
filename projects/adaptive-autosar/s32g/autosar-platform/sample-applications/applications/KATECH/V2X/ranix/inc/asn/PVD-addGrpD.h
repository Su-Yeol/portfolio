/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_PVD_addGrpD_H_
#define	_PVD_addGrpD_H_


#include "asn_application.h"

/* Including external dependencies */
#include "asn_SEQUENCE_OF.h"
#include "constr_SEQUENCE_OF.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSPVD;

/* PVD-addGrpD */
typedef struct PVD_addGrpD {
	struct PVD_addGrpD__cits {
		A_SEQUENCE_OF(struct CITSPVD) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *cits;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PVD_addGrpD_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PVD_addGrpD;
extern asn_SEQUENCE_specifics_t asn_SPC_PVD_addGrpD_specs_1;
extern asn_TYPE_member_t asn_MBR_PVD_addGrpD_1[1];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSPVD.h"

#endif	/* _PVD_addGrpD_H_ */
#include "asn_internal.h"
