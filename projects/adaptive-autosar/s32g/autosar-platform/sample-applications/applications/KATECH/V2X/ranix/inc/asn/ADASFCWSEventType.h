/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_ADASFCWSEventType_H_
#define	_ADASFCWSEventType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ADASFCWSEventType {
	ADASFCWSEventType_nowarning	= 0,
	ADASFCWSEventType_bus	= 1,
	ADASFCWSEventType_car	= 2,
	ADASFCWSEventType_truck	= 3,
	ADASFCWSEventType_motors	= 4,
	ADASFCWSEventType_special	= 5,
	ADASFCWSEventType_bicycle	= 6
} e_ADASFCWSEventType;

/* ADASFCWSEventType */
typedef ENUMERATED_t	 ADASFCWSEventType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_ADASFCWSEventType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_ADASFCWSEventType;
extern const asn_INTEGER_specifics_t asn_SPC_ADASFCWSEventType_specs_1;
asn_struct_free_f ADASFCWSEventType_free;
asn_struct_print_f ADASFCWSEventType_print;
asn_constr_check_f ADASFCWSEventType_constraint;
ber_type_decoder_f ADASFCWSEventType_decode_ber;
der_type_encoder_f ADASFCWSEventType_encode_der;
xer_type_decoder_f ADASFCWSEventType_decode_xer;
xer_type_encoder_f ADASFCWSEventType_encode_xer;
jer_type_encoder_f ADASFCWSEventType_encode_jer;
oer_type_decoder_f ADASFCWSEventType_decode_oer;
oer_type_encoder_f ADASFCWSEventType_encode_oer;
per_type_decoder_f ADASFCWSEventType_decode_uper;
per_type_encoder_f ADASFCWSEventType_encode_uper;
per_type_decoder_f ADASFCWSEventType_decode_aper;
per_type_encoder_f ADASFCWSEventType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _ADASFCWSEventType_H_ */
#include "asn_internal.h"
