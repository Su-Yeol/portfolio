/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetSteeringWheelAngle_H_
#define	_SetSteeringWheelAngle_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SetSteeringWheelAngle */
typedef long	 SetSteeringWheelAngle_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_SetSteeringWheelAngle_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_SetSteeringWheelAngle;
asn_struct_free_f SetSteeringWheelAngle_free;
asn_struct_print_f SetSteeringWheelAngle_print;
asn_constr_check_f SetSteeringWheelAngle_constraint;
ber_type_decoder_f SetSteeringWheelAngle_decode_ber;
der_type_encoder_f SetSteeringWheelAngle_encode_der;
xer_type_decoder_f SetSteeringWheelAngle_decode_xer;
xer_type_encoder_f SetSteeringWheelAngle_encode_xer;
jer_type_encoder_f SetSteeringWheelAngle_encode_jer;
oer_type_decoder_f SetSteeringWheelAngle_decode_oer;
oer_type_encoder_f SetSteeringWheelAngle_encode_oer;
per_type_decoder_f SetSteeringWheelAngle_decode_uper;
per_type_encoder_f SetSteeringWheelAngle_encode_uper;
per_type_decoder_f SetSteeringWheelAngle_decode_aper;
per_type_encoder_f SetSteeringWheelAngle_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SetSteeringWheelAngle_H_ */
#include "asn_internal.h"
