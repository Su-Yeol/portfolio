/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Topaz_msdLed.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell LED functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz_msdLed_h
#define __Topaz_msdLed_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef enum
{
	TOPAZ_MSD_LED_BLINK_RATE_21_MS,
	TOPAZ_MSD_LED_BLINK_RATE_42_MS,
	TOPAZ_MSD_LED_BLINK_RATE_84_MS,
	TOPAZ_MSD_LED_BLINK_RATE_168_MS,
	TOPAZ_MSD_LED_BLINK_RATE_336_MS,
	TOPAZ_MSD_LED_BLINK_RATE_672_MS,
} TOPAZ_MSD_LED_BLINK_RATE;

typedef enum
{
	TOPAZ_MSD_LED_STRETCH_DURATION_NO,
	TOPAZ_MSD_LED_STRETCH_DURATION_21_MS,
	TOPAZ_MSD_LED_STRETCH_DURATION_42_MS,
	TOPAZ_MSD_LED_STRETCH_DURATION_84_MS,
	TOPAZ_MSD_LED_STRETCH_DURATION_168_MS,
} TOPAZ_MSD_LED_STRETCH_DURATION;
typedef enum
{
	TOPAZ_MSD_LED_SPECIAL_MODE_LAN,
	TOPAZ_MSD_LED_SPECIAL_MODE_WAN,
	TOPAZ_MSD_LED_SPECIAL_MODE_CPU,
	TOPAZ_MSD_LED_SPECIAL_MODE_PTP,
} TOPAZ_MSD_LED_SPECIAL_MODE;


/*******************************************************************************
* Topaz_gledForceOn
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
MSD_STATUS Topaz_gledForceOn
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);
MSD_STATUS Topaz_gledForceOnIntf
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);
/*******************************************************************************
*   Topaz_gledForceOff
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
MSD_STATUS Topaz_gledForceOff
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);
MSD_STATUS Topaz_gledForceOffIntf
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);

/*******************************************************************************
* Topaz_gledLinkAct
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
MSD_STATUS Topaz_gledLinkAct
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);
MSD_STATUS Topaz_gledLinkActIntf
	(
	IN	MSD_QD_DEV	*dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum
	);
/*******************************************************************************
*   Topaz_gledModeSet
*
* DESCRIPTION:
*       This routine is to set led mode
*		For Peridot :
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
*       mode  - LED mode
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledModeSet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum,
	IN  MSD_U8    mode
	);
MSD_STATUS   Topaz_gledModeSetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum,
	IN  MSD_U8    mode
	);
/*******************************************************************************
*   Topaz_gledModeGet
*
* DESCRIPTION:
*       This routine is to get led mode
*		For Peridot :
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledModeGet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum,
	OUT  MSD_U8    *mode
	);
MSD_STATUS   Topaz_gledModeGetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8    ledNum,
	OUT  MSD_U8    *mode
	);
/*******************************************************************************
*   Topaz_gledBlinkRateSet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledBlinkRateSet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  TOPAZ_MSD_LED_BLINK_RATE    rate
	);
MSD_STATUS   Topaz_gledBlinkRateSetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_LED_BLINK_RATE    rate
	);
/*******************************************************************************
*   Topaz_gledBlinkRateGet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledBlinkRateGet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	OUT  TOPAZ_MSD_LED_BLINK_RATE    *rate
	);
MSD_STATUS   Topaz_gledBlinkRateGetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	OUT  MSD_LED_BLINK_RATE    *rate
	);
/*******************************************************************************
*   Topaz_gledPulseStretchSet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledPulseStretchSet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  TOPAZ_MSD_LED_STRETCH_DURATION    duration
	);
MSD_STATUS   Topaz_gledPulseStretchSetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_LED_STRETCH_DURATION    duration
	);
/*******************************************************************************
*   Topaz_gledPulseStretchGet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledPulseStretchGet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	OUT TOPAZ_MSD_LED_STRETCH_DURATION    *duration
	);
MSD_STATUS   Topaz_gledPulseStretchGetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	OUT MSD_LED_STRETCH_DURATION    *duration
	);
/*******************************************************************************
*   Topaz_gledSpecialModeSet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledSpecialModeSet
	(
	IN  MSD_QD_DEV   *dev,
	IN  TOPAZ_MSD_LED_SPECIAL_MODE	mode,
	IN  MSD_U16  portVect
	);
MSD_STATUS   Topaz_gledSpecialModeSetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LED_SPECIAL_MODE	mode,
	IN  MSD_U16  portVect
	);
/*******************************************************************************
*   Topaz_gledSpecialModeGet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledSpecialModeGet
	(
	IN  MSD_QD_DEV   *dev,
	IN  TOPAZ_MSD_LED_SPECIAL_MODE	mode,
	OUT  MSD_U16	*portVect
	);
MSD_STATUS   Topaz_gledSpecialModeGetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LED_SPECIAL_MODE	mode,
	OUT  MSD_U16	*portVect
	);
/*******************************************************************************
*   Topaz_gledCtrlSet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledCtrlSet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8 	pointer,
	IN  MSD_U16	data
	);
MSD_STATUS   Topaz_gledCtrlSetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8 	pointer,
	IN  MSD_U16	data
	);
/*******************************************************************************
*   Topaz_gledCtrlGet
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
*       For Peridot and Topaz:
*       If IMP boots, all LEDs are deactive. It's the IMP software's responsibility
*       to determine when to turn on the LEDs by clearing these bits (Global LED Control,
*       index 0x00, bit[3:0], Skip Columns) to zero. Then we can call this API to
*       configure LED mode.
*
*******************************************************************************/
MSD_STATUS   Topaz_gledCtrlGet
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8 	pointer,
	OUT MSD_U16	*data
	);
MSD_STATUS   Topaz_gledCtrlGetIntf
	(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT portNum,
	IN  MSD_U8 	pointer,
	OUT MSD_U16	*data
	);

/*******************************************************************************
* Topaz_msdLedDump
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
MSD_STATUS Topaz_gledDump
(
IN  MSD_QD_DEV   *dev
);

#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdLed_h */