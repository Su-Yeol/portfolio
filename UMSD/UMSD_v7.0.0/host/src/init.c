/*******************************************************************************
*
* init.c
*
* DESCRIPTION:
*       QD initialization module
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "Wrapper.h"
#include "msdApi.h"
#include "rmuPcap.h"
 
#define MSG_PRINT printf

#define SOHO_DEV_NUM	0 /* update by application*/

#ifdef USE_SMI_MULTI_ADDR
  #define SOHO_DEV_ADDR	2 /* depends on hardware configuration, non-zero if dut is in multi-chip mode*/
#else
  #define SOHO_DEV_ADDR	0
#endif

/*MSD_SYS_CONFIG   cfg;*/
MSD_QD_DEV       QdDev;
MSD_QD_DEV       *qddev = &QdDev;
MSD_U8	sohoDevNum = SOHO_DEV_NUM;
MSD_RMU_MODE gRmuMode = MSD_RMU_DSA_MODE;
MSD_U32 gEtherTypeValue = 0x9100;
MSD_U8 gXMDIOFlag = 0;
MSD_U8 gFWRmuFlag = 0;
MSD_U16 gFakeDevice = 0;

static MSD_STATUS SMIRead(MSD_U8 devNum, MSD_U8 phyAddr,
               MSD_U8 miiReg, MSD_U16* value)
{
	int ret = 0;

    if (gFakeDevice)
    {
        *value = gFakeDevice;
        return MSD_OK;
    }

	if (gXMDIOFlag == 0)
	{
		ret = readRegister(phyAddr, miiReg);
	}
	else
	{
		/*For Amethyst, access register through C45 is supported, and devAddr been fixed as 3, and register has one 0x8000 offset*/
		ret = readC45Register(phyAddr, 3, miiReg | 0x8000);
	}
	*value = (MSD_U16)ret;
	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
                                                                                
static MSD_STATUS SMIWrite(MSD_U8 devNum, MSD_U8 phyAddr,
                      MSD_U8 miiReg, MSD_U16 value)
{
	int ret = 0;

	if (gFakeDevice)
	{
		return MSD_OK;
	}

	if (gXMDIOFlag == 0)
	{
		ret = writeRegister(phyAddr, miiReg, value);
	}
	else
	{
		/*For Amethyst, access register through C45 is supported, and devAddr been fixed as 3, and register has one 0x8000 offset*/
		ret = writeC45Register(phyAddr, 3, miiReg | 0x8000, value);
	}

	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}

#ifdef USE_SEMAPHORE
static MSD_SEM osSemCreate(MSD_SEM_BEGIN_STATE state)
{
	MSD_SEM pSem;
	int count = (int)state;

	pSem = semaphoreCreate(count);

	return pSem;
}
static MSD_STATUS osSemDelete(MSD_SEM smid)
{
	int ret = 0 ;

	ret = semaphoreClose(smid);
	
	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
static MSD_STATUS osSemWait(MSD_SEM smid, MSD_U32 timeOut)
{
	int ret = 0 ;

	ret = semaphoreWait(smid, timeOut);
	
	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
static MSD_STATUS osSemSignal(MSD_SEM smid)
{
	int ret = 0 ;

	ret = semaphoreRelease(smid);
	
	if(ret < 0)
		return MSD_FAIL;
	else
		return MSD_OK;
}
#endif

/* Register function to BSP */
MSD_STATUS qdStart(MSD_U8 devNum, MSD_U8 baseAddr, MSD_U32 phyAddr, MSD_U32 cpuPort, MSD_32 bus_interface, MSD_U16 tempDeviceId)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_SYS_CONFIG   cfg;

	memset((char*)&cfg,0,sizeof(MSD_SYS_CONFIG));

	cfg.BSPFunctions.rmu_tx_rx = send_and_receive_packet;
	cfg.BSPFunctions.readMii   = SMIRead;
	cfg.BSPFunctions.writeMii  = SMIWrite;
    cfg.InterfaceChannel = (MSD_INTERFACE)bus_interface;
#ifdef USE_SEMAPHORE
	cfg.BSPFunctions.semCreate = osSemCreate;
	cfg.BSPFunctions.semDelete = osSemDelete;
	cfg.BSPFunctions.semTake   = osSemWait;
	cfg.BSPFunctions.semGive   = osSemSignal;
#else
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake   = NULL;
	cfg.BSPFunctions.semGive   = NULL;
#endif

	cfg.devNum = devNum;
	cfg.baseAddr = baseAddr;
    cfg.rmuMode = gRmuMode;
    cfg.eTypeValue = gEtherTypeValue;
	cfg.tempDeviceId = tempDeviceId;

	if((status = msdLoadDriver(&cfg)) != MSD_OK)
	{
		return status;
	}

	return MSD_OK;
}

void qdClose() 
{
	if (sohoDevGet(sohoDevNum))
		msdUnLoadDriver(sohoDevNum);
}

/* 
 *	Look up any Switch devices that are attached to the SMI pair.
*/
MSD_STATUS qdInit(int baseAddr, int bus_interface, MSD_U16 tempDeviceId)
{
	MSD_U8 devNum, devAddr;
	MSD_U32		phyAddr, cpuPort;
	MSD_STATUS	status;

	cpuPort = 0;  /* Not use so far  */
	phyAddr = 0;  /* Not use so far  */

	devNum = sohoDevNum;
	/*devAddr = SOHO_DEV_ADDR;*/
    devAddr = baseAddr;
	
	status = qdStart(devNum, devAddr, phyAddr, cpuPort, bus_interface, tempDeviceId);
	if (MSD_OK != status)
	{
		return -1;
	}

	qddev = sohoDevGet(sohoDevNum);

	return MSD_OK;
}

void clean_stdin(void)
{
	int c;
	do{
		c = getchar();
	}while(c != '\n' && c != EOF);
}
