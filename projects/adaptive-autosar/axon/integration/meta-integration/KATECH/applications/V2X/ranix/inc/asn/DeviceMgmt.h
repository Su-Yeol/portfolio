/*
 * From ASN.1 module "AddGrpD"
 */

#ifndef	_DeviceMgmt_H_
#define	_DeviceMgmt_H_


#include "asn_application.h"

/* Including external dependencies */
#include "BIT_STRING.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum DeviceMgmt {
	DeviceMgmt_noADASSignal	= 0,
	DeviceMgmt_noDrowsySignal	= 1,
	DeviceMgmt_noV2XSignal	= 2,
	DeviceMgmt_noDispplaySignal	= 3,
	DeviceMgmt_noDTGSignal	= 4,
	DeviceMgmt_noGPSSignal	= 5,
	DeviceMgmt_noCITSCenter	= 6,
	DeviceMgmt_nohmistate	= 7
} e_DeviceMgmt;

/* DeviceMgmt */
typedef BIT_STRING_t	 DeviceMgmt_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_DeviceMgmt_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_DeviceMgmt;
asn_struct_free_f DeviceMgmt_free;
asn_struct_print_f DeviceMgmt_print;
asn_constr_check_f DeviceMgmt_constraint;
ber_type_decoder_f DeviceMgmt_decode_ber;
der_type_encoder_f DeviceMgmt_encode_der;
xer_type_decoder_f DeviceMgmt_decode_xer;
xer_type_encoder_f DeviceMgmt_encode_xer;
jer_type_encoder_f DeviceMgmt_encode_jer;
oer_type_decoder_f DeviceMgmt_decode_oer;
oer_type_encoder_f DeviceMgmt_encode_oer;
per_type_decoder_f DeviceMgmt_decode_uper;
per_type_encoder_f DeviceMgmt_encode_uper;
per_type_decoder_f DeviceMgmt_decode_aper;
per_type_encoder_f DeviceMgmt_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _DeviceMgmt_H_ */
#include "asn_internal.h"
