/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_ADASLDWSEventType_H_
#define	_ADASLDWSEventType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ADASLDWSEventType {
	ADASLDWSEventType_nowarning	= 0,
	ADASLDWSEventType_left	= 1,
	ADASLDWSEventType_right	= 2,
	ADASLDWSEventType_warning	= 3
} e_ADASLDWSEventType;

/* ADASLDWSEventType */
typedef ENUMERATED_t	 ADASLDWSEventType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_ADASLDWSEventType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_ADASLDWSEventType;
extern const asn_INTEGER_specifics_t asn_SPC_ADASLDWSEventType_specs_1;
asn_struct_free_f ADASLDWSEventType_free;
asn_struct_print_f ADASLDWSEventType_print;
asn_constr_check_f ADASLDWSEventType_constraint;
ber_type_decoder_f ADASLDWSEventType_decode_ber;
der_type_encoder_f ADASLDWSEventType_encode_der;
xer_type_decoder_f ADASLDWSEventType_decode_xer;
xer_type_encoder_f ADASLDWSEventType_encode_xer;
jer_type_encoder_f ADASLDWSEventType_encode_jer;
oer_type_decoder_f ADASLDWSEventType_decode_oer;
oer_type_encoder_f ADASLDWSEventType_encode_oer;
per_type_decoder_f ADASLDWSEventType_decode_uper;
per_type_encoder_f ADASLDWSEventType_encode_uper;
per_type_decoder_f ADASLDWSEventType_decode_aper;
per_type_encoder_f ADASLDWSEventType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _ADASLDWSEventType_H_ */
#include "asn_internal.h"
