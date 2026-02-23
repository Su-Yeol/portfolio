/*
 * From ASN.1 module "DSRC"
 */

#ifndef	_DSRC_Longitude_H_
#define	_DSRC_Longitude_H_


#include "asn_application.h"

/* Including external dependencies */
#include "NativeInteger.h"

#ifdef __cplusplus
extern "C" {
#endif

/* DSRC_Longitude */
typedef long	 DSRC_Longitude_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_DSRC_Longitude_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_DSRC_Longitude;
asn_struct_free_f DSRC_Longitude_free;
asn_struct_print_f DSRC_Longitude_print;
asn_constr_check_f DSRC_Longitude_constraint;
ber_type_decoder_f DSRC_Longitude_decode_ber;
der_type_encoder_f DSRC_Longitude_encode_der;
xer_type_decoder_f DSRC_Longitude_decode_xer;
xer_type_encoder_f DSRC_Longitude_encode_xer;
jer_type_encoder_f DSRC_Longitude_encode_jer;
oer_type_decoder_f DSRC_Longitude_decode_oer;
oer_type_encoder_f DSRC_Longitude_encode_oer;
per_type_decoder_f DSRC_Longitude_decode_uper;
per_type_encoder_f DSRC_Longitude_encode_uper;
per_type_decoder_f DSRC_Longitude_decode_aper;
per_type_encoder_f DSRC_Longitude_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _DSRC_Longitude_H_ */
#include "asn_internal.h"
