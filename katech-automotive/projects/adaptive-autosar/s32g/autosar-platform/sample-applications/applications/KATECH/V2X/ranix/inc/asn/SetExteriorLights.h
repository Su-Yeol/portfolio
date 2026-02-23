/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetExteriorLights_H_
#define	_SetExteriorLights_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SetExteriorLights {
	SetExteriorLights_lowBeamHeadlightsOn	= 0,
	SetExteriorLights_highBeamHeadlightsOn	= 1,
	SetExteriorLights_leftTurnSignalOn	= 2,
	SetExteriorLights_rightTurnSignalOn	= 3,
	SetExteriorLights_hazardSignalOn	= 4,
	SetExteriorLights_automaticLightControlOn	= 5,
	SetExteriorLights_daytimeRunningLightsOn	= 6,
	SetExteriorLights_fogLightOn	= 7,
	SetExteriorLights_parkingLightsOn	= 8
} e_SetExteriorLights;

/* SetExteriorLights */
typedef BIT_STRING_t	 SetExteriorLights_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SetExteriorLights;
asn_struct_free_f SetExteriorLights_free;
asn_struct_print_f SetExteriorLights_print;
asn_constr_check_f SetExteriorLights_constraint;
ber_type_decoder_f SetExteriorLights_decode_ber;
der_type_encoder_f SetExteriorLights_encode_der;
xer_type_decoder_f SetExteriorLights_decode_xer;
xer_type_encoder_f SetExteriorLights_encode_xer;
jer_type_encoder_f SetExteriorLights_encode_jer;
oer_type_decoder_f SetExteriorLights_decode_oer;
oer_type_encoder_f SetExteriorLights_encode_oer;
per_type_decoder_f SetExteriorLights_decode_uper;
per_type_encoder_f SetExteriorLights_encode_uper;
per_type_decoder_f SetExteriorLights_decode_aper;
per_type_encoder_f SetExteriorLights_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SetExteriorLights_H_ */
#include "asn_internal.h"
