/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Fir_msdPortPrioMap.c
*
* DESCRIPTION:
*       API definitions for Port Priority Mapping Rules control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <fir/include/api/Fir_msdPortPrioMap.h>
#include <fir/include/api/Fir_msdApiInternal.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


/*******************************************************************************
* Fir_gqosSetPrioMapRule
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
MSD_STATUS Fir_gqosSetPrioMapRule
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   port,
    IN  MSD_BOOL    mode
)
{
    MSD_U16          data;           /* temporary data buffer */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gqosSetPrioMapRule Called.\n"));
    /* translate bool to binary */
	MSD_BOOL_2_BIT(mode, data);
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Set the TagIfBoth.  */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)6, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("write FIR_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gqosSetPrioMapRule Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gqosGetPrioMapRule
*
* DESCRIPTION:
*       This routine Fir_gets priority mapping rule (TagIfBoth bit).
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
MSD_STATUS Fir_gqosGetPrioMapRule
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_BOOL   *mode
)
{
    MSD_U16          data;           /* Used to poll the SWReset bit */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gqosGetPrioMapRule Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Fir_get the TagIfBoth.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)6, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* translate bool to binary */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gqosGetPrioMapRule Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            FIR_MSD_INITIAL_PRI_DEFAULT
*               
*            FIR_MSD_INITIAL_PRI_TAG
*                
*            FIR_MSD_INITIAL_PRI_IP
*                
*            FIR_MSD_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - FIR_MSD_INITIAL_PRI type
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
MSD_STATUS Fir_gprtSetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    IN  FIR_MSD_INITIAL_PRI  mode
)
{
    MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
    MSD_U8           hwPort;           /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data = 0;

    MSD_DBG_INFO(("Fir_gprtSetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if ((mode < FIR_MSD_INITIAL_PRI_DEFAULT) || (mode > FIR_MSD_INITIAL_PRI_TAG_IP))
		{
			MSD_DBG_ERROR(("Bad mode: %u.\n", mode));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			switch (mode)
			{
			case FIR_MSD_INITIAL_PRI_DEFAULT:
				data = 0;
				break;
			case FIR_MSD_INITIAL_PRI_TAG:
				data = 1U;
				break;
			case FIR_MSD_INITIAL_PRI_IP:
				data = 2U;
				break;
			case FIR_MSD_INITIAL_PRI_TAG_IP:
				data = 3U;
				break;
			default:
				MSD_DBG_ERROR(("Failed Bad Mode: %u.\n", mode));
				retVal = MSD_BAD_PARAM;
				break;
			}

			if (retVal != MSD_BAD_PARAM)
			{
				/* Set the FIR_MSD_INITIAL_PRI mode.            */
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)4, (MSD_U8)2, data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("write FIR_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetInitialPri Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetInitialPri
*
* DESCRIPTION:
*       This routine Fir_gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            FIR_MSD_INITIAL_PRI_DEFAULT
*               
*            FIR_MSD_INITIAL_PRI_TAG
*                
*            FIR_MSD_INITIAL_PRI_IP
*                
*            FIR_MSD_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - FIR_MSD_INITIAL_PRI type
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
MSD_STATUS Fir_gprtGetInitialPri
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT FIR_MSD_INITIAL_PRI  *mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetInitialPri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Fir_get the FIR_MSD_INITIAL_PRI type            */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)4, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("read FIR_PORT_CONTROL Register return %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			switch (data)
			{
			case 0x0:
				*mode = FIR_MSD_INITIAL_PRI_DEFAULT;
				break;
			case 0x1U:
				*mode = FIR_MSD_INITIAL_PRI_TAG;
				break;
			case 0x2U:
				*mode = FIR_MSD_INITIAL_PRI_IP;
				break;
			case 0x3U:
				*mode = FIR_MSD_INITIAL_PRI_TAG_IP;
				break;
			default:
				MSD_DBG_ERROR(("Failed (get the FIR_MSD_INITIAL_PRI type fail).\n"));
				retVal = MSD_FAIL;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetInitialPri Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetDefQpri
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
MSD_STATUS Fir_gprtSetDefQpri
(
  IN  MSD_QD_DEV    *dev,
  IN  MSD_LPORT    port,
  IN  MSD_U8  	  defque
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
	MSD_U16		     data;

	MSD_DBG_INFO(("Fir_gprtSetDefQpri Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (defque > (MSD_U8)7)
		{
			MSD_DBG_ERROR(("Bad defque: %u.\n", defque));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			data = (MSD_U16)((MSD_U16)defque & (MSD_U16)0x7);

			/* Set the default queue priority.               */
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)0, (MSD_U8)3, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("write FIR_QD_REG_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDefQpri Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetDefQpri
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
MSD_STATUS Fir_gprtGetDefQpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_U8       *defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data;

    MSD_DBG_INFO(("Fir_gprtGetDefQpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Set the default queue priority.               */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)0, (MSD_U8)3, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("read FIR_PORT_CONTROL2 Register return %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			*defque = msdShort2Char(data & (MSD_U16)0x7);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDefQpri Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetIEEEPrioMap
*
* DESCRIPTION:
*       This API set port IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr		- Pointer to the desired entry of the IEEE Priority  
*						  Mapping table selected above
*       entryStruct	- Mapping data structure for the above selected entry.
*							FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*							FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
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
MSD_STATUS Fir_gprtSetIEEEPrioMap
(
    IN  MSD_QD_DEV	*dev,
    IN  MSD_LPORT	port,
    IN  FIR_MSD_IEEE_PRIORITY_TABLE_SET		table,
    IN  MSD_U8							entryPtr,
    IN  FIR_PRI_TABLE_STRUCT   *entryStruct
)
{
    MSD_STATUS     retVal;         /* Functions return value.      */
    MSD_U16        data, data1;
    MSD_U16        ptr;
    MSD_U8         hwPort;         /* the physical port number     */
	MSD_U8		   phyAddr;
	MSD_U16        count = (MSD_U16)0x10;

    MSD_DBG_INFO(("Fir_gprtSetIEEEPrioMap Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		return MSD_BAD_PARAM;
	}
	if ((table < FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0) || (table > FIR_PRI_EG_AVB_FPRI_2_DSCP))
    {
        MSD_DBG_ERROR(("Bad table: %u.\n", table));
        return MSD_BAD_PARAM;
    }

	ptr = (MSD_U16)((MSD_U16)(((MSD_U16)entryPtr & (MSD_U16)0x7) << 9) | (MSD_U16)(((MSD_U16)table & (MSD_U16)0x7) << 12));

	data = 0;

	switch(table)
	{
		case FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0:
			MSD_BF_SET(data, entryStruct->ingTblDei0.ieeeYellow, 8, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei0.disIeeeQpri, 7, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei0.ieeeQpri, 4, 3);
			MSD_BF_SET(data, entryStruct->ingTblDei0.disIeeeFpri, 3, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei0.ieeeFpri, 0, 3);
			break;
		case FIR_PRI_EG_GREEN_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.disVIDMarking, 4, 1);
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.disPCPMarking, 3, 1);
			MSD_BF_SET(data,entryStruct->egGreeTblPcp.framePCP, 0, 3);
			break;
		case FIR_PRI_EG_YEL_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egYelTblPcp.disVIDMarking, 4, 1);
			MSD_BF_SET(data,entryStruct->egYelTblPcp.disPCPMarking, 3, 1);
			MSD_BF_SET(data,entryStruct->egYelTblPcp.framePCP, 0, 3);
			break;
		case FIR_PRI_EG_AVB_FPRI_2_PCP:
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.disVIDMarking, 4, 1);
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.disPCPMarking, 3, 1);
			MSD_BF_SET(data,entryStruct->egAvbTblPcp.framePCP, 0, 3);
			break;
		case FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_1:
			MSD_BF_SET(data, entryStruct->ingTblDei1.ieeeYellow, 8, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei1.disIeeeQpri, 7, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei1.ieeeQpri, 4, 3);
			MSD_BF_SET(data, entryStruct->ingTblDei1.disIeeeFpri, 3, 1);
			MSD_BF_SET(data, entryStruct->ingTblDei1.ieeeFpri, 0, 3);
			break;
		case FIR_PRI_EG_GREEN_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egGreeTblDscp.enDSCPMarking, 6, 1);
			MSD_BF_SET(data,entryStruct->egGreeTblDscp.frameDSCP, 0, 6);
			break;
		case FIR_PRI_EG_YEL_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egYelTblDscp.enDSCPMarking, 6, 1);
			MSD_BF_SET(data,entryStruct->egYelTblDscp.frameDSCP, 0, 6);
			break;
		case FIR_PRI_EG_AVB_FPRI_2_DSCP:
			MSD_BF_SET(data,entryStruct->egAvbTbldscp.enDSCPMarking, 6, 1);
			MSD_BF_SET(data,entryStruct->egAvbTbldscp.frameDSCP, 0, 6);
			break;
        default:
            MSD_DBG_ERROR(("Bad table: %u.\n", (MSD_U16)table));
            return MSD_BAD_PARAM;
			break;
	}

	data |= ptr;

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
    do    
    {
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_IEEE_PRI_MAPPING_TBL, (MSD_U8)15, (MSD_U8)1, &data1);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            MSD_DBG_ERROR(("read FIR_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
            return retVal;
        }
		if (count == (MSD_U16)0)
		{
            msdSemGive(dev->devNum, dev->tblRegsSem);
            MSD_DBG_ERROR(("Failed(table always busy).\n"));
			return MSD_FAIL;
		}
		count -= 1U;

	} while (data1 == (MSD_U16)1);

	data = data | (MSD_U16)0x8000;

	retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_IEEE_PRI_MAPPING_TBL, data);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write FIR_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->tblRegsSem);

    MSD_DBG_INFO(("Fir_gprtSetIEEEPrioMap Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Fir_gprtGetIEEEPrioMap
*
* DESCRIPTION:
*       This API ge tport IEEE Priority Mapping.
*
* INPUTS:
*       port		- the logical port number
*       table		- Frame Priority Table set
*						FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_0
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*                       FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*       entryPtr	- Pointer to the desired entry of the IEEE Priority  
*					  Mapping table selected above
*
* OUTPUTS:
*       entryStruct	- Mapping data structure for the above selected entry.
*						FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*						FIR_MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
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
MSD_STATUS Fir_gprtGetIEEEPrioMap
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT    port,
    IN  FIR_MSD_IEEE_PRIORITY_TABLE_SET      table,
    IN  MSD_U8							entryPtr,
    OUT FIR_PRI_TABLE_STRUCT    *entryStruct
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16       data;
    MSD_U16       ptr;
    MSD_U8        hwPort;         /* the physical port number     */
	MSD_U8		  phyAddr;
	MSD_U16       count = (MSD_U16)0x10;

    MSD_DBG_INFO(("Fir_gprtGetIpPrioMap Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_INFO(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    if ((table < FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0) || (table > FIR_PRI_EG_AVB_FPRI_2_DSCP))
    {
        MSD_DBG_ERROR(("Bad table: %u.\n", table));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
    do    
    {
		retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_IEEE_PRI_MAPPING_TBL, &data);
		if(retVal != MSD_OK)
		{	
            MSD_DBG_ERROR(("read FIR_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if (count == (MSD_U16)0)
		{
            MSD_DBG_ERROR(("Read FIR_IEEE_PRI_MAPPING_TBL register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((data & (MSD_U16)0x8000) != 0U);

	/* Write table + pointer */
	ptr = (MSD_U16)((MSD_U16)(((MSD_U16)entryPtr & (MSD_U16)0x7) << 9) | (MSD_U16)(((MSD_U16)table & (MSD_U16)0x7) << 12));

	data = ptr;
	retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_IEEE_PRI_MAPPING_TBL, data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->tblRegsSem);
        MSD_DBG_ERROR(("write FIR_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	/* read-back value */
	retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_IEEE_PRI_MAPPING_TBL, &data);
	if(retVal != MSD_OK)
	{
        msdSemGive(dev->devNum, dev->tblRegsSem);
        MSD_DBG_ERROR(("read FIR_IEEE_PRI_MAPPING_TBL Register return %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    msdSemGive(dev->devNum, dev->tblRegsSem);

	switch(table)
	{
		case FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0:			
			entryStruct->ingTblDei0.ieeeYellow = (MSD_U8)(MSD_BF_GET(data, 8, 1));
			entryStruct->ingTblDei0.disIeeeQpri = (MSD_U8)(MSD_BF_GET(data, 7, 1));
			entryStruct->ingTblDei0.ieeeQpri = (MSD_U8)(MSD_BF_GET(data, 4, 3));
			entryStruct->ingTblDei0.disIeeeFpri = (MSD_U8)(MSD_BF_GET(data, 3, 1));
			entryStruct->ingTblDei0.ieeeFpri = (MSD_U8)(MSD_BF_GET(data, 0, 3));
			break;
		case FIR_PRI_EG_GREEN_FPRI_2_PCP:
			entryStruct->egGreeTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egGreeTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egGreeTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case FIR_PRI_EG_YEL_FPRI_2_PCP:
			entryStruct->egYelTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egYelTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egYelTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case FIR_PRI_EG_AVB_FPRI_2_PCP:
			entryStruct->egAvbTblPcp.disVIDMarking = (MSD_U8)(MSD_BF_GET(data,4,1));
			entryStruct->egAvbTblPcp.disPCPMarking = (MSD_U8)(MSD_BF_GET(data,3,1));
			entryStruct->egAvbTblPcp.framePCP = (MSD_U8)(MSD_BF_GET(data,0,3));
			break;
		case FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_1:
			entryStruct->ingTblDei1.ieeeYellow = (MSD_U8)(MSD_BF_GET(data, 8, 1));
			entryStruct->ingTblDei1.disIeeeQpri = (MSD_U8)(MSD_BF_GET(data, 7, 1));
			entryStruct->ingTblDei1.ieeeQpri = (MSD_U8)(MSD_BF_GET(data, 4, 3));
			entryStruct->ingTblDei1.disIeeeFpri = (MSD_U8)(MSD_BF_GET(data, 3, 1));
			entryStruct->ingTblDei1.ieeeFpri = (MSD_U8)(MSD_BF_GET(data, 0, 3));
			break;
		case FIR_PRI_EG_GREEN_FPRI_2_DSCP:
			entryStruct->egGreeTblDscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egGreeTblDscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
		case FIR_PRI_EG_YEL_FPRI_2_DSCP:
			entryStruct->egYelTblDscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egYelTblDscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
		case FIR_PRI_EG_AVB_FPRI_2_DSCP:
			entryStruct->egAvbTbldscp.enDSCPMarking = (MSD_U8)(MSD_BF_GET(data,6,1));
			entryStruct->egAvbTbldscp.frameDSCP = (MSD_U8)(MSD_BF_GET(data,0,6));
			break;
        default:
            MSD_DBG_ERROR(("Bad table: %u.\n", table));
            return MSD_BAD_PARAM;
			break;
	}

    MSD_DBG_INFO(("Fir_gprtGetIpPrioMap Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Fir_gprtRewriteDscp
*
* DESCRIPTION:
*       Re-write ip dscp at egress based on the classification/trust settings in the switch. When this is
*       disabled, the packet is given qos treatment within the switch but the DSCP is not modified at the
*       egress. The L2 cos value gets modified as per the classification/trust settings in the switch
*       irrespective of this setting.
*
* INPUTS:
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
MSD_STATUS Fir_gprtRewriteDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_BOOL    en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtRewriteDscp Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL3, (MSD_U8)5, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write Fir_gprtRewriteDscp Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtRewriteDscp Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtSetCosToQueue
*
* DESCRIPTION:
*       Update the COS to egress queue mapping.
*
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
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
MSD_STATUS Fir_gprtSetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    IN  MSD_U8     queue
)
{
    MSD_STATUS       retVal;
    FIR_PRI_TABLE_STRUCT table;

    MSD_DBG_INFO(("Fir_gprtSetCosToQueue Called.\n"));

    retVal = Fir_gprtGetIEEEPrioMap(dev, portNum,
        FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0, cosVal, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Fir_gprtSetCosToQueue.\n"));
    }
	else
	{
		table.ingTblDei0.disIeeeQpri = 0;
		table.ingTblDei0.ieeeQpri = queue;

		retVal = Fir_gprtSetIEEEPrioMap(dev, portNum,
			FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0, cosVal, &table);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to SetIEEEPrioMap in Fir_gprtSetCosToQueue.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetCosToQueue Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetCosToQueue
*
* DESCRIPTION:
*       Get the physical queue to be mapped for trusted or assigned cos
* INPUTS:
*       portNum - the logical port number
*       cosVal  - Trusted or assigned cos for the packet
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
MSD_STATUS Fir_gprtGetCosToQueue
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     cosVal,
    OUT MSD_U8     *queue
)
{
    MSD_STATUS       retVal;
    FIR_PRI_TABLE_STRUCT table;

    MSD_DBG_INFO(("Fir_gprtGetCosToQueue Called.\n"));

    retVal = Fir_gprtGetIEEEPrioMap(dev, portNum,
        FIR_PRI_ING_PCP_2_F_Q_PRI_DEI_0, cosVal, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Fir_gprtGetCosToQueue.\n"));
    }
	else
	{
		*queue = table.ingTblDei0.ieeeQpri;
	}

    MSD_DBG_INFO(("Fir_gprtGetCosToQueue Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtSetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
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
MSD_STATUS Fir_gprtSetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cosVal,
    IN  MSD_U8      dscp
)
{
    MSD_STATUS       retVal;
    FIR_PRI_TABLE_STRUCT table;

    MSD_DBG_INFO(("Fir_gprtSetCosToDscp Called.\n"));

    retVal = Fir_gprtGetIEEEPrioMap(dev, portNum,
        FIR_PRI_EG_GREEN_FPRI_2_DSCP, cosVal, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Fir_gprtSetCosToDscp.\n"));
    }
	else
	{
		table.egGreeTblDscp.enDSCPMarking = 0x1;
		table.egGreeTblDscp.frameDSCP = dscp;

		retVal = Fir_gprtSetIEEEPrioMap(dev, portNum,
			FIR_PRI_EG_GREEN_FPRI_2_DSCP, cosVal, &table);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to SetIEEEPrioMap in Fir_gprtSetCosToDscp.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetCosToDscp Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetCosToDscp
*
* DESCRIPTION:
*       Update the COS to DSCP mapping. This will be used for rewriting DSCP for
*       IPv4/IPv6 packets based on the assigned cos value.
*
* INPUTS:
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
MSD_STATUS Fir_gprtGetCosToDscp
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      cosVal,
    OUT MSD_U8      *dscp
)
{
    MSD_STATUS       retVal;
    FIR_PRI_TABLE_STRUCT table;

    MSD_DBG_INFO(("Fir_gprtGetCosToDscp Called.\n"));

    retVal = Fir_gprtGetIEEEPrioMap(dev, portNum,
        FIR_PRI_EG_GREEN_FPRI_2_DSCP, cosVal, &table);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to GetIEEEPrioMap in Fir_gprtGetCosToDscp.\n"));
    }
	else
	{
		*dscp = table.egGreeTblDscp.frameDSCP;
	}

	MSD_DBG_INFO(("Fir_gprtGetCosToDscp Exit.\n"));
    return retVal;
}

/******************************************************************************
* Fir_gprtSetDefFpri
*
* Set port default priority.
*
******************************************************************************/
MSD_STATUS Fir_gprtSetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8  	  defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16		    data;

    MSD_DBG_INFO(("Fir_gprtSetDefFpri Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (defque > (MSD_U8)7)
		{
			MSD_DBG_ERROR(("Bad defFPri: %u.\n", defque));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			data = (MSD_U16)((MSD_U16)defque & (MSD_U16)0x7);

			/* Set the default frame priority.  */
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)13, (MSD_U8)3, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("set Fir_PVID Register return %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDefFpri Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gprtGetDefFpri
*
* Get port default priority.
*
******************************************************************************/
MSD_STATUS Fir_gprtGetDefFpri
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_U8  	   *defque
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16		    data;

    MSD_DBG_INFO(("Fir_gprtGetDefFpri Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Get the default frame priority.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)13, (MSD_U8)3, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("set Fir_PVID Register return %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			*defque = (MSD_U8)(data & (MSD_U16)0x7);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDefFpri Exit.\n"));
	return retVal;
}