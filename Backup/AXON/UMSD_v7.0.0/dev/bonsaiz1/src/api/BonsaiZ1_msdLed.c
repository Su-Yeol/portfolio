/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* BonsaiZ1_msdLed.c
*
* DESCRIPTION:
*       API/Structure definitions for Marvell LED functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsaiz1/include/api/BonsaiZ1_msdLed.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdApiInternal.h>
#include <bonsaiz1/include/driver/BonsaiZ1_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>


/****************************************************************************/
/* internal LED related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS BonsaiZ1_LedCtrl_Write
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
IN	MSD_U16	data
);
static MSD_STATUS BonsaiZ1_LedCtrl_Read
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
OUT	MSD_U16	*data
);

/*******************************************************************************
* BonsaiZ1_gledForceOn
*
* DESCRIPTION:
*       This routine is to Force Port LED On
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledForceOn
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U8 pointer = 0;
    MSD_U8 foff = 0;
    MSD_U8 flen = 0;
    MSD_U16 val = 0;
    MSD_U16 tmp = 0;

    MSD_DBG_INFO(("BonsaiZ1_gledForceOn Called.\n"));

    if ((portNum > (MSD_U32)7) || ((MSD_U32)0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (ledNum)
		{
		case 0:
			pointer = 0;
			foff = 0;
			flen = (MSD_U8)4;
			val = (MSD_U16)0xf;
			break;
		case 1U:
			pointer = 0;
			foff = (MSD_U8)4;
			flen = (MSD_U8)4;
			val = (MSD_U16)0xf;
			break;
		default:
			MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", ledNum));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, pointer, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, val, foff, flen);

				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, pointer, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("BonsaiZ1_gledForceOn Exit.\n"));
	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gledForceOff
*
* DESCRIPTION:
*       This routine is to Force Port LED Off
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledForceOff
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U8 pointer = 0;
    MSD_U8 foff = 0;
    MSD_U8 flen = 0;
    MSD_U16 val = 0;
    MSD_U16 tmp = 0;

    MSD_DBG_INFO(("BonsaiZ1_gledForceOff Called.\n"));

    if ((portNum > (MSD_U32)7) || ((MSD_U32)0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (ledNum)
		{
		case 0:
			pointer = 0;
			foff = 0;
			flen = (MSD_U8)4;
			val = (MSD_U16)0xe;
			break;
		case 1U:
			pointer = 0;
			foff = (MSD_U8)4;
			flen = (MSD_U8)4;
			val = (MSD_U16)0xe;
			break;
		default:
			MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", ledNum));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, pointer, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, val, foff, flen);

				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, pointer, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("BonsaiZ1_gledForceOff Exit.\n"));
	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gledLinkAct
*
* DESCRIPTION:
*       This routine is to set Port LED mode to link/act. That is:
*           off - no link, on - link, blink = activity.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledLinkAct
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U8 pointer = 0;
    MSD_U8 foff = 0;
    MSD_U8 flen = 0;
    MSD_U16 val = 0;
    MSD_U16 tmp = 0;

    MSD_DBG_INFO(("BonsaiZ1_gledLinkAct Called.\n"));

    if ((portNum > (MSD_U32)7) || ((MSD_U32)0 == portNum))
    {
        MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (ledNum)
		{
		case 0:
			pointer = 0;
			foff = 0;
			flen = (MSD_U8)4;
			if (portNum > (MSD_LPORT)8)
			{
				val = (MSD_U16)0x1;
			}
			else
			{
				val = (MSD_U16)0x3;
			}
			break;
		case 1U:
			pointer = 0;
			foff = (MSD_U8)4;
			flen = (MSD_U8)4;
			if (portNum > (MSD_LPORT)8)
			{
				val = (MSD_U16)0x7;
			}
			else
			{
				val = (MSD_U16)0x5;
			}

			break;
		default:
			MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", ledNum));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, pointer, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, val, foff, flen);

				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, pointer, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("BonsaiZ1_gledLinkAct Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedModeSet
*
* DESCRIPTION:
*       This routine is to set led mode
*		For BonsaiZ1 :
*		Port 1 - 8 :
*			0000 :  Link/Act/Speed by Blink Rate (off = no link, on = link, blink = activity, blink speed = link speed)
*			0001 :  100/Gig Link/Act (off = no link, on = 100 or Gig link, blink = activity)
*			0010 :  Gig Link/Act (off = no link, on = Gig link, blink = activity)
*			0010 :  Link/Act (off = no link, on = link, blink = activity)
*			0100 :  Special LED 3
*			0101 :  Special LED 4
*			0110 :  Duplex/Collision (off = half duplex, on = full duplex, blink = col)
*			0111 :  10/Gig Link/Act (off = no link, on = 10 or Gig link, blink = activity)
*			1000 :  Link (off = no link, on = link)
*			1001 :  10 Link (off = no link, on = 10 link)
*			1010 :  10 Link/Act (off = no link, on = 10 link, blink = activity)
*			1011 :  100/Gig Link (off = no link, on = 100 or Gig link)
*			1100 :  PTP Act (blink on = PTP activity)
*			1101 :  Force Blink
*			1110 :  Force Off
*			1111 :  Force On
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*       mode    - LED mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK    - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledModeSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8    ledNum,
IN  MSD_U8    mode
)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U8 pointer = 0;
	MSD_U8 foff = 0;
	MSD_U8 flen = 0;
	MSD_U16 val = 0;
	MSD_U16 tmp = 0;

	MSD_DBG_INFO(("BonsaiZ1_gledModeSet Called.\n"));

	if ((portNum > (MSD_U32)7) || (portNum == (MSD_U32)0))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (mode > (MSD_U8)0xf)
		{
			MSD_DBG_ERROR(("Bad Mode: %u. It should be within [0,15]\n", portNum));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			switch (ledNum)
			{
			case 0:
				pointer = 0;
				foff = 0;
				flen = (MSD_U8)4;
				val = mode;
				break;
			case 1U:
				pointer = 0;
				foff = (MSD_U8)4;
				flen = (MSD_U8)4;
				val = mode;
				break;
			default:
				MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", ledNum));
				retVal = MSD_BAD_PARAM;
				break;
			}
			if (retVal != MSD_BAD_PARAM)
			{
				retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, pointer, &tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					MSD_BF_SET(tmp, val, foff, flen);

					retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, pointer, tmp);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
					}
				}
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledModeSet Exit.\n"));
	return retVal;
}
/*******************************************************************************
* msdLedModeGet
*
* DESCRIPTION:
*       This routine is to get led mode
*		For BonsaiZ1 :
*		LED mode (Port 1 - 8) :
*			0000 :  Link/Act/Speed by Blink Rate (off = no link, on = link, blink = activity, blink speed = link speed)
*			0001 :  100/Gig Link/Act (off = no link, on = 100 or Gig link, blink = activity)
*			0010 :  Gig Link/Act (off = no link, on = Gig link, blink = activity)
*			0010 :  Link/Act (off = no link, on = link, blink = activity)
*			0100 :  Special LED 3
*			0101 :  Special LED 4
*			0110 :  Duplex/Collision (off = half duplex, on = full duplex, blink = col)
*			0111 :  10/Gig Link/Act (off = no link, on = 10 or Gig link, blink = activity)
*			1000 :  Link (off = no link, on = link)
*			1001 :  10 Link (off = no link, on = 10 link)
*			1010 :  10 Link/Act (off = no link, on = 10 link, blink = activity)
*			1011 :  100/Gig Link (off = no link, on = 100 or Gig link)
*			1100 :  PTP Act (blink on = PTP activity)
*			1101 :  Force Blink
*			1110 :  Force Off
*			1111 :  Force On
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       ledNum  - LED Index
*
* OUTPUTS:
*       mode - Led mode.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledModeGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT    portNum,
IN  MSD_U8       ledNum,
OUT  MSD_U8      *mode
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	
	MSD_DBG_INFO(("BonsaiZ1_gledModeGet Called.\n"));

	if ((portNum > (MSD_U32)7) || (portNum == (MSD_U32)0))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, 0, &tmp);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			switch (ledNum)
			{
			case 0:
				*mode = (MSD_U8)(tmp & (MSD_U8)0x0f);
				break;
			case 1U:
				*mode = (MSD_U8)(tmp & (MSD_U8)0xf0) >> 4;
				break;
			default:
				MSD_DBG_ERROR(("Bad LED: %u. It should be within [0,1]\n", ledNum));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledModeGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedBlinkRateSet
*
* DESCRIPTION:
*       This routine is to set led blink rate
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       rate  - LED blink rate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledBlinkRateSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  BONSAIZ1_MSD_LED_BLINK_RATE    rate
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_DBG_INFO(("BonsaiZ1_gledBlinkRateSet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0, 7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (rate > BONSAIZ1_MSD_LED_BLINK_RATE_672_MS)
		{
			MSD_DBG_ERROR(("Bad Rate: %u.\n", rate));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, (MSD_U8)6, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, (MSD_U16)rate, 0, 3);

				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, (MSD_U8)6, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledBlinkRateSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedBlinkRateGet
*
* DESCRIPTION:
*       This routine is to get led blink rate
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       rate  - LED blink rate
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledBlinkRateGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
OUT  BONSAIZ1_MSD_LED_BLINK_RATE    *rate
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_U16 val;
	MSD_DBG_INFO(("BonsaiZ1_gledBlinkRateGet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0, 7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, (MSD_U8)6, &tmp);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			val = (MSD_U16)(tmp & (MSD_U16)0x7);
			switch (val)
			{
			case 0: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_21_MS;
				break;
			case 1U: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_42_MS;
				break;
			case 2U: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_84_MS;
				break;
			case 3U: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_168_MS;
				break;
			case 4U: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_336_MS;
				break;
			case 5U: *rate = BONSAIZ1_MSD_LED_BLINK_RATE_672_MS;
				break;
			default:
				MSD_DBG_ERROR(("BonsaiZ1_gledBlinkRateGet read error"));
				retVal = MSD_FAIL;
				break;
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledBlinkRateGet Exit.\n"));
	return retVal;
}


/*******************************************************************************
* BonsaiZ1_gledPulseStretchSet
*
* DESCRIPTION:
*       This routine is to set led stretch duration
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       duration  - LED stretch duration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledPulseStretchSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  BONSAIZ1_MSD_LED_STRETCH_DURATION    duration
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_DBG_INFO(("BonsaiZ1_gledPulseStretchSet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0, 7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (duration > BONSAIZ1_STRETCH_DURATION_168_MS)
		{
			MSD_DBG_ERROR(("Bad duration: %u.\n", duration));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, (MSD_U8)6, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, (MSD_U16)duration, (MSD_U16)4, (MSD_U16)3);

				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, (MSD_U8)6, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledPulseStretchSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gledPulseStretchGet
*
* DESCRIPTION:
*       This routine is to get led stretch duration
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       duration  - LED stretch duration
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledPulseStretchGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
OUT  BONSAIZ1_MSD_LED_STRETCH_DURATION    *duration
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp;
	MSD_U16 val;
	MSD_DBG_INFO(("BonsaiZ1_gledPulseStretchGet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0, 7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, (MSD_U8)6, &tmp);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			val = (MSD_U16)(tmp & (MSD_U16)0x70) >> 4;
			switch (val)
			{
			case 0: *duration = BONSAIZ1_STRETCH_DURATION_NO;
				break;
			case 1U: *duration = BONSAIZ1_STRETCH_DURATION_21_MS;
				break;
			case 2U: *duration = BONSAIZ1_STRETCH_DURATION_42_MS;
				break;
			case 3U: *duration = BONSAIZ1_STRETCH_DURATION_84_MS;
				break;
			case 4U: *duration = BONSAIZ1_STRETCH_DURATION_168_MS;
				break;
			default:
				MSD_DBG_ERROR(("BonsaiZ1_gledPulseStretchGet read error"));
				retVal = MSD_FAIL;
				break;
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledPulseStretchGet Exit.\n"));
	return retVal;
}


/*******************************************************************************
* msdLedSpecialModeSet
*
* DESCRIPTION:
*       This routine is to set Special led portvect
*
*
* INPUTS:
*       devNum  - physical device number
*       mode  - Special led number
*		portVect - Special led bit mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledSpecialModeSet
(
IN  MSD_QD_DEV  *dev,
IN  BONSAIZ1_MSD_LED_SPECIAL_MODE mode,
IN  MSD_U16  portVect
)
{
	MSD_STATUS retVal;
	MSD_U8 pointer = 0;
	MSD_U8 foff = 0;
	MSD_U8 flen = 0;
	MSD_U16 tmp = 0;

	MSD_DBG_INFO(("BonsaiZ1_gledSpecialModeSet Called.\n"));

	if (mode > BONSAIZ1_MSD_LED_SPECIAL_MODE_PTP)
	{
		MSD_DBG_INFO(("bad mode: It should be within [0,3]\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (portVect > (MSD_U16)0x7ff)
		{
			MSD_DBG_INFO(("bad portVect: only support [0,0x7ff]."));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)mode + (MSD_U8)1;
			foff = 0;
			flen = (MSD_U8)11;

			retVal = BonsaiZ1_LedCtrl_Read(dev, 0, pointer, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BF_SET(tmp, portVect, foff, flen);

				retVal = BonsaiZ1_LedCtrl_Write(dev, 0, pointer, tmp);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledSpecialModeSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedSpecialModeGet
*
* DESCRIPTION:
*       This routine is to get Special led portvect
*
*
* INPUTS:
*       devNum  - physical device number
*       mode  - Special led number
*
* OUTPUTS:
*		portVect - Special led bit mask
*
* RETURNS:

*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledSpecialModeGet
(
IN  MSD_QD_DEV   *dev,
IN  BONSAIZ1_MSD_LED_SPECIAL_MODE mode,
OUT  MSD_U16	*portVect
)
{
	MSD_STATUS retVal;
	MSD_U8 pointer = 0;
	MSD_U16 tmp = 0;

	MSD_DBG_INFO(("BonsaiZ1_gledSpecialModeGet Called.\n"));

	if (mode > BONSAIZ1_MSD_LED_SPECIAL_MODE_PTP)
	{
		MSD_DBG_INFO(("bad mode: only support LAN , WAN , PTP."));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		pointer = (MSD_U8)mode + (MSD_U8)1;
		retVal = BonsaiZ1_LedCtrl_Read(dev, 0, pointer, &tmp);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			*portVect = tmp & (MSD_U16)0x7ff;
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledSpecialModeGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedCtrlSet
*
* DESCRIPTION:
*       This routine is to set led control register
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pointer - LED register pointer
*       data  - LED register data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledCtrlSet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8 	pointer,
IN  MSD_U16	data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("BonsaiZ1_gledCtrlSet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0,7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (pointer > (MSD_U8)7)
		{
			MSD_DBG_ERROR(("Bad Pointer: %u. It should be within [0,7]\n", pointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if (data > (MSD_U16)0x7ff)
			{
				MSD_DBG_ERROR(("Bad Data: %u. It should be less than 11 bits\n", data));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				retVal = BonsaiZ1_LedCtrl_Write(dev, portNum, pointer, data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Write returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledCtrlSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* msdLedCtrlGet
*
* DESCRIPTION:
*       This routine is to get led control register
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pointer - LED register pointer
*
* OUTPUTS:
*       data  - LED register data
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For BonsaiZ1 and BonsaiZ1:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledCtrlGet
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT portNum,
IN  MSD_U8 	pointer,
OUT MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U16 tmp = 0;
	MSD_DBG_INFO(("BonsaiZ1_gledCtrlGet Called.\n"));

	if (portNum > (MSD_U32)7)
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [0, 7]\n", portNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (pointer > (MSD_U8)7)
		{
			MSD_DBG_ERROR(("Bad Pointer: %u. It should be within [0, 7]\n", pointer));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = BonsaiZ1_LedCtrl_Read(dev, portNum, pointer, &tmp);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				*data = tmp & (MSD_U16)0x7ff;
			}
		}
	}

	MSD_DBG_INFO(("BonsaiZ1_gledCtrlGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* BonsaiZ1_msdLedDump
*
* DESCRIPTION:
*       This routine is to get all led infomation
*
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gledDump
(
IN  MSD_QD_DEV   *dev
)
{
	MSD_STATUS retVal;
	MSD_U8 portNum = 0, pointer = 0;
	MSD_U16 data0, data1, data6;

	MSD_DBG_INFO(("BonsaiZ1_gledDump Called.\n"));

	/* Port0 global control*/
	MSG(("\n--------- Port 0 Global Control ----------\n"));

	pointer = 0;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("NonMUXedLEDs    0x%-7x\n", (data0 & (MSD_U8)0x10) >> 4));
	MSG(("SkipColumns     0x%-7x\n", data0 & (MSD_U8)0xF));

	pointer = (MSD_U8)1;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("LAN Link/Act    0x%-7x\n", data0));

	pointer = (MSD_U8)2;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("WAN Link/Act    0x%-7x\n", data0));

	pointer = (MSD_U8)3;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("CPU Link/Act    0x%-7x\n", data0));

	pointer = (MSD_U8)4;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("PTP Activity    0x%-7x\n", data0));

	pointer = (MSD_U8)6;
	retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSG(("Pulse Stretch   0x%-7x\n", (data0 & (MSD_U8)0x70) >> 4));
	MSG(("Blink Rate      0x%-7x\n", data0 & (MSD_U8)0x7));

	MSG(("------------------------------------------\n"));
	/* Print out led field*/
	MSG(("\n----------------------------------------------------------------------------------------\n"));
	MSG(("     LED1Select    LED0Select    LED3Select    LED2Select    PulseStretch    BlinkRate\n"));
	MSG(("----------------------------------------------------------------------------------------\n"));

	for (portNum = 1; portNum < dev->numOfPorts - 1; portNum++)
	{
		/* Control for LED 0 & 1*/
		pointer = 0;
		retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data0);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		/* Control for LED 2 & 3*/
		pointer = (MSD_U8)1;
		retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data1);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		/* port Stretch and Blink Rate*/
		pointer = (MSD_U8)6;
		retVal = BonsaiZ1_LedCtrl_Read(dev, (MSD_LPORT)portNum, pointer, &data6);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_LedCtrl_Read returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		MSG(("P%X     0x%-7x      0x% -7x     0x%-7x      0x% -7x     0x%-7x      0x%-7x \n",
			portNum, (data0 & (MSD_U8)0xF0) >> 4, data0 & (MSD_U8)0xF, (data1 & (MSD_U8)0xF0) >> 4, data1 & (MSD_U8)0xF, (data6 & (MSD_U8)0x70) >> 4, data6 & (MSD_U8)0x7));
	}


	MSD_DBG_INFO(("BonsaiZ1_gledDump Exit.\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS BonsaiZ1_LedCtrl_Write
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
IN	MSD_U16	data
)
{
    MSD_STATUS    retVal = MSD_OK;    /* Functions return value.      */
    MSD_U8        hwPort;             /* the physical port number     */
    MSD_U8		  phyAddr;
    MSD_U16   count, tmpData;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = BONSAIZ1_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
    {
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

		count = (MSD_U16)10;
		tmpData = (MSD_U16)1;
		while (tmpData == (MSD_U16)1)
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, BONSAIZ1_LED_CONTROL, (MSD_U8)15, (MSD_U8)1, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			else
			{
				count -= (MSD_U16)1;
				if (count == (MSD_U16)0)
				{
					retVal = MSD_FAIL;
					tmpData = 0;
				}
			}
		}

		if (retVal != MSD_FAIL)
		{
			tmpData = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x7) << 12) | (data & (MSD_U16)0x7FF));

			retVal = msdSetAnyReg(dev->devNum, phyAddr, BONSAIZ1_LED_CONTROL, tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

    return retVal;
}

static MSD_STATUS BonsaiZ1_LedCtrl_Read
(
IN	MSD_QD_DEV	*dev,
IN  MSD_LPORT  portNum,
IN	MSD_U8	pointer,
OUT	MSD_U16	*data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16         tmpData;
	MSD_U16         count = (MSD_U16)0x10;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(portNum);
    phyAddr = BONSAIZ1_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

		do {
			retVal = msdGetAnyReg(dev->devNum, phyAddr, BONSAIZ1_LED_CONTROL, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			else
			{
				count -= (MSD_U16)1;
				if (count == (MSD_U16)0)
				{
					retVal = MSD_FAIL;
					tmpData = 0;
				}
			}
		} while ((tmpData & (MSD_U16)0x8000) != 0U);

		if (retVal != MSD_FAIL)
		{
			tmpData = (MSD_U16)((MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x7) << 12) | (MSD_U16)0);
			retVal = msdSetAnyReg(dev->devNum, phyAddr, BONSAIZ1_LED_CONTROL, tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				retVal = msdGetAnyReg(dev->devNum, phyAddr, BONSAIZ1_LED_CONTROL, &tmpData);
				if (retVal != MSD_OK)
				{
					retVal = MSD_FAIL;
				}
				else
				{
					*data = (MSD_U16)(tmpData & (MSD_U16)0x7ff);
				}
			}
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

    return retVal;
}