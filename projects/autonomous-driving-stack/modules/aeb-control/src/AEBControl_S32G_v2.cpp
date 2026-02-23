#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/aeb-control/AControlModule.h"
#include "../../common/include/modules/aeb-control/ACommunicator.h"

#include <cstdio>
#include <cstdlib>

CANClass VehicleCANFD;

uint8_t AliveCnt = 0;
uint8_t CRCDataHigh = 0;
uint8_t CRCDataLow = 0;

uint8_t AEBStopAliveCnt = 0;
uint8_t WrngLvlCnt = 0;

bool AEBComFlag = false;
bool AEBStopFlag = false;

uint8_t FCA_WrngLvlSta = 0;
uint8_t FCA_SysFlrSta = 0;
uint8_t FCA_OnOffEquipSta = 0;
uint8_t FCA_HydrlcBstAsstlSta = 0;
uint8_t FCA_StbltActvReq = 0;
uint8_t FCA_VehStpReq = 0;
uint8_t FCA_DclReqVal = 0;
uint8_t FCA_FullActvReq = 0;
uint8_t FCA_PartialActvReq = 0;
uint8_t FCA_PrefillActvReq = 0;
uint8_t FCA_WrngSndSta = 0;
uint8_t FCA_SnstvtyModRetVal = 0;
uint16_t FCA_RelVel = 0;
uint8_t FCA_TimetoCllsn = 0;
uint8_t Nmode_FCAOff_Sta = 0;
uint8_t FCA_Jnctn_OnOffEquipSta = 0;
uint8_t ADAS_DRV_FCA_Plus_Sta = 0;
uint8_t FCA_WrngTrgtDis = 0;

int Msg1A0_index = 0;

/**
 * @brief Short description of `getch`.
 *
 * Detailed explanation of what `getch` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
int getch(void)
{
    return -1;
}

/**
 * @brief Short description of `Key1`.
 *
 * Detailed explanation of what `Key1` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void Key1() {}

/**
 * @brief Short description of `AEBComControl`.
 *
 * Detailed explanation of what `AEBComControl` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void AEBComControl() {}

/**
 * @brief Short description of `AEBStopControl`.
 *
 * Detailed explanation of what `AEBStopControl` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void AEBStopControl() {}

/**
 * @brief Short description of `main`.
 *
 * Detailed explanation of what `main` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
int main()
{
    std::fprintf(
        stderr,
        "[aeb-control] private implementation is not available. "
        "Set USE_PRIVATE_IMPL=1 and provide modules/common/src/private/aeb-control/*_impl.cpp\n");
    return EXIT_FAILURE;
}
#endif
