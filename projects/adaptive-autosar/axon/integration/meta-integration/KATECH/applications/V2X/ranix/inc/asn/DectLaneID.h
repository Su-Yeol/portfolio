/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_DectLaneID_H_
#define	_DectLaneID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "ENUMERATED.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum DectLaneID {
	DectLaneID_unavailable	= 0,
	DectLaneID_busOnlyLane	= 10,
	DectLaneID_ctrBusLane	= 11,
	DectLaneID_rdSdBusLane	= 12,
	DectLaneID_mergingArea	= 13,
	DectLaneID_accelerationLane	= 14,
	DectLaneID_decelrationLane	= 15,
	DectLaneID_rampLane	= 16,
	DectLaneID_shoulderLane	= 17,
	DectLaneID_carLane	= 20,
	DectLaneID_car_1stLane	= 21,
	DectLaneID_car_2ndLane	= 22,
	DectLaneID_car_3rdLane	= 23,
	DectLaneID_car_4thLane	= 24,
	DectLaneID_car_5thLane	= 25,
	DectLaneID_car_6thLane	= 26,
	DectLaneID_car_7thLane	= 27,
	DectLaneID_car_8thLane	= 28,
	DectLaneID_car_9thLane	= 29,
	DectLaneID_car_10thLane	= 30,
	DectLaneID_car_11thLane	= 31,
	DectLaneID_car_12thLane	= 32,
	DectLaneID_car_13thLane	= 33,
	DectLaneID_car_14thLane	= 34,
	DectLaneID_car_15thLane	= 35,
	DectLaneID_car_16thLane	= 36,
	DectLaneID_car_17thLane	= 37,
	DectLaneID_car_18thLane	= 38,
	DectLaneID_car_19thLane	= 39,
	DectLaneID_car_20thLane	= 40,
	DectLaneID_etcLane	= 99
} e_DectLaneID;

/* DectLaneID */
typedef ENUMERATED_t	 DectLaneID_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_DectLaneID_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_DectLaneID;
extern const asn_INTEGER_specifics_t asn_SPC_DectLaneID_specs_1;
asn_struct_free_f DectLaneID_free;
asn_struct_print_f DectLaneID_print;
asn_constr_check_f DectLaneID_constraint;
ber_type_decoder_f DectLaneID_decode_ber;
der_type_encoder_f DectLaneID_encode_der;
xer_type_decoder_f DectLaneID_decode_xer;
xer_type_encoder_f DectLaneID_encode_xer;
jer_type_encoder_f DectLaneID_encode_jer;
oer_type_decoder_f DectLaneID_decode_oer;
oer_type_encoder_f DectLaneID_encode_oer;
per_type_decoder_f DectLaneID_decode_uper;
per_type_encoder_f DectLaneID_encode_uper;
per_type_decoder_f DectLaneID_decode_aper;
per_type_encoder_f DectLaneID_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _DectLaneID_H_ */
#include "asn_internal.h"
