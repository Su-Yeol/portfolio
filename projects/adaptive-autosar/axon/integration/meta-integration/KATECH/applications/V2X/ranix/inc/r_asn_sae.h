#ifndef R_ASN_SAE_H
#define R_ASN_SAE_H

/////////
// Header

#include "r_type.h"

#include "MessageFrame.h"

///////////////////
// Macro definition

//////////////////
// Type definition

/**
 * @enum r_asn_codec_pdu_sae_t_t r_asn.h
 * @brief ASN Codec 에서 사용 하는 SAE 규격의 PDU Type
 * @ingroup ASN1
 */
typedef enum
{
	R_ASN_PDU_SAE_START = 150,

	// SAE J2735
	R_ASN_PDU_MessageFrame,					//!< MessageFrame : SAE J2735

} r_asn_codec_pdu_sae_t;

/////////////////
// Extern variable

//////////////////
// Extern function

#endif /* R_ASN_SAE_H */
