/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSADAS_H_
#define	_CITSADAS_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ADASFCWSEventType.h"
#include "ADASLDWSEventType.h"
#include "ADASPCWSEventType.h"
#include "ADASOTHERSEventType.h"
#include "NativeInteger.h"
#include "DSRC_Longitude.h"
#include "DSRC_Latitude.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSADAS */
typedef struct CITSADAS {
	ADASFCWSEventType_t	*fcws;	/* OPTIONAL */
	ADASLDWSEventType_t	*ldws;	/* OPTIONAL */
	ADASPCWSEventType_t	*pcws;	/* OPTIONAL */
	ADASOTHERSEventType_t	*others;	/* OPTIONAL */
	long	*ttcDistance;	/* OPTIONAL */
	long	*ttcTime;	/* OPTIONAL */
	DSRC_Longitude_t	*Long;	/* OPTIONAL */
	DSRC_Latitude_t	*lat;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSADAS_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSADAS;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSADAS_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSADAS_1[8];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSADAS_H_ */
#include "asn_internal.h"
