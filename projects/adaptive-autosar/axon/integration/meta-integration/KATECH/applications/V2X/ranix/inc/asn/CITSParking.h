/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSParking_H_
#define	_CITSParking_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "CITSStationDetail.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSParking */
typedef struct CITSParking {
	long	 parkinglot_id;
	CITSStationDetail_t	 normal;
	CITSStationDetail_t	 disable;
	CITSStationDetail_t	 big;
	CITSStationDetail_t	 compact;
	CITSStationDetail_t	 emergency;
	CITSStationDetail_t	 women;
	CITSStationDetail_t	 etc;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSParking_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSParking;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSParking_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSParking_1[8];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSParking_H_ */
#include "asn_internal.h"
