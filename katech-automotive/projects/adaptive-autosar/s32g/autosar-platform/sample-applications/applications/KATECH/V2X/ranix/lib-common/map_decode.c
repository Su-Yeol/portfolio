/////////
// Header

#include "r_type.h"
#include "r_debug.h"
#include "r_mem.h"

#include "map_decode.h"

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
//	struct IntersectionGeometryList	*intersections;	/* OPTIONAL */
//	struct RoadSegmentList	*roadSegments;	/* OPTIONAL */
//	struct DataParameters	*dataParameters;	/* OPTIONAL */
//	struct RestrictionClassList	*restrictionList;	/* OPTIONAL */

static r_ret_t show_speed_limit_list(SpeedLimitList_t *speedLimits)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	RegulatorySpeedLimit_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = speedLimits;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("      speedLimits : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("        Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			num = r_asn_num_get(&(asn_list_item->type));
			R_DBG_INFO("          type : %lld\n", num);

			R_DBG_INFO("          speed : %d\n", asn_list_item->speed);
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_lane_type(LaneTypeAttributes_t *laneType)
{
	r_ret_t ret = R_SUCCESS;
	u64 bitmask = 0;

	R_DBG_INFO("              laneType : %d\n", laneType->present);

	switch(laneType->present)
	{
		case LaneTypeAttributes_PR_vehicle:
			bitmask = r_asn_bitmask_get(&(laneType->choice.vehicle));
			R_DBG_INFO("                vehicle : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_crosswalk:
			bitmask = r_asn_bitmask_get(&(laneType->choice.crosswalk));
			R_DBG_INFO("                crosswalk : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_bikeLane:
			bitmask = r_asn_bitmask_get(&(laneType->choice.bikeLane));
			R_DBG_INFO("                bikeLane : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_sidewalk:
			bitmask = r_asn_bitmask_get(&(laneType->choice.sidewalk));
			R_DBG_INFO("                sidewalk : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_median:
			bitmask = r_asn_bitmask_get(&(laneType->choice.median));
			R_DBG_INFO("                median : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_striping:
			bitmask = r_asn_bitmask_get(&(laneType->choice.striping));
			R_DBG_INFO("                striping : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_trackedVehicle:
			bitmask = r_asn_bitmask_get(&(laneType->choice.trackedVehicle));
			R_DBG_INFO("                trackedVehicle : 0x%X\n", bitmask);
			break;

		case LaneTypeAttributes_PR_parking:
			bitmask = r_asn_bitmask_get(&(laneType->choice.parking));
			R_DBG_INFO("                parking : 0x%X\n", bitmask);
			break;

		default:
			ret = R_NOT_SUPPORT;
			break;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_node_offset_point_xy(NodeOffsetPointXY_t *delta)
{
	r_ret_t ret = R_SUCCESS;

	R_DBG_INFO("                  NodeXY\n");
	R_DBG_INFO("                    delta : %d\n", delta->present);

	switch(delta->present)
	{
		case NodeOffsetPointXY_PR_node_XY1:
			R_DBG_INFO("                      node_XY1\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY1.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY1.y);
			break;

		case NodeOffsetPointXY_PR_node_XY2:
			R_DBG_INFO("                      node_XY2\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY2.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY2.y);
			break;

		case NodeOffsetPointXY_PR_node_XY3:
			R_DBG_INFO("                      node_XY3\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY3.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY3.y);
			break;

		case NodeOffsetPointXY_PR_node_XY4:
			R_DBG_INFO("                      node_XY4\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY4.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY4.y);
			break;

		case NodeOffsetPointXY_PR_node_XY5:
			R_DBG_INFO("                      node_XY5\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY5.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY5.y);
			break;

		case NodeOffsetPointXY_PR_node_XY6:
			R_DBG_INFO("                      node_XY6\n");
			R_DBG_INFO("                        x : %d\n", delta->choice.node_XY6.x);
			R_DBG_INFO("                        y : %d\n", delta->choice.node_XY6.y);
			break;

		case NodeOffsetPointXY_PR_node_LatLon:
			R_DBG_INFO("                      node_LatLon\n");
			R_DBG_INFO("                        lat : %d\n", delta->choice.node_LatLon.lat);
			R_DBG_INFO("                        lon : %d\n", delta->choice.node_LatLon.lon);
			break;

		case NodeOffsetPointXY_PR_regional:
			// To Do : delta->choice.regional
			break;

		default:
			ret = R_NOT_SUPPORT;
			break;
	}

	return ret;
}

static r_ret_t show_node_attribute_xy_list(NodeAttributeXYList_t *localNode)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	NodeAttributeXY_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = localNode;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("                  localNode : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                    Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			if (asn_list_item != NULL) {
				num = r_asn_num_get(asn_list_item);
				R_DBG_INFO("                      NodeAttributeXY : %lld\n", num);
			}

		}
	}

	return ret;
}

static r_ret_t show_segment_attribute_xy_list_disabled(SegmentAttributeXYList_t *enabled)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	SegmentAttributeXY_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = enabled;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("                  disabled : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                    Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			if (asn_list_item != NULL) {
				num = r_asn_num_get(asn_list_item);
				R_DBG_INFO("                      SegmentAttributeXY : %lld\n", num);
			}

		}
	}

	return ret;
}

static r_ret_t show_segment_attribute_xy_list_enabled(SegmentAttributeXYList_t *enabled)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	SegmentAttributeXY_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = enabled;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("                  disabled : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                    Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			if (asn_list_item != NULL) {
				num = r_asn_num_get(asn_list_item);
				R_DBG_INFO("                      SegmentAttributeXY : %lld\n", num);
			}

		}
	}

	return ret;
}

static r_ret_t show_lane_data_attribute_list(LaneDataAttributeList_t *data)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	LaneDataAttribute_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = data;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("                  data : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                    Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			R_DBG_INFO("                      LaneDataAttribute : %d\n", asn_list_item->present);

			switch(asn_list_item->present)
			{
				case LaneDataAttribute_PR_pathEndPointAngle:
					R_DBG_INFO("                      pathEndPointAngle : %d\n",
					            asn_list_item->choice.pathEndPointAngle);
					break;

				case LaneDataAttribute_PR_laneCrownPointCenter:
					R_DBG_INFO("                      laneCrownPointCenter : %d\n",
					            asn_list_item->choice.laneCrownPointCenter);
					break;

				case LaneDataAttribute_PR_laneCrownPointLeft:
					R_DBG_INFO("                      laneCrownPointLeft : %d\n",
					            asn_list_item->choice.laneCrownPointLeft);
					break;

				case LaneDataAttribute_PR_laneCrownPointRight:
					R_DBG_INFO("                      laneCrownPointRight : %d\n",
					            asn_list_item->choice.laneCrownPointRight);
					break;

				case LaneDataAttribute_PR_laneAngle:
					R_DBG_INFO("                      laneAngle : %d\n",
					            asn_list_item->choice.laneAngle);
					break;

				case LaneDataAttribute_PR_speedLimits:
					ret = show_speed_limit_list(&(asn_list_item->choice.speedLimits));
					break;

				case LaneDataAttribute_PR_regional:
					// To Do : asn_list_item->choice.regional
					break;

				default:
					break;
			}

		}
	}

	return ret;
}

static r_ret_t show_node_attribute_set_xy(NodeAttributeSetXY_t *attributes)
{
	r_ret_t ret = R_SUCCESS;

	if (attributes->localNode != NULL) {
		ret = show_node_attribute_xy_list(attributes->localNode);
	}

	if (attributes->disabled != NULL) {
		ret = show_segment_attribute_xy_list_disabled(attributes->disabled);
	}

	if (attributes->enabled != NULL) {
		ret = show_segment_attribute_xy_list_enabled(attributes->enabled);
	}

	if (attributes->data != NULL) {
		ret = show_lane_data_attribute_list(attributes->data);
	}

	return ret;
}

static r_ret_t show_node_set_xy(NodeSetXY_t *nodes)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	NodeXY_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = nodes;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("                nodes : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                  Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			ret = show_node_offset_point_xy(&(asn_list_item->delta));

			if (asn_list_item->attributes != NULL) {
				ret = show_node_attribute_set_xy(asn_list_item->attributes);
			}

		}
	}

	return ret;
}

static r_ret_t show_node_list(NodeListXY_t *nodeList)
{
	r_ret_t ret = R_SUCCESS;

	R_DBG_INFO("              nodeList : %d\n", nodeList->present);

	switch(nodeList->present)
	{
		case NodeListXY_PR_nodes:
			ret = show_node_set_xy(&(nodeList->choice.nodes));
			break;

		case NodeListXY_PR_computed:
			// To Do : nodeList->choice.computed
			break;

		default:
			ret = R_NOT_SUPPORT;
			break;
	}

	return ret;
}

static r_ret_t show_connects_to_list(ConnectsToList_t *connectsTo)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	Connection_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;
	u64 bitmask = 0;

	asn_list = connectsTo;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("              connectsTo : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			R_DBG_INFO("                  connectingLang\n");

			R_DBG_INFO("                    lane : %d\n", asn_list_item->connectingLane.lane);
			if (asn_list_item->connectingLane.maneuver != NULL) {
				bitmask = r_asn_bitmask_get(asn_list_item->connectingLane.maneuver);
				R_DBG_INFO("                    maneuver : 0x%X\n", bitmask);
			}

			if (asn_list_item->remoteIntersection != NULL) {
				R_DBG_INFO("                  remoteIntersection\n");

				if (asn_list_item->remoteIntersection->region != NULL) {
					R_DBG_INFO("                    region : %d\n",
					            *(asn_list_item->remoteIntersection->region));
				}

				R_DBG_INFO("                    id : %d\n", asn_list_item->remoteIntersection->id);
			}

			if (asn_list_item->signalGroup != NULL) {
				R_DBG_INFO("                  signalGroup : %d\n", *(asn_list_item->signalGroup));
			}

			if (asn_list_item->userClass != NULL) {
				R_DBG_INFO("                  userClass : %d\n", *(asn_list_item->userClass));
			}

			if (asn_list_item->connectionID != NULL) {
				R_DBG_INFO("                  connectionID : %d\n", *(asn_list_item->connectionID));
			}

		}
	}

	return ret;
}

static r_ret_t show_overlay_lane_list(OverlayLaneList_t *overlays)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	LaneID_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = overlays;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("              overlays : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			R_DBG_INFO("                  laneID : %d\n", *(asn_list_item));
		}
	}

	return ret;
}

static r_ret_t show_lane_list(LaneList_t *laneSet)
{

	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	GenericLane_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;
	s32 len = 0;
	u8 *str = NULL;
	u64 bitmask = 0;

	asn_list = laneSet;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("      laneSet : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("        Item : %d\n", i);
		R_DBG_INFO("          GenericLane\n");

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			R_DBG_INFO("            laneID : %d\n", asn_list_item->laneID);

			if (asn_list_item->name != NULL) {
				len = r_asn_str_get(asn_list_item->name, &str);
				if ((len > 0) && (str != NULL)) {
					R_DBG_INFO("            Name : %s\n", str);
					(void) r_free(str);
				}
			}

			if (asn_list_item->ingressApproach != NULL) {
				R_DBG_INFO("            ingressApproach : %d\n", *(asn_list_item->ingressApproach));
			}
			if (asn_list_item->egressApproach != NULL) {
				R_DBG_INFO("            egressApproach : %d\n", *(asn_list_item->egressApproach));
			}

			R_DBG_INFO("            laneAttributes\n");

			bitmask = r_asn_bitmask_get(&(asn_list_item->laneAttributes.directionalUse));
			R_DBG_INFO("              directionalUse : 0x%X\n", bitmask);
			bitmask = r_asn_bitmask_get(&(asn_list_item->laneAttributes.sharedWith));
			R_DBG_INFO("              sharedWith : 0x%X\n", bitmask);

			ret = show_lane_type(&(asn_list_item->laneAttributes.laneType));

			// To Do : asn_list_item->laneAttributes.regional

			if (asn_list_item->maneuvers != NULL) {
				bitmask = r_asn_bitmask_get(asn_list_item->maneuvers);
				R_DBG_INFO("              maneuvers : 0x%X\n", bitmask);
			}

			ret = show_node_list(&(asn_list_item->nodeList));

			if (asn_list_item->connectsTo != NULL) {
				ret = show_connects_to_list(asn_list_item->connectsTo);
			}

			if (asn_list_item->overlays != NULL) {
				ret = show_overlay_lane_list(asn_list_item->overlays);
			}

			// To Do : asn_list_item->regional
		}
	}

	return ret;
}

static r_ret_t show_preempt_priority_list(PreemptPriorityList_t *preemptPriorityData)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	SignalControlZone_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s64 num = 0;

	asn_list = preemptPriorityData;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("              overlays : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("                Item : %d\n", i);

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {
			R_DBG_INFO("                  laneID : %d\n", *(asn_list_item));
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_intersection_geometry_list(IntersectionGeometryList_t *intersections)
{
	r_ret_t ret = R_FAIL;
	r_asn_list_t *asn_list = NULL;
	IntersectionGeometry_t *asn_list_item = NULL;
	s32 count = 0;
	s32 i = 0;
	s32 len = 0;
//	s64 num = 0;
	u8 *str = NULL;

	asn_list = intersections;
	count = r_asn_list_get_count(asn_list);

	R_DBG_INFO("intersections : %d\n", count);

	for (i = 0; i < count; i++) {
		R_DBG_INFO("  Item : %d\n", i);
		R_DBG_INFO("    IntersectionGeometry\n");

		ret = r_asn_list_get(asn_list, i, (r_asn_list_item_t**) &asn_list_item);
		if ((ret == R_SUCCESS) && (asn_list_item != NULL)) {

			if (asn_list_item->name != NULL) {
				len = r_asn_str_get(asn_list_item->name, &str);
				if ((len > 0) && (str != NULL)) {
					R_DBG_INFO("        Name : %s\n", str);
					(void) r_free(str);
				}
			}

			R_DBG_INFO("      id\n");
			if (asn_list_item->id.region != NULL) {
				R_DBG_INFO("        region : %d\n", *(asn_list_item->id.region));
			}
			R_DBG_INFO("        id : %d\n", asn_list_item->id.id);
			R_DBG_INFO("      revision : %d\n", asn_list_item->revision);

			R_DBG_INFO("      refPoint\n");
			R_DBG_INFO("        lat : %d\n", asn_list_item->refPoint.lat);
			R_DBG_INFO("        long : %d\n", asn_list_item->refPoint.Long);
			if (asn_list_item->refPoint.elevation != NULL) {
				R_DBG_INFO("        elevation : %d\n", *(asn_list_item->refPoint.elevation));
			}
			// To Do : asn_list_item->refPoint.regional

			if (asn_list_item->laneWidth != NULL) {
				R_DBG_INFO("      laneWidth : %d\n", *(asn_list_item->laneWidth));
			}

			if (asn_list_item->speedLimits != NULL) {
				ret = show_speed_limit_list(asn_list_item->speedLimits);
			}

			ret = show_lane_list(&(asn_list_item->laneSet));

//			To Do : asn_list_item->preemptPriorityData
//			if (asn_list_item->preemptPriorityData != NULL) {
//				ret = show_preempt_priority_list(asn_list_item->preemptPriorityData);
//			}

//			To Do : asn_list_item->regional
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////

static r_ret_t show_map_main(MapData_t *map_frame)
{
	r_ret_t ret = R_FAIL;
	u8 *name = NULL;
	s32 len = 0;
	s64 num = 0;

	R_DBG_INFO("--------------------------------\n");

	if (map_frame->timeStamp != NULL) {
		R_DBG_INFO("timeStamp : %lld\n", *(map_frame->timeStamp));
	}

	R_DBG_INFO("msgIssueRevision : %lld\n", map_frame->msgIssueRevision);

	if (map_frame->layerType != NULL) {
		num = r_asn_num_get(map_frame->layerType);
		R_DBG_INFO("layerType : %lld\n", num);
	}

	if (map_frame->layerID != NULL) {
		R_DBG_INFO("layerID : %lld\n", *(map_frame->layerID));
	}

	if (map_frame->intersections != NULL) {
		ret = show_intersection_geometry_list(map_frame->intersections);
	}

// To Do : map_frame->regional

	R_DBG_INFO("--------------------------------\n");

	return ret;
}

//////////////////
// Global function

// 디코딩된 MAP 정보를 이용하여 저장된 내용을 출력 한다.
r_ret_t show_map(r_asn_info_t *info_map)
{
	r_ret_t ret = R_FAIL;
	MessageFrame_t *msg_frame = NULL;

	msg_frame = r_asn_get_strcut_data(info_map);
	if (msg_frame != NULL) {
		if (msg_frame->messageId == DSRCmsgID_mapData) {
			if (msg_frame->value.present == MessageFrame__value_PR_MapData) {

				ret = show_map_main(&(msg_frame->value.choice.MapData));

			}
		}
	}

	return ret;
}
