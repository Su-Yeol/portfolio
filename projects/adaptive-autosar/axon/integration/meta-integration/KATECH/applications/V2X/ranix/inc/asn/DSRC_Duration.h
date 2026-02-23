/*
 * From ASN.1 module "DSRC"
 */

#ifndef	_DSRC_Duration_H_
#define	_DSRC_Duration_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DSRC_Duration */
typedef long	 DSRC_Duration_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DSRC_Duration;
asn_struct_free_f DSRC_Duration_free;
asn_struct_print_f DSRC_Duration_print;
asn_constr_check_f DSRC_Duration_constraint;
ber_type_decoder_f DSRC_Duration_decode_ber;
der_type_encoder_f DSRC_Duration_encode_der;
xer_type_decoder_f DSRC_Duration_decode_xer;
xer_type_encoder_f DSRC_Duration_encode_xer;
jer_type_encoder_f DSRC_Duration_encode_jer;
oer_type_decoder_f DSRC_Duration_decode_oer;
oer_type_encoder_f DSRC_Duration_encode_oer;
per_type_decoder_f DSRC_Duration_decode_uper;
per_type_encoder_f DSRC_Duration_encode_uper;
per_type_decoder_f DSRC_Duration_decode_aper;
per_type_encoder_f DSRC_Duration_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _DSRC_Duration_H_ */
#include "asn_internal.h"
