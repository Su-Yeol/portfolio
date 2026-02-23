/*
 * From ASN.1 module "TCI-2735"
 */

#ifndef	_SetTemporaryID_H_
#define	_SetTemporaryID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "OCTET_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SetTemporaryID */
typedef OCTET_STRING_t	 SetTemporaryID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_SetTemporaryID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_SetTemporaryID;
asn_struct_free_f SetTemporaryID_free;
asn_struct_print_f SetTemporaryID_print;
asn_constr_check_f SetTemporaryID_constraint;
ber_type_decoder_f SetTemporaryID_decode_ber;
der_type_encoder_f SetTemporaryID_encode_der;
xer_type_decoder_f SetTemporaryID_decode_xer;
xer_type_encoder_f SetTemporaryID_encode_xer;
jer_type_encoder_f SetTemporaryID_encode_jer;
oer_type_decoder_f SetTemporaryID_decode_oer;
oer_type_encoder_f SetTemporaryID_encode_oer;
per_type_decoder_f SetTemporaryID_decode_uper;
per_type_encoder_f SetTemporaryID_encode_uper;
per_type_decoder_f SetTemporaryID_decode_aper;
per_type_encoder_f SetTemporaryID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SetTemporaryID_H_ */
#include "asn_internal.h"
