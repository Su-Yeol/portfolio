/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_DispLocation_H_
#define	_DispLocation_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum DispLocation {
	DispLocation_pass	= 0,
	DispLocation_entry	= 1,
	DispLocation_before_entry	= 2,
	DispLocation_timIn	= 10,
	DispLocation_timOut	= 11,
	DispLocation_ttcZone0	= 20,
	DispLocation_ttcZone1	= 21,
	DispLocation_ttcZone2	= 22,
	DispLocation_ttcZone3	= 23,
	DispLocation_before_zone	= 24,
	DispLocation_alert	= 30,
	DispLocation_approach	= 31,
	DispLocation_approach1	= 32,
	DispLocation_before_approach	= 33,
	DispLocation_others	= 99
} e_DispLocation;

/* DispLocation */
typedef ENUMERATED_t	 DispLocation_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_DispLocation_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_DispLocation;
extern const asn_INTEGER_specifics_t asn_SPC_DispLocation_specs_1;
asn_struct_free_f DispLocation_free;
asn_struct_print_f DispLocation_print;
asn_constr_check_f DispLocation_constraint;
ber_type_decoder_f DispLocation_decode_ber;
der_type_encoder_f DispLocation_encode_der;
xer_type_decoder_f DispLocation_decode_xer;
xer_type_encoder_f DispLocation_encode_xer;
jer_type_encoder_f DispLocation_encode_jer;
oer_type_decoder_f DispLocation_decode_oer;
oer_type_encoder_f DispLocation_encode_oer;
per_type_decoder_f DispLocation_decode_uper;
per_type_encoder_f DispLocation_encode_uper;
per_type_decoder_f DispLocation_decode_aper;
per_type_encoder_f DispLocation_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _DispLocation_H_ */
#include "asn_internal.h"
