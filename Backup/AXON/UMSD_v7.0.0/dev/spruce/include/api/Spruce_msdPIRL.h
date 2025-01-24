/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdPIRL.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PIRL functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdPIRL_h
#define Spruce_msdPIRL_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported PIRL Types			 			                                   */
/****************************************************************************/

/*
 *  typedef: enum SPRUCE_MSD_PIRL_COUNT_MODE
 *
 *  Description: Enumeration of the port egress rate limit counting mode.
 *
 *  Enumerations:
 *      SPRUCE_MSD_PIRL_COUNT_FRAME -
 *                Count the number of frames
 *      SPRUCE_MSD_PIRL_COUNT_ALL_LAYER1 -
 *                Count all Layer 1 bytes:
 *                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
 *      SPRUCE_MSD_PIRL_COUNT_ALL_LAYER2 -
 *                Count all Layer 2 bytes: Frame's DA to CRC
 *      SPRUCE_MSD_PIRL_COUNT_ALL_LAYER3 -
 *                Count all Layer 3 bytes:
 *                Frame's DA to CRC - 18 - 4 (if frame is tagged)
 */
typedef enum
{
    SPRUCE_MSD_PIRL_COUNT_FRAME = 0,
    SPRUCE_MSD_PIRL_COUNT_ALL_LAYER1,
    SPRUCE_MSD_PIRL_COUNT_ALL_LAYER2,
    SPRUCE_MSD_PIRL_COUNT_ALL_LAYER3
} SPRUCE_MSD_PIRL_COUNT_MODE;

/*
 *  typedef: struct SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT
 *
 *  Description:  This structure includes all the necessary parameters used to set port 
 *                ingress rate limit. It's the output of API 'Spruce_gpirlCustomSetup' based on users' 
 *                target rate and target burst allocation. And also, this structure can be used 
 *                in API 'Spruce_gpirlWriteResource' to configure IRL port resource. You can check IRL 
 *                sample code for detail usage. If the real Ingress Rate Limit is too much off 
 *                from the target rate, please contact FAE and gets the correct parameters.
 *
 *        isValid         - Reserved for future use
 *        ebsLimit        - Excess Burst Size limit on the Yellow bucket ( 0 ~ 0xFFFFFF)
 *                          In Single Rate Two Color Mode, EBSLimit must be cleared to zero.
 *        cbsLimit        - Committed Burst Size limit on the Green bucket
 *        bktIncrement    - the amount of tokens that need to be added for each 
 *                          byte of packet information.
 *        bktRateFactorGrn   - the amount of tokens that need to be decremented for each rate 
 *                             resource decrement on the Green Bucket.
 *                             The rate for Green is:
 *                                  Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * BRFGrn)/BktInc
 *                                  Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * BRFGrn)/BktInc
 *        bktRateFactorYel   - the amount of tokens that need to be decremented for each rate 
 *                             resource decrement on the Yellow Bucket.
 *                             In Single Rate Two Color Mode, BRFYel must be cleared to zero.
 *                             In Single Rate Three Color Mode, BRFYel must be equal to BRFGrn. Both must be non-zero.
 *                                  The rate for (Green + Yellow) is:
 *                                      Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * BRFYel)/BktInc
 *                                      Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * BRFTel)/BktInc						
 *							   In Dual Rate Three Color Mode, BRFYel must be greater than BRFGrn
 *                                  The Yellow rate limit is:
 *                                      Rate in bps: Calculated Igress Rate(bps) = (500,000,000 * (BRFYel-BRFGrn)/BktInc
 *                                      Rate if fps: Calculated Igress Rate(fps) = (62,000,000 * (BRFYel-BRFGrn)/BktInc		
 *        Delta           - the error ratio between Calculated Rate and Target Rate((Rate - TgtRate)/TgtRate)
 *        countMode	      - bytes to be counted for accounting
 *                          MSD_PIRL2_COUNT_FRAME,
 *                          MSD_PIRL2_COUNT_ALL_LAYER1,
 *                          MSD_PIRL2_COUNT_ALL_LAYER2, or
 *                          MSD_PIRL2_COUNT_ALL_LAYER3
*/
typedef struct
{
    MSD_BOOL       isValid;
    MSD_U32        ebsLimit;
    MSD_U32        cbsLimit;
    MSD_U32        bktIncrement;
    MSD_U32        bktRateFactorGrn;
    MSD_U32        bktRateFactorYel;
    MSD_DOUBLE     Delta;

    /*Count Mode*/
    SPRUCE_MSD_PIRL_COUNT_MODE       countMode;

} SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT;

/*
 *  typedef: enum SPRUCE_MSD_PIRL_FC_DEASSERT
 *
 *  Description: Enumeration of the port flow control de-assertion mode on PIRL.
 *
 *  Enumerations:
 *      SPRUCE_MSD_PIRL_FC_DEASSERT_EMPTY -
 *                De-assert when the ingress rate resource has become empty
 *        SPRUCE_MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
 *                De-assert when the ingress rate resource has enough room as
 *                specified by the CBSLimit.
 */
typedef enum
{
    SPRUCE_PIRL_FC_DEASSERT_EMPTY = 0,
    SPRUCE_PIRL_FC_DEASSERT_EBS_LIMIT
} SPRUCE_MSD_PIRL_FC_DEASSERT;

/*
 * Typedef: enum SPRUCE_MSD_PIRL_ACTION
 *
 * Description: Defines the Action that should be taken when
 *        there there are not enough tokens to accept the entire incoming frame
 *
 * Fields:
 *        Spruce_PIRL_ACTION_DROP - drop packets
 *        Spruce_PIRL_ACTION_FC - end flow control packet
 */
typedef enum
{
    Spruce_PIRL_ACTION_DROP = 0x0,
    Spruce_PIRL_ACTION_FC     = 0x1
} SPRUCE_MSD_PIRL_ACTION;

/*
 *  typedef: struct SPRUCE_MSD_PIRL_DATA
 *
 *  Description: data structure for PIRL Bucket programing that is resource based.
 *
 *  Fields:
 *      bktTypeMask	- various traffic type to be counted
 *      tcamFlows	- indicate to account for tcam flows 0 to 3, but not TCP/UDP data                 
 *      priAndPt    - determine the incoming frames that get rate limited using
 *                    this ingress rate resource.
 *                         MSD_TRUE - typeMask And priMask
 *                         MSD_FALSE - typeMask Or priMask         
 *      useFPri     - used to determine what priority type this resource is to account for
 *                    via the Pri Select bits below as follows:
 *                          MSD_TRUE  - use the FPri assigned to the frame
 *                          MSD_FALSE - use the QPri assigned to the frame
 *      priSelect   - priority bit mask that each bit indicates one of the eight
 *                    priorities. Setting each one of these bits indicates
 *                    that this particular rate resource is slated to account for
 *                    incoming frames with the enabled bits' priority.
 *      colorAware  - set to color aware mode or color blind mode
 *      accountGrnOverflow	- Green&Yellow buckets are connected when in Coloar Aware mode.
 *      accountQConf    - account discarded frames due to queue congestion
 *      accountFiltered - account filtered frames. Account for All 
 *      samplingMode    - sample one out of so many frames/bytes for a stream of frames
 *      actionMode      - action should be taken when there are not enough tokens
 *                        to accept the entire incoming frame
 *                                Spruce_PIRL_ACTION_DROP - drop frame
 *                                Spruce_PIRL_ACTION_FC - send flow control mode.
 *      fcMode         - flow control de-assertion mode when actionMode is
 *                       set to Spruce_PIRL_ACTION_FC.
 *                                SPRUCE_MSD_PIRL_FC_DEASSERT_EMPTY -
 *                                    De-assert when the ingress rate resource has 
 *                                    become empty.
 *                                SPRUCE_MSD_PIRL_FC_DEASSERT_CBS_LIMIT -
 *                                    De-assert when the ingress rate resource has 
 *                                    enough room as specified by the EBSLimit.     
 *      fcPriority     - When the ingress port associated with this resource is enabled
 *                          for Priority Flow Control and if this resource is configured to 
 *                          use FC Action, then this register is used to indicate the PFC 
 *                          priority that is to be used on the Priority flow Control frames 
 *                          generated due to this resource.
 *                                
 */
typedef struct
{
    /* frame types to be accounted*/
    MSD_U16 		bktTypeMask;
    MSD_BOOL		tcamFlows;
    MSD_BOOL		priAndPt;
    MSD_BOOL		useFPri;
    MSD_U8			priSelect;

    /*color aware or color blind mode*/
    MSD_BOOL		colorAware;

    /*condition for account*/
    MSD_BOOL		accountGrnOverflow;
    MSD_BOOL		accountQConf;
    MSD_BOOL		accountFiltered;

    /*condition for account*/
    MSD_BOOL		samplingMode;
	   
	/*flow control*/
    SPRUCE_MSD_PIRL_ACTION    actionMode;
    SPRUCE_MSD_PIRL_FC_DEASSERT		fcMode;
    MSD_U8	    fcPriority;	/* used when port PFC mode enabled */
	
    /*customer setup*/
    SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;

} SPRUCE_MSD_PIRL_DATA;

/****************************************************************************/
/* Exported PIRL Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Spruce_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes all PIRL Resources for all ports.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlInitialize
(
    IN  MSD_QD_DEV              *dev
);
MSD_STATUS Spruce_gpirlInitializeIntf
(
IN  MSD_QD_DEV              *dev
);
/*******************************************************************************
* Spruce_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort - target logical port
*       irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlInitResource
(
    IN  MSD_QD_DEV         *dev,
    IN    MSD_U32          irlPort,
    IN    MSD_U32          irlRes
);
MSD_STATUS Spruce_gpirlInitResourceIntf
(
IN  MSD_QD_DEV         *dev,
IN    MSD_U32          irlPort,
IN    MSD_U32          irlRes
);
/*******************************************************************************
* Spruce_gpirlCustomSetup_sr2c
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user 
*       specific target rate, target burst size and countMode.
*
* INPUTS:
*       tgtRate	- target rate(units is fps when countMode = SPRUCE_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is bps)
*       tgtBstSize	- target burst size(units is Byte)
*       customSetup - countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRFGrn/BRFYel/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		We have the following contrain in calculate the final parameters:
*			CBS >= BktIncrement * tgtBstSize
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlCustomSetup_sr2c
(
    IN    MSD_U32      tgtRate,
    IN    MSD_U32      tgtBstSize,
    INOUT SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
);
MSD_STATUS Spruce_gpirlCustomSetup_sr2cIntf
(
IN    MSD_U32      tgtRate,
IN    MSD_U32      tgtBstSize,
IN  MSD_PIRL_COUNT_MODE countMode,
OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
);
/*******************************************************************************
* Spruce_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - target logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlReadResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT        irlPort,
    IN  MSD_U32          irlRes,
    OUT SPRUCE_MSD_PIRL_DATA    *pirlData
);
MSD_STATUS Spruce_gpirlReadResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT        irlPort,
IN  MSD_U32          irlRes,
OUT MSD_PIRL_DATA    *pirlData
);
/*******************************************************************************
* Spruce_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- target port
*       irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlWriteResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT        irlPort,
    IN  MSD_U32          irlRes,
    IN  SPRUCE_MSD_PIRL_DATA    *pirlData
);
MSD_STATUS Spruce_gpirlWriteResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT        irlPort,
IN  MSD_U32          irlRes,
IN  MSD_PIRL_DATA    *pirlData
);
/*******************************************************************************
* Spruce_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*
* OUTPUTS:
*       data	- register value
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlGetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     irlPort,
    IN  MSD_U32       irlRes,
    IN  MSD_U32       regOffset,
    OUT MSD_U16       *data
);
MSD_STATUS Spruce_gpirlGetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     irlPort,
IN  MSD_U32       irlRes,
IN  MSD_U32       regOffset,
OUT MSD_U16       *data
);

/*******************************************************************************
* Spruce_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - target resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
*        
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlSetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     irlPort,
    IN  MSD_U32       irlRes,
    IN  MSD_U32       regOffset,
    IN  MSD_U16       data
);
MSD_STATUS Spruce_gpirlSetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     irlPort,
IN  MSD_U32       irlRes,
IN  MSD_U32       regOffset,
IN  MSD_U16       data
);

/*******************************************************************************
* Spruce_gpirlAdvConfigIntf
*
* DESCRIPTION:
*       This routine config ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlAdvConfigIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes,
IN  MSD_U32  tgtRate,
IN  MSD_U32  tgtBstSize,
IN  MSD_PIRL_COUNT_MODE  countMode,
IN  MSD_PIRL_PKTTYPE_OR_PRI  *pktTypeOrPri
);

/*******************************************************************************
* Spruce_gpirlAdvConfigDisableIntf
*
* DESCRIPTION:
*       This routine clear config for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlAdvConfigDisableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes
);
#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdPIRL_h */
