/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_BSM_addGrpD_H_
#define	_BSM_addGrpD_H_


#include "asn_application.h"

/* Including external dependencies */
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSBSM;

/* BSM-addGrpD */
typedef struct BSM_addGrpD {
	struct CITSBSM	*cits;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BSM_addGrpD_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BSM_addGrpD;
extern asn_SEQUENCE_specifics_t asn_SPC_BSM_addGrpD_specs_1;
extern asn_TYPE_member_t asn_MBR_BSM_addGrpD_1[1];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSBSM.h"

#endif	/* _BSM_addGrpD_H_ */
#include "asn_internal.h"
