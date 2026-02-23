/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_ADASPCWSEventType_H_
#define	_ADASPCWSEventType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ADASPCWSEventType {
	ADASPCWSEventType_nowarning	= 0,
	ADASPCWSEventType_pedestrian	= 1
} e_ADASPCWSEventType;

/* ADASPCWSEventType */
typedef ENUMERATED_t	 ADASPCWSEventType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_ADASPCWSEventType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_ADASPCWSEventType;
extern const asn_INTEGER_specifics_t asn_SPC_ADASPCWSEventType_specs_1;
asn_struct_free_f ADASPCWSEventType_free;
asn_struct_print_f ADASPCWSEventType_print;
asn_constr_check_f ADASPCWSEventType_constraint;
ber_type_decoder_f ADASPCWSEventType_decode_ber;
der_type_encoder_f ADASPCWSEventType_encode_der;
xer_type_decoder_f ADASPCWSEventType_decode_xer;
xer_type_encoder_f ADASPCWSEventType_encode_xer;
jer_type_encoder_f ADASPCWSEventType_encode_jer;
oer_type_decoder_f ADASPCWSEventType_decode_oer;
oer_type_encoder_f ADASPCWSEventType_encode_oer;
per_type_decoder_f ADASPCWSEventType_decode_uper;
per_type_encoder_f ADASPCWSEventType_encode_uper;
per_type_decoder_f ADASPCWSEventType_decode_aper;
per_type_encoder_f ADASPCWSEventType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _ADASPCWSEventType_H_ */
#include "asn_internal.h"
