/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_LinkID_H_
#define	_LinkID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "CITSText.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LinkID */
typedef CITSText_t	 LinkID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_LinkID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_LinkID;
asn_struct_free_f LinkID_free;
asn_struct_print_f LinkID_print;
asn_constr_check_f LinkID_constraint;
ber_type_decoder_f LinkID_decode_ber;
der_type_encoder_f LinkID_encode_der;
xer_type_decoder_f LinkID_decode_xer;
xer_type_encoder_f LinkID_encode_xer;
jer_type_encoder_f LinkID_encode_jer;
oer_type_decoder_f LinkID_decode_oer;
oer_type_encoder_f LinkID_encode_oer;
per_type_decoder_f LinkID_decode_uper;
per_type_encoder_f LinkID_encode_uper;
per_type_decoder_f LinkID_decode_aper;
per_type_encoder_f LinkID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _LinkID_H_ */
#include "asn_internal.h"
