/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_EnableCongestionMitigation_H_
#define	_EnableCongestionMitigation_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BOOLEAN.h"

#ifdef __cplusplus
extern "C" {
#endif

/* EnableCongestionMitigation */
typedef BOOLEAN_t	 EnableCongestionMitigation_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EnableCongestionMitigation;
asn_struct_free_f EnableCongestionMitigation_free;
asn_struct_print_f EnableCongestionMitigation_print;
asn_constr_check_f EnableCongestionMitigation_constraint;
ber_type_decoder_f EnableCongestionMitigation_decode_ber;
der_type_encoder_f EnableCongestionMitigation_encode_der;
xer_type_decoder_f EnableCongestionMitigation_decode_xer;
xer_type_encoder_f EnableCongestionMitigation_encode_xer;
jer_type_encoder_f EnableCongestionMitigation_encode_jer;
oer_type_decoder_f EnableCongestionMitigation_decode_oer;
oer_type_encoder_f EnableCongestionMitigation_encode_oer;
per_type_decoder_f EnableCongestionMitigation_decode_uper;
per_type_encoder_f EnableCongestionMitigation_encode_uper;
per_type_decoder_f EnableCongestionMitigation_decode_aper;
per_type_encoder_f EnableCongestionMitigation_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _EnableCongestionMitigation_H_ */
#include "asn_internal.h"
