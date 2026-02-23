/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSelectric_H_
#define	_CITSelectric_H_


#include "asn_application.h"

/* Including external dependencies */
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct CITSStationDetail;

/* CITSelectric */
typedef struct CITSelectric {
	struct CITSStationDetail	*normal;	/* OPTIONAL */
	struct CITSStationDetail	*quick;	/* OPTIONAL */
	struct CITSStationDetail	*super;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSelectric_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSelectric;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSelectric_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSelectric_1[3];

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "CITSStationDetail.h"

#endif	/* _CITSelectric_H_ */
#include "asn_internal.h"
