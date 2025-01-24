/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* BonsaiZ1_msdPortPrioMapIntf.c
*
* DESCRIPTION:
*       API definitions for Port Priority Mapping Rules control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsaiz1/include/api/BonsaiZ1_msdPortPrioMap.h>
#include <utils/msdUtils.h>
/*******************************************************************************
* BonsaiZ1_gqosSetPrioMapRule
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
*       port - the logical port number.
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gqosSetPrioMapRuleIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    mode
)
{
	return BonsaiZ1_gqosSetPrioMapRule(dev, port, mode);
}
/*******************************************************************************
* BonsaiZ1_gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine BonsaiZ1_gets priority mapping rule (TagIfBoth bit).
*       If the current frame is both IEEE 802.3ac tagged and an IPv4 or IPv6, 
*       and UserPrioMap (for IEEE 802.3ac) and IPPrioMap (for IP frame) are 
*       enabled, then priority selection is made based on this setup.
*       If PrioMapRule is set to MSD_TRUE, UserPrioMap is used.
*       If PrioMapRule is reset to MSD_FALSE, IPPrioMap is used.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for user prio rule, MSD_FALSE for otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gqosGetPrioMapRuleIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_BOOL   *mode
)
{
	return BonsaiZ1_gqosGetPrioMapRule(dev, port, mode);
}

/*******************************************************************************
* BonsaiZ1_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            BONSAIZ1_MSD_INITIAL_PRI_DEFAULT
*               
*            BONSAIZ1_MSD_INITIAL_PRI_TAG
*                
*            BONSAIZ1_MSD_INITIAL_PRI_IP
*                
*            BONSAIZ1_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - BONSAIZ1_MSD_INITIAL_PRI type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  MSD_INITIAL_PRI  mode
)
{
	return BonsaiZ1_gprtSetInitialPri(dev, port, (BONSAIZ1_MSD_INITIAL_PRI)mode);
}

/*******************************************************************************
* BonsaiZ1_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine BonsaiZ1_gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            BONSAIZ1_MSD_INITIAL_PRI_DEFAULT
*               
*            BONSAIZ1_MSD_INITIAL_PRI_TAG
*                
*            BONSAIZ1_MSD_INITIAL_PRI_IP
*                
*            BONSAIZ1_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - BONSAIZ1_MSD_INITIAL_PRI type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetInitialPriIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_INITIAL_PRI  *mode
)
{
	return BonsaiZ1_gprtGetInitialPri(dev, port, (BONSAIZ1_MSD_INITIAL_PRI*)mode);
}

/*******************************************************************************
* BonsaiZ1_gprtSetDefQpri
*
* DESCRIPTION:
*       This routine Set Default queue priority.
*
* INPUTS:
*       port - the logical port number
*       defque - default queue priority (0~7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetDefQpriIntf
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
)
{
	return BonsaiZ1_gprtSetDefQpri(dev, port, defque);
}

/*******************************************************************************
* BonsaiZ1_gprtGetDefQpri
*
* DESCRIPTION:
*       This routine Get Default queue priority.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       defque - default queue priority structure
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetDefQpriIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
)
{
	return BonsaiZ1_gprtGetDefQpri(dev, port, defque);
}

/*******************************************************************************
* BonsaiZ1_gprtSetIEEEPrioMap
*
* DESCRIPTION:
*       This API set port IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_0
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr		- Pointer to the desired entry of the IEEE Priority  
*						  Mapping table selected above
*       entryStruct	- Mapping data structure for the above selected entry.
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_0_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*							BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetIEEEPrioMapIntf
(
    IN  MSD_QD_DEV	*dev,
    IN  MSD_LPORT	port,
    IN  MSD_IEEE_PRIORITY_TABLE_SET		table,
    IN  MSD_U8							entryPtr,
    IN  MSD_IEEE_PRIORITY_TABLE_STRUCT   *entryStruct
)
{
	BONSAIZ1_PRI_TABLE_STRUCT entry;

	if (NULL == entryStruct)
	{
		MSD_DBG_ERROR(("Input param MSD_IEEE_PRIORITY_TABLE_STRUCT in BonsaiZ1_gprtSetIEEEPrioMapIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.ingTblDei0.disIeeeFpri = entryStruct->ingTbl.disIeeeFpri;
	entry.ingTblDei0.disIeeeQpri = entryStruct->ingTbl.disIeeeQpri;
	entry.ingTblDei0.ieeeFpri = entryStruct->ingTbl.ieeeFpri;
	entry.ingTblDei0.ieeeQpri = entryStruct->ingTbl.ieeeQpri;
	entry.ingTblDei0.ieeeYellow = entryStruct->ingTbl.ieeeYellow;

	return BonsaiZ1_gprtSetIEEEPrioMap(dev, port, (BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_SET)table, entryPtr, &entry);
}


/*******************************************************************************
* BonsaiZ1_gprtGetIEEEPrioMap
*
* DESCRIPTION:
*       This API ge tport IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr	- Pointer to the desired entry of the IEEE Priority  
*					  Mapping table selected above
*
* OUTPUTS:
*       entryStruct	- Mapping data structure for the above selected entry.
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*						BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetIEEEPrioMapIntf
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT    port,
    IN  MSD_IEEE_PRIORITY_TABLE_SET      table,
    IN  MSD_U8							entryPtr,
    OUT MSD_IEEE_PRIORITY_TABLE_STRUCT    *entryStruct
)
{
	BONSAIZ1_PRI_TABLE_STRUCT entry;
	MSD_STATUS    retVal;

	if (NULL == entryStruct)
	{
		MSD_DBG_ERROR(("Input param MSD_IEEE_PRIORITY_TABLE_STRUCT in BonsaiZ1_gprtSetIEEEPrioMapIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(BONSAIZ1_PRI_TABLE_STRUCT));
	retVal = BonsaiZ1_gprtGetIEEEPrioMap(dev, port, (BONSAIZ1_MSD_IEEE_PRIORITY_TABLE_SET)table, entryPtr, &entry);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)entryStruct, 0, sizeof(MSD_IEEE_PRIORITY_TABLE_STRUCT));

	entryStruct->ingTbl.disIeeeFpri = entry.ingTblDei0.disIeeeFpri;
	entryStruct->ingTbl.disIeeeQpri = entry.ingTblDei0.disIeeeQpri;
	entryStruct->ingTbl.ieeeFpri = entry.ingTblDei0.ieeeFpri;
	entryStruct->ingTbl.ieeeQpri = entry.ingTblDei0.ieeeQpri;
	entryStruct->ingTbl.ieeeYellow = entry.ingTblDei0.ieeeYellow;

	return retVal;
}
