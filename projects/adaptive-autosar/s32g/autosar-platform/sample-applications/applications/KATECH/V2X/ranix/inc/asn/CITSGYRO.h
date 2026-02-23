/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSGYRO_H_
#define	_CITSGYRO_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSGYRO */
typedef struct CITSGYRO {
	long	*xmax;	/* OPTIONAL */
	long	*xmin;	/* OPTIONAL */
	long	*xavg;	/* OPTIONAL */
	long	*ymax;	/* OPTIONAL */
	long	*ymin;	/* OPTIONAL */
	long	*yavg;	/* OPTIONAL */
	long	*zmax;	/* OPTIONAL */
	long	*zmin;	/* OPTIONAL */
	long	*zavg;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSGYRO_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSGYRO;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSGYRO_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSGYRO_1[9];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSGYRO_H_ */
#include "asn_internal.h"
