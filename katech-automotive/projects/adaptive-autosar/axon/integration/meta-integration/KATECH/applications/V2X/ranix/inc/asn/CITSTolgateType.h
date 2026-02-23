/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSTolgateType_H_
#define	_CITSTolgateType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CITSTolgateType {
	CITSTolgateType_general	= 1,
	CITSTolgateType_hipass	= 2,
	CITSTolgateType_truck_hipass	= 3,
	CITSTolgateType_multi_hipass_2	= 4,
	CITSTolgateType_multi_hipass_3	= 5,
	CITSTolgateType_multi_hipass_4	= 6,
	CITSTolgateType_multi_hipass_5	= 7
} e_CITSTolgateType;

/* CITSTolgateType */
typedef ENUMERATED_t	 CITSTolgateType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_CITSTolgateType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_CITSTolgateType;
extern const asn_INTEGER_specifics_t asn_SPC_CITSTolgateType_specs_1;
asn_struct_free_f CITSTolgateType_free;
asn_struct_print_f CITSTolgateType_print;
asn_constr_check_f CITSTolgateType_constraint;
ber_type_decoder_f CITSTolgateType_decode_ber;
der_type_encoder_f CITSTolgateType_encode_der;
xer_type_decoder_f CITSTolgateType_decode_xer;
xer_type_encoder_f CITSTolgateType_encode_xer;
jer_type_encoder_f CITSTolgateType_encode_jer;
oer_type_decoder_f CITSTolgateType_decode_oer;
oer_type_encoder_f CITSTolgateType_encode_oer;
per_type_decoder_f CITSTolgateType_decode_uper;
per_type_encoder_f CITSTolgateType_encode_uper;
per_type_decoder_f CITSTolgateType_decode_aper;
per_type_encoder_f CITSTolgateType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _CITSTolgateType_H_ */
#include "asn_internal.h"
