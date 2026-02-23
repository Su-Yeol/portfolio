/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetTransmissionState_H_
#define	_SetTransmissionState_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SetTransmissionState {
	SetTransmissionState_neutral	= 0,
	SetTransmissionState_park	= 1,
	SetTransmissionState_forwardGears	= 2,
	SetTransmissionState_reverseGears	= 3,
	SetTransmissionState_reserved1	= 4,
	SetTransmissionState_reserved2	= 5,
	SetTransmissionState_reserved3	= 6,
	SetTransmissionState_unavailable	= 7
} e_SetTransmissionState;

/* SetTransmissionState */
typedef ENUMERATED_t	 SetTransmissionState_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_SetTransmissionState_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_SetTransmissionState;
extern const asn_INTEGER_specifics_t asn_SPC_SetTransmissionState_specs_1;
asn_struct_free_f SetTransmissionState_free;
asn_struct_print_f SetTransmissionState_print;
asn_constr_check_f SetTransmissionState_constraint;
ber_type_decoder_f SetTransmissionState_decode_ber;
der_type_encoder_f SetTransmissionState_encode_der;
xer_type_decoder_f SetTransmissionState_decode_xer;
xer_type_encoder_f SetTransmissionState_encode_xer;
jer_type_encoder_f SetTransmissionState_encode_jer;
oer_type_decoder_f SetTransmissionState_decode_oer;
oer_type_encoder_f SetTransmissionState_encode_oer;
per_type_decoder_f SetTransmissionState_decode_uper;
per_type_encoder_f SetTransmissionState_encode_uper;
per_type_decoder_f SetTransmissionState_decode_aper;
per_type_encoder_f SetTransmissionState_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SetTransmissionState_H_ */
#include "asn_internal.h"
