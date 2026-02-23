/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_MsgChannel_H_
#define	_MsgChannel_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MsgChannel {
	MsgChannel_waveCV2X	= 0,
	MsgChannel_lte5G	= 1,
	MsgChannel_display	= 2,
	MsgChannel_outVehicle	= 3,
	MsgChannel_inVehicle	= 4,
	MsgChannel_adas	= 5,
	MsgChannel_drowsy	= 6
} e_MsgChannel;

/* MsgChannel */
typedef ENUMERATED_t	 MsgChannel_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_MsgChannel_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_MsgChannel;
extern const asn_INTEGER_specifics_t asn_SPC_MsgChannel_specs_1;
asn_struct_free_f MsgChannel_free;
asn_struct_print_f MsgChannel_print;
asn_constr_check_f MsgChannel_constraint;
ber_type_decoder_f MsgChannel_decode_ber;
der_type_encoder_f MsgChannel_encode_der;
xer_type_decoder_f MsgChannel_decode_xer;
xer_type_encoder_f MsgChannel_encode_xer;
jer_type_encoder_f MsgChannel_encode_jer;
oer_type_decoder_f MsgChannel_decode_oer;
oer_type_encoder_f MsgChannel_encode_oer;
per_type_decoder_f MsgChannel_decode_uper;
per_type_encoder_f MsgChannel_encode_uper;
per_type_decoder_f MsgChannel_decode_aper;
per_type_encoder_f MsgChannel_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _MsgChannel_H_ */
#include "asn_internal.h"
