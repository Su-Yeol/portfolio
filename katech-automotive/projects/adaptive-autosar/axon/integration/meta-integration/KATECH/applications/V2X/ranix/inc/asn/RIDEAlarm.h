/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_RIDEAlarm_H_
#define	_RIDEAlarm_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum RIDEAlarm {
	RIDEAlarm_none	= 0,
	RIDEAlarm_geton	= 1,
	RIDEAlarm_getoff	= 2,
	RIDEAlarm_anADAuse	= 3
} e_RIDEAlarm;

/* RIDEAlarm */
typedef ENUMERATED_t	 RIDEAlarm_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_RIDEAlarm_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_RIDEAlarm;
extern const asn_INTEGER_specifics_t asn_SPC_RIDEAlarm_specs_1;
asn_struct_free_f RIDEAlarm_free;
asn_struct_print_f RIDEAlarm_print;
asn_constr_check_f RIDEAlarm_constraint;
ber_type_decoder_f RIDEAlarm_decode_ber;
der_type_encoder_f RIDEAlarm_encode_der;
xer_type_decoder_f RIDEAlarm_decode_xer;
xer_type_encoder_f RIDEAlarm_encode_xer;
jer_type_encoder_f RIDEAlarm_encode_jer;
oer_type_decoder_f RIDEAlarm_decode_oer;
oer_type_encoder_f RIDEAlarm_encode_oer;
per_type_decoder_f RIDEAlarm_decode_uper;
per_type_encoder_f RIDEAlarm_encode_uper;
per_type_decoder_f RIDEAlarm_decode_aper;
per_type_encoder_f RIDEAlarm_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _RIDEAlarm_H_ */
#include "asn_internal.h"
