/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_EventDrowsy_H_
#define	_EventDrowsy_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum EventDrowsy {
	EventDrowsy_frontEyeOff	= 0,
	EventDrowsy_drowsyLight	= 1,
	EventDrowsy_drowsySerious	= 2,
	EventDrowsy_faceoff	= 3,
	EventDrowsy_smoking	= 4,
	EventDrowsy_phone	= 5,
	EventDrowsy_yawn	= 6
} e_EventDrowsy;

/* EventDrowsy */
typedef ENUMERATED_t	 EventDrowsy_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_EventDrowsy_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_EventDrowsy;
extern const asn_INTEGER_specifics_t asn_SPC_EventDrowsy_specs_1;
asn_struct_free_f EventDrowsy_free;
asn_struct_print_f EventDrowsy_print;
asn_constr_check_f EventDrowsy_constraint;
ber_type_decoder_f EventDrowsy_decode_ber;
der_type_encoder_f EventDrowsy_encode_der;
xer_type_decoder_f EventDrowsy_decode_xer;
xer_type_encoder_f EventDrowsy_encode_xer;
jer_type_encoder_f EventDrowsy_encode_jer;
oer_type_decoder_f EventDrowsy_decode_oer;
oer_type_encoder_f EventDrowsy_encode_oer;
per_type_decoder_f EventDrowsy_decode_uper;
per_type_encoder_f EventDrowsy_encode_uper;
per_type_decoder_f EventDrowsy_decode_aper;
per_type_encoder_f EventDrowsy_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _EventDrowsy_H_ */
#include "asn_internal.h"
