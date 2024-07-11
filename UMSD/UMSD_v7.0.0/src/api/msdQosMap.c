/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdQosMap.c
*
* DESCRIPTION:
*       API definitions for Quality of Service
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdQosMap.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdQosPortPrioMapSet
*
* DESCRIPTION:
*       This API sets port IEEE Priority Mapping.
*
* INPUTS:
*       devNum      - physical device number
*       portNum		- the logical port number
*       table		- Frame Priority Table set
*						MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr    - Pointer to the desired entry of the IEEE Priority
*						  Mapping table selected above
*       entryStruct	- Mapping data structure for the above selected entry.
*							MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1_STRUCT
*							MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*							MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortPrioMapSet
(
    IN  MSD_U8    devNum,
	IN  MSD_LPORT portNum,
    IN  MSD_IEEE_PRIORITY_TABLE_SET table,
    IN  MSD_U8 entryPtr,
    IN  MSD_IEEE_PRIORITY_TABLE_STRUCT *entryStruct
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap(dev, portNum, table, entryPtr, entryStruct);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortPrioMapGet
*
* DESCRIPTION:
*       This API gets port IEEE Priority Mapping.
*
* INPUTS:
*       devNum      - physical device number
*       portNum		- the logical port number
*       table		- Frame Priority Table set
*						MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr	- Pointer to the desired entry of the IEEE Priority
*					  Mapping table selected above
*
* OUTPUTS:
*       entryStruct	- Mapping data structure for the above selected entry.
*						MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*						MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortPrioMapGet
(
    IN  MSD_U8    devNum,
	IN  MSD_LPORT portNum,
    IN  MSD_IEEE_PRIORITY_TABLE_SET table,
    IN  MSD_U8 entryPtr,
    OUT MSD_IEEE_PRIORITY_TABLE_STRUCT *entryStruct
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap(dev, portNum, table, entryPtr, entryStruct);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortCosToQueueSet
*
* DESCRIPTION:
*       Updates the COS to egress queue mapping.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos value for the packet
*       queue   - Physical queue to be mapped for trusted or assigned cos
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortCosToQueueSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    IN  MSD_U8     queue
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue(dev, portNum, cosVal, queue);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortCosToQueueGet
*
* DESCRIPTION:
*       Gets the physical queue to be mapped for COS.
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos value for the packet
*
* OUTPUTS:
*       queue   - Physical queue to be mapped for trusted or assigned cos
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortCosToQueueGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    OUT MSD_U8     *queue
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue(dev, portNum, cosVal, queue);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortCosToDscpMappingSet
*
* DESCRIPTION:
*       Updates the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*       dscp    - frame differentiated services code point
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortCosToDscpMappingSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
	IN  MSD_U8      cosVal,
    IN  MSD_U8      dscp
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp(dev, portNum, cosVal, dscp);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortCosToDscpMappingGet
*
* DESCRIPTION:
*       Gets the DSCP for IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
*
* OUTPUTS:
*       dscp    - frame differentiated services code point
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortCosToDscpMappingGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    OUT MSD_U8     *dscp
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp(dev, portNum, cosVal, dscp);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDefQPriSet
*
* DESCRIPTION:
*       This routine sets Default queue priority.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       defQPri - default queue priority 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortDefQPriSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U8    defQPri
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri(dev, portNum, defQPri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortDefQPriGet
*
* DESCRIPTION:
*       This routine gets Default queue priority.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*
* OUTPUTS:
*       defQPri - default queue priority 
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortDefQPriGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U8    *defQPri
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri(dev, portNum, defQPri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortDefFPriSet
*
* DESCRIPTION:
*       This routine sets Default queue priority.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       defQPri - default frame priority
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortDefFPriSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U8    defFPri
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri(dev, portNum, defFPri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortDefFPriGet
*
* DESCRIPTION:
*       This routine gets Default queue priority.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*
* OUTPUTS:
*       defFPri - default frame priority 
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortDefFPriGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U8    *defFPri
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri(dev, portNum, defFPri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortPrioRuleSet
*
* DESCRIPTION:
*       This routine sets priority mapping rule(TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6,
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       en      - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortPrioRuleSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_BOOL  en
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine gets priority mapping rule (TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6,
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*
* OUTPUTS:
*       en      - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortPrioRuleGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_BOOL  *en
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortInitialPriSet
*
* DESCRIPTION:
*       This routine sets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            MSD_INITIAL_PRI_DEFAULT
*            MSD_INITIAL_PRI_TAG
*            MSD_INITIAL_PRI_IP
*            MSD_INITIAL_PRI_TAG_IP
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       mode    - MSD_INITIAL_PRI type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortInitialPriSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_INITIAL_PRI  mode
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdPortInitialPriGet
*
* DESCRIPTION:
*       This routine gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            MSD_INITIAL_PRI_DEFAULT
*            MSD_INITIAL_PRI_TAG
*            MSD_INITIAL_PRI_IP
*            MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*
* OUTPUTS:
*       mode    - MSD_INITIAL_PRI type
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortInitialPriGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_INITIAL_PRI  *mode
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdQosPortRewriteDscp
*
* DESCRIPTION:
*       This routine enables or disables rewrite ip dscp at egress. When this is disabled,
*       the packet is given qos treatment within the switch but the DSCP is not modified at the
*       egress. The L2 cos value gets modified as per the classification/trust settings in the switch
*       irrespective of this setting.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       en      - enable/disable dscp re-write at egress on the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdQosPortRewriteDscp
(
    IN MSD_U8     devNum,
    IN MSD_LPORT  portNum,
    IN MSD_BOOL   en
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp != NULL)
		{
			retVal = dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}