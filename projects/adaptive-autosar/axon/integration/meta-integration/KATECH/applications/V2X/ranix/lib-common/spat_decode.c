/////////
// Header
extern "C" {
#include "r_type.h"
#include "r_debug.h"
#include "r_mem.h"
#include "r_asn.h"

#include "spat_decode.h"

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

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_movement_event_list(MovementEventList_t *state_time_speed)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	MovementEvent_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
	s64 num = 0;
	u8 *str = NULL;

	asn_list = state_time_speed;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("          state_time_speed : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("            Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			num = r_asn_num_get(&(asn_list_item->eventState));
			R_DBG_INFO("                eventState : %d\n", num);

			if (asn_list_item->timing != NULL) {
				if (asn_list_item->timing->startTime != NULL) {
					R_DBG_INFO("                startTime : %d\n",
					            *(asn_list_item->timing->startTime));
				}
				R_DBG_INFO("                minEndTime : %d\n", asn_list_item->timing->minEndTime);
				if (asn_list_item->timing->maxEndTime != NULL) {
					R_DBG_INFO("                maxEndTime : %d\n",
					            *(asn_list_item->timing->maxEndTime));
				}
				if (asn_list_item->timing->likelyTime != NULL) {
					R_DBG_INFO("                likelyTime : %d\n",
					            *(asn_list_item->timing->likelyTime));
				}
				if (asn_list_item->timing->confidence != NULL) {
					R_DBG_INFO("                confidence : %d\n",
					            *(asn_list_item->timing->confidence));
				}
				if (asn_list_item->timing->nextTime != NULL) {
					R_DBG_INFO("                nextTime : %d\n",
					            *(asn_list_item->timing->nextTime));
				}

				// To Do : asn_list_item->speeds
				// To Do : asn_list_item->regional
			}
		}
	}

	return ret;
}

static r_ret_t show_maneuver_assist_list(ManeuverAssistList_t *maneuverAssistList)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	ConnectionManeuverAssist_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
	s64 num = 0;
	u8 *str = NULL;

	asn_list = maneuverAssistList;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("          maneuverAssistList : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("            Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			R_DBG_INFO("                connectionID : %d\n", asn_list_item->connectionID);
			if (asn_list_item->queueLength != NULL) {
				R_DBG_INFO("                  queueLength : %d\n", *(asn_list_item->queueLength));
			}
			if (asn_list_item->availableStorageLength != NULL) {
				R_DBG_INFO("                  availableStorageLength : %d\n",
				            *(asn_list_item->availableStorageLength));
			}
			if (asn_list_item->waitOnStop != NULL) {
				R_DBG_INFO("                waitOnStop : TRUE\n");
				if (*(asn_list_item->waitOnStop) != 0) {
					R_DBG_INFO("                waitOnStop : TRUE\n");
				} else {
					R_DBG_INFO("                waitOnStop : FALSE\n");
				}
			}
			if (asn_list_item->pedBicycleDetect != NULL) {
				if (*(asn_list_item->pedBicycleDetect) != 0) {
					R_DBG_INFO("                pedBicycleDetect : TRUE\n");
				} else {
					R_DBG_INFO("                pedBicycleDetect : FALSE\n");
				}
			}

			// To Do : asn_list_item->regional
		}
	}

	return ret;
}

static r_ret_t show_movement_list(MovementList_t *states)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	MovementState_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
	u8 *str = NULL;

	asn_list = states;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("    states : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("      Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			if (asn_list_item->movementName != NULL) {
				len = r_asn_str_get(asn_list_item->movementName, &str);
				if ((len > 0) && (str != NULL)) {
					R_DBG_INFO("          movementName : %s\n", str);
					(void) r_free(str);
				}
			}

			R_DBG_INFO("          signalGroup : %d\n", asn_list_item->signalGroup);

			ret = show_movement_event_list(&(asn_list_item->state_time_speed));

			if (asn_list_item->maneuverAssistList != NULL) {
				ret = show_maneuver_assist_list(asn_list_item->maneuverAssistList);
			}

			// To Do : asn_list_item->regional
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_enabled_lanes(EnabledLaneList_t *enabledLanes)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	LaneID_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;

	asn_list = enabledLanes;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("    enabledLanes : %d\n", count);

	for (i = 0; i < count; i++) {
		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			R_DBG_INFO("      LaneID : %d\n", *(asn_list_item));
		}
	}

	return ret;
}

static r_ret_t show_intersections(IntersectionStateList_t *intersections)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	IntersectionState_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
	u8 *str = NULL;
	u64 bitmask = 0;

	asn_list = intersections;
	count = r_asn_list_get_count(asn_list);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("\n");
		R_DBG_INFO("Intersections : %d\n", i);
		R_DBG_INFO("  IntersectionState\n");

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			if (asn_list_item->name != NULL) {
				len = r_asn_str_get(asn_list_item->name, &str);
				if ((len > 0) && (str != NULL)) {
					R_DBG_INFO("    Name : %s\n", str);
					(void) r_free(str);
				}
			}

			R_DBG_INFO("    id\n");
			if (asn_list_item->id.region != NULL) {
				R_DBG_INFO("      region : %d\n", *(asn_list_item->id.region));
			}
			R_DBG_INFO("      id : %d\n", asn_list_item->id.id);
			R_DBG_INFO("    revision : %d\n", asn_list_item->revision);

			bitmask = r_asn_bitmask_get(&(asn_list_item->status));
			R_DBG_INFO("    status : 0x%X\n", bitmask);

			if (asn_list_item->moy != NULL) {
				R_DBG_INFO("    moy : %d\n", *(asn_list_item->moy));
			}
			if (asn_list_item->timeStamp != NULL) {
				R_DBG_INFO("    timeStamp : %d\n", *(asn_list_item->timeStamp));
			}

			if (asn_list_item->enabledLanes != NULL) {
				ret = show_enabled_lanes(asn_list_item->enabledLanes);
			}

			ret = show_movement_list(&(asn_list_item->states));

			// To Do : asn_list_item->regional
		}
	}

	return ret;
}

static r_ret_t show_spat_main(SPAT_t *spat_frame)
{
	r_ret_t ret = R_FAIL;
	u8 *name = NULL;
	s32 len = 0;

	R_DBG_INFO("--------------------------------\n");

	if (spat_frame->timeStamp != NULL) {
		R_DBG_INFO("Timestamp : %lld\n", *(spat_frame->timeStamp));
	}

	if (spat_frame->name != NULL) {
		len = r_asn_str_get(spat_frame->name, &name);
		if ((len > 0) && (name != NULL)) {
			R_DBG_INFO("Name : %s\n", name);
			(void) r_free(name);
		}
	}

	ret = show_intersections(&(spat_frame->intersections));

	// To Do : asn_list_item->regional

	R_DBG_INFO("--------------------------------\n");

	return ret;
}

//////////////////
// Global function

// 디코딩된 SPAT 정보를 이용하여 저장된 내용을 출력 한다.
r_ret_t show_spat(r_asn_info_t *info_spat)
{
	r_ret_t ret = R_FAIL;
	MessageFrame_t *msg_frame = NULL;

	msg_frame = r_asn_get_strcut_data(info_spat);
	if (msg_frame != NULL) {
		if (msg_frame->messageId == DSRCmsgID_signalPhaseAndTimingMessage) {
			if (msg_frame->value.present == MessageFrame__value_PR_SPAT) {

				ret = show_spat_main(&(msg_frame->value.choice.SPAT));

			}
		}
	}

	return ret;
}
}