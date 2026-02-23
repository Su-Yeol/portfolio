/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_MsgType_H_
#define	_MsgType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MsgType {
	MsgType_bsm	= 0,
	MsgType_spat	= 1,
	MsgType_map	= 2,
	MsgType_rsa	= 3,
	MsgType_tim	= 4,
	MsgType_adas	= 5,
	MsgType_drowsy	= 6,
	MsgType_psm	= 7
} e_MsgType;

/* MsgType */
typedef ENUMERATED_t	 MsgType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_MsgType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_MsgType;
extern const asn_INTEGER_specifics_t asn_SPC_MsgType_specs_1;
asn_struct_free_f MsgType_free;
asn_struct_print_f MsgType_print;
asn_constr_check_f MsgType_constraint;
ber_type_decoder_f MsgType_decode_ber;
der_type_encoder_f MsgType_encode_der;
xer_type_decoder_f MsgType_decode_xer;
xer_type_encoder_f MsgType_encode_xer;
jer_type_encoder_f MsgType_encode_jer;
oer_type_decoder_f MsgType_decode_oer;
oer_type_encoder_f MsgType_encode_oer;
per_type_decoder_f MsgType_decode_uper;
per_type_encoder_f MsgType_encode_uper;
per_type_decoder_f MsgType_decode_aper;
per_type_encoder_f MsgType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _MsgType_H_ */
#include "asn_internal.h"
