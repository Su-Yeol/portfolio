/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetVehicleEventFlags_H_
#define	_SetVehicleEventFlags_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SetVehicleEventFlags {
	SetVehicleEventFlags_eventHazardLights	= 0,
	SetVehicleEventFlags_eventStopLineViolation	= 1,
	SetVehicleEventFlags_eventABSactivated	= 2,
	SetVehicleEventFlags_eventTractionControlLoss	= 3,
	SetVehicleEventFlags_eventStabilityControlActivated	= 4,
	SetVehicleEventFlags_eventHazardousMaterials	= 5,
	SetVehicleEventFlags_eventReserved1	= 6,
	SetVehicleEventFlags_eventHardBraking	= 7,
	SetVehicleEventFlags_eventLightsChanged	= 8,
	SetVehicleEventFlags_eventWipersChanged	= 9,
	SetVehicleEventFlags_eventFlatTire	= 10,
	SetVehicleEventFlags_eventDisabledVehicle	= 11,
	SetVehicleEventFlags_eventAirBagDeployment	= 12
} e_SetVehicleEventFlags;

/* SetVehicleEventFlags */
typedef BIT_STRING_t	 SetVehicleEventFlags_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SetVehicleEventFlags;
asn_struct_free_f SetVehicleEventFlags_free;
asn_struct_print_f SetVehicleEventFlags_print;
asn_constr_check_f SetVehicleEventFlags_constraint;
ber_type_decoder_f SetVehicleEventFlags_decode_ber;
der_type_encoder_f SetVehicleEventFlags_encode_der;
xer_type_decoder_f SetVehicleEventFlags_decode_xer;
xer_type_encoder_f SetVehicleEventFlags_encode_xer;
jer_type_encoder_f SetVehicleEventFlags_encode_jer;
oer_type_decoder_f SetVehicleEventFlags_decode_oer;
oer_type_encoder_f SetVehicleEventFlags_encode_oer;
per_type_decoder_f SetVehicleEventFlags_decode_uper;
per_type_encoder_f SetVehicleEventFlags_encode_uper;
per_type_decoder_f SetVehicleEventFlags_decode_aper;
per_type_encoder_f SetVehicleEventFlags_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SetVehicleEventFlags_H_ */
#include "asn_internal.h"
