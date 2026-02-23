#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/decision-module/DCommunicator.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

const string S32GIp = "";
const int S32GPort = 0;

CANClass C_CANFrame;
CANClass MDPSFrame;
CANClass E_CANFrame;
CANClass DRVFrame;

uint8_t AlvCnt = 0;
uint8_t AlvCnt2 = 0;
uint8_t AlvCnt3 = 0;

uint8_t SCCbtn = 0;
int SCC_SysFlrSta = 0;
int SCC_MainOnOffSta = 0;
int SCC_OpSta = 0;
int SCC_TakeoverReq = 0;
int SCC_InfoDis = 0;
int SCC_DrvAlrtDis = 0;
int SCC_ObjDstLvlVal = 0;
int SCC_HeadwayDstSetVal = 0;
int SCC_NSCCAutoSetSpdSta = 0;
int SCC_TrgtSpdSetVal = 0;
int SCC_NSCCOpsta = 0;
int SCC_NSCCOnOffsta = 0;
int SCC_ObjSta = 0;
int SCC_SnstvtyModRetVal = 0;
int SCC_NSCCAutoSetSpdUpdtsta = 0;
int SCC_NSCCInfoPUDis = 0;
int SCC_AccelReqVal = 0;
int SCC_AccelReqRawVal = 0;
int SCC_JrkUpplimVal = 0;
int SCC_JrkLwrLimVal = 0;
int SCC_AccelLimBandUppVal = 0;
int SCC_AccelLimBandLwrVal = 0;
int SCC_VehStpReq = 0;
int SCC_DclModReq = 0;
int SCC_TrgtDstVal = 0;

int SWRC_CrsMainSwSta = 0;

uint8_t MDPSmodeCnt = 0;
uint8_t MDPSmode = 0;
uint8_t PAmode = 0;
int PA_StrAnglReq = 0;
int PrePA_StrAnglReq = 0;
int CurrentAnglVal = 0;
int ReferAnglVal = 0;
int AnglValDiffer = 0;

uint8_t PRNDmode = 0;
double ESC_CylPrsrVal = 0.0;

/**
 * @brief Short description of `CCANtoMDPS`.
 *
 * Detailed explanation of what `CCANtoMDPS` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CCANtoMDPS() {}

/**
 * @brief Short description of `MDPStoCCAN`.
 *
 * Detailed explanation of what `MDPStoCCAN` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void MDPStoCCAN() {}

/**
 * @brief Short description of `ECANtoDRV`.
 *
 * Detailed explanation of what `ECANtoDRV` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ECANtoDRV() {}

/**
 * @brief Short description of `DRVtoECAN`.
 *
 * Detailed explanation of what `DRVtoECAN` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void DRVtoECAN() {}

/**
 * @brief Short description of `UDPRecv`.
 *
 * Detailed explanation of what `UDPRecv` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void UDPRecv() {}

/**
 * @brief Short description of `main`.
 *
 * Detailed explanation of what `main` does,
 * including major steps and responsibilities.
 *
 * @param argc Input parameter.
 * @param argv Input parameter.
 * @return Output value from the function.
 */
int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;
    std::fprintf(
        stderr,
        "[decision-module:gateway] private implementation is not available. "
        "Set USE_PRIVATE_IMPL=1 and provide modules/common/src/private/decision-module/*_impl.cpp\n");
    return EXIT_FAILURE;
}

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
 * @brief Short description of `key`.
 *
 * Detailed explanation of what `key` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void key() {}

/**
 * @brief Short description of `CANClass::SetSocket`.
 *
 * Detailed explanation of what `CANClass::SetSocket` does,
 * including major steps and responsibilities.
 *
 * @param ifname Input parameter.
 * @param canfd Input parameter.
 * @return void
 */
void CANClass::SetSocket(const std::string& ifname, const int canfd)
{
    (void)ifname;
    (void)canfd;
    std::memset(&FrameFd, 0, sizeof(FrameFd));
    std::memset(&Frame, 0, sizeof(Frame));
}

/**
 * @brief Short description of `CANClass::ReceiveCANFD`.
 *
 * Detailed explanation of what `CANClass::ReceiveCANFD` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::ReceiveCANFD() {}

/**
 * @brief Short description of `CANClass::ReceiveCAN`.
 *
 * Detailed explanation of what `CANClass::ReceiveCAN` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::ReceiveCAN() {}

/**
 * @brief Short description of `CANClass::SendCANFD`.
 *
 * Detailed explanation of what `CANClass::SendCANFD` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::SendCANFD() {}

/**
 * @brief Short description of `CANClass::SendCAN`.
 *
 * Detailed explanation of what `CANClass::SendCAN` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::SendCAN() {}

/**
 * @brief Short description of `CANClass::CloseSocket`.
 *
 * Detailed explanation of what `CANClass::CloseSocket` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::CloseSocket() {}

/**
 * @brief Short description of `CANClass::InitFrame`.
 *
 * Detailed explanation of what `CANClass::InitFrame` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::InitFrame()
{
    std::memset(&FrameFd, 0, sizeof(FrameFd));
    std::memset(&Frame, 0, sizeof(Frame));
}

/**
 * @brief Short description of `UDPClass::SetSocket`.
 *
 * Detailed explanation of what `UDPClass::SetSocket` does,
 * including major steps and responsibilities.
 *
 * @param ip Input parameter.
 * @param port Input parameter.
 * @param BindFlag Input parameter.
 * @return void
 */
void UDPClass::SetSocket(const std::string& ip, const int port, const bool BindFlag)
{
    (void)ip;
    (void)port;
    (void)BindFlag;
    std::memset(Buffer, 0, sizeof(Buffer));
}

/**
 * @brief Short description of `UDPClass::Receive`.
 *
 * Detailed explanation of what `UDPClass::Receive` does,
 * including major steps and responsibilities.
 *
 * @param buffersize Input parameter.
 * @return void
 */
void UDPClass::Receive(const uint16_t buffersize)
{
    (void)buffersize;
}

/**
 * @brief Short description of `UDPClass::Send`.
 *
 * Detailed explanation of what `UDPClass::Send` does,
 * including major steps and responsibilities.
 *
 * @param SendByte Input parameter.
 * @return void
 */
void UDPClass::Send(const uint16_t SendByte)
{
    (void)SendByte;
}

/**
 * @brief Short description of `UDPClass::CloseSocket`.
 *
 * Detailed explanation of what `UDPClass::CloseSocket` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void UDPClass::CloseSocket() {}

/**
 * @brief Short description of `TCPClass::SetServerSocket`.
 *
 * Detailed explanation of what `TCPClass::SetServerSocket` does,
 * including major steps and responsibilities.
 *
 * @param ip Input parameter.
 * @param port Input parameter.
 * @return void
 */
void TCPClass::SetServerSocket(const std::string& ip, const uint16_t port)
{
    (void)ip;
    (void)port;
    std::memset(SendBuffer, 0, sizeof(SendBuffer));
}

/**
 * @brief Short description of `TCPClass::Send`.
 *
 * Detailed explanation of what `TCPClass::Send` does,
 * including major steps and responsibilities.
 *
 * @param SendByte Input parameter.
 * @return void
 */
void TCPClass::Send(const uint16_t SendByte)
{
    (void)SendByte;
}

/**
 * @brief Short description of `TCPClass::CloseSocket`.
 *
 * Detailed explanation of what `TCPClass::CloseSocket` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void TCPClass::CloseSocket() {}
#endif
