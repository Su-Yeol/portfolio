/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_ADASOTHERSEventType_H_
#define	_ADASOTHERSEventType_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ADASOTHERSEventType {
	ADASOTHERSEventType_nowarning	= 0,
	ADASOTHERSEventType_pothole	= 1,
	ADASOTHERSEventType_rubberconleft	= 2,
	ADASOTHERSEventType_rubberconright	= 3,
	ADASOTHERSEventType_rubberRightLeft	= 4
} e_ADASOTHERSEventType;

/* ADASOTHERSEventType */
typedef ENUMERATED_t	 ADASOTHERSEventType_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_ADASOTHERSEventType_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_ADASOTHERSEventType;
extern const asn_INTEGER_specifics_t asn_SPC_ADASOTHERSEventType_specs_1;
asn_struct_free_f ADASOTHERSEventType_free;
asn_struct_print_f ADASOTHERSEventType_print;
asn_constr_check_f ADASOTHERSEventType_constraint;
ber_type_decoder_f ADASOTHERSEventType_decode_ber;
der_type_encoder_f ADASOTHERSEventType_encode_der;
xer_type_decoder_f ADASOTHERSEventType_decode_xer;
xer_type_encoder_f ADASOTHERSEventType_encode_xer;
jer_type_encoder_f ADASOTHERSEventType_encode_jer;
oer_type_decoder_f ADASOTHERSEventType_decode_oer;
oer_type_encoder_f ADASOTHERSEventType_encode_oer;
per_type_decoder_f ADASOTHERSEventType_decode_uper;
per_type_encoder_f ADASOTHERSEventType_encode_uper;
per_type_decoder_f ADASOTHERSEventType_decode_aper;
per_type_encoder_f ADASOTHERSEventType_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _ADASOTHERSEventType_H_ */
#include "asn_internal.h"
