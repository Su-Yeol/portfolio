/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_CITSText_H_
#define	_CITSText_H_


#include "asn_application.h"

/* Including external dependencies */
#include "UTF8String.h"

#ifdef __cplusplus
extern "C" {
#endif

/* CITSText */
typedef UTF8String_t	 CITSText_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_CITSText_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_CITSText;
asn_struct_free_f CITSText_free;
asn_struct_print_f CITSText_print;
asn_constr_check_f CITSText_constraint;
ber_type_decoder_f CITSText_decode_ber;
der_type_encoder_f CITSText_encode_der;
xer_type_decoder_f CITSText_decode_xer;
xer_type_encoder_f CITSText_encode_xer;
jer_type_encoder_f CITSText_encode_jer;
oer_type_decoder_f CITSText_decode_oer;
oer_type_encoder_f CITSText_encode_oer;
per_type_decoder_f CITSText_decode_uper;
per_type_encoder_f CITSText_encode_uper;
per_type_decoder_f CITSText_decode_aper;
per_type_encoder_f CITSText_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _CITSText_H_ */
#include "asn_internal.h"
