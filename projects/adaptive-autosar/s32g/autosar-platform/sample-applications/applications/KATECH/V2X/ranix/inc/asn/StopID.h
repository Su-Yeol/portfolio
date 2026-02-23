/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_StopID_H_
#define	_StopID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "CITSText.h"

#ifdef __cplusplus
extern "C" {
#endif

/* StopID */
typedef CITSText_t	 StopID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_StopID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_StopID;
asn_struct_free_f StopID_free;
asn_struct_print_f StopID_print;
asn_constr_check_f StopID_constraint;
ber_type_decoder_f StopID_decode_ber;
der_type_encoder_f StopID_encode_der;
xer_type_decoder_f StopID_decode_xer;
xer_type_encoder_f StopID_encode_xer;
jer_type_encoder_f StopID_encode_jer;
oer_type_decoder_f StopID_decode_oer;
oer_type_encoder_f StopID_encode_oer;
per_type_decoder_f StopID_decode_uper;
per_type_encoder_f StopID_encode_uper;
per_type_decoder_f StopID_decode_aper;
per_type_encoder_f StopID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _StopID_H_ */
#include "asn_internal.h"
