/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_IntersectionName_H_
#define	_IntersectionName_H_


#include "asn_application.h"

/* Including external dependencies */
#include "CITSText.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IntersectionName */
typedef CITSText_t	 IntersectionName_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_IntersectionName;
asn_struct_free_f IntersectionName_free;
asn_struct_print_f IntersectionName_print;
asn_constr_check_f IntersectionName_constraint;
ber_type_decoder_f IntersectionName_decode_ber;
der_type_encoder_f IntersectionName_encode_der;
xer_type_decoder_f IntersectionName_decode_xer;
xer_type_encoder_f IntersectionName_encode_xer;
jer_type_encoder_f IntersectionName_encode_jer;
oer_type_decoder_f IntersectionName_decode_oer;
oer_type_encoder_f IntersectionName_encode_oer;
per_type_decoder_f IntersectionName_decode_uper;
per_type_encoder_f IntersectionName_encode_uper;
per_type_decoder_f IntersectionName_decode_aper;
per_type_encoder_f IntersectionName_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _IntersectionName_H_ */
#include "asn_internal.h"
