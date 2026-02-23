/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSWeatherInfo_H_
#define	_CITSWeatherInfo_H_


#include "asn_application.h"

/* Including external dependencies */
#include "AmbientAirTemperature.h"
#include "EssPrecipRate.h"
#include "NativeInteger.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSWeatherInfo */
typedef struct CITSWeatherInfo {
	AmbientAirTemperature_t	*temp;	/* OPTIONAL */
	EssPrecipRate_t	*rainfall;	/* OPTIONAL */
	long	*humidity;	/* OPTIONAL */
	long	*windDir;	/* OPTIONAL */
	long	*windSpd;	/* OPTIONAL */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CITSWeatherInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CITSWeatherInfo;
extern asn_SEQUENCE_specifics_t asn_SPC_CITSWeatherInfo_specs_1;
extern asn_TYPE_member_t asn_MBR_CITSWeatherInfo_1[5];

#ifdef __cplusplus
}
#endif

#endif	/* _CITSWeatherInfo_H_ */
#include "asn_internal.h"
