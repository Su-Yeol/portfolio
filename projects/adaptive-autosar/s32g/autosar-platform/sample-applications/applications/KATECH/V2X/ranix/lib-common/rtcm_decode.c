/////////
// Header

#include "r_type.h"
#include "r_debug.h"
#include "r_mem.h"
#include "r_asn.h"

#include "rtcm_decode.h"

///////////////////
// Macro definition

//////////////////
// Type Definition

//////////////////
// Extern variable

//////////////////
// Extern function

//////////////////
// Global variable

/////////////////
// Local variable

static r_ret_t show_rtcm_header(RTCMheader_t *rtcmHeader)
{
	r_ret_t ret = R_FAIL;
	u64 bitmask = 0;

	bitmask = r_asn_bitmask_get(&(rtcmHeader->status));
	R_DBG_INFO("  GNSSstatus : 0x%X\n", bitmask);

	R_DBG_INFO("  AntennaOffsetSet\n");
	R_DBG_INFO("    antOffsetX : %d\n", rtcmHeader->offsetSet.antOffsetX);
	R_DBG_INFO("    antOffsetY : %d\n", rtcmHeader->offsetSet.antOffsetY);
	R_DBG_INFO("    antOffsetZ : %d\n", rtcmHeader->offsetSet.antOffsetZ);

	return ret;
}

static r_ret_t show_rtcm_message_list(RTCMmessageList_t *msgs)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	RTCMmessage_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
	u8 *str = NULL;

	asn_list = msgs;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("msgs : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("  Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			len = r_asn_str_get(asn_list_item, &str);
			if ((len > 0) && (str != NULL)) {
				R_DBG_INFO_HEX(str, len, "    RTCMmessage : %d bytes\n", len);
				(void) r_free(str);
			}

		}
	}

	return ret;
}

static r_ret_t show_rtcm_main(RTCMcorrections_t *rtcm_frame)
{
	r_ret_t ret = R_FAIL;
	u8 *name = NULL;
	s32 len = 0;
	s64 num = 0;

	R_DBG_INFO("--------------------------------\n");

	R_DBG_INFO("msgCnt : %d\n", rtcm_frame->msgCnt);

	num = r_asn_num_get(&(rtcm_frame->rev));
	R_DBG_INFO("rev : %lld\n", num);

	if (rtcm_frame->timeStamp != NULL) {
		R_DBG_INFO("timeStamp : %lld\n", *(rtcm_frame->timeStamp));
	}

	if (rtcm_frame->anchorPoint != NULL) {
		// To Do : rtcm_frame->anchorPoint
	}

	if (rtcm_frame->rtcmHeader != NULL) {
		ret = show_rtcm_header(rtcm_frame->rtcmHeader);
	}

	ret = show_rtcm_message_list(&(rtcm_frame->msgs));

	// To Do : rtcm_frame->regional

	R_DBG_INFO("--------------------------------\n");

	return ret;
}

//////////////////
// Global function

// 디코딩된 RTCM 정보를 이용하여 저장된 내용을 출력 한다.
r_ret_t show_rtcm(r_asn_info_t *info_rtcm)
{
	r_ret_t ret = R_FAIL;
	MessageFrame_t *msg_frame = NULL;

	msg_frame = r_asn_get_strcut_data(info_rtcm);
	if (msg_frame != NULL) {
		if (msg_frame->messageId == DSRCmsgID_rtcmCorrections) {
			if (msg_frame->value.present == MessageFrame__value_PR_RTCMcorrections) {

				ret = show_rtcm_main(&(msg_frame->value.choice.RTCMcorrections));

			}
		}
	}

	return ret;
}
