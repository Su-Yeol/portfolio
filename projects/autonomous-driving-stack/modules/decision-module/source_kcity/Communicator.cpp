#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/decision-module/DCommunicator.h"

#include <cstring>

const double FrontLength = 0.0;
const double OffsetLatitude = 0.0;
const double OffsetLongitude = 0.0;
const double EarthRadius = 6378137.0;
const double Lat2meter = 1.0;
const double Lon2meter = 1.0;
const double toRadian = 1.0;
const double toDegree = 1.0;

const int TargetSpeed = 0;
const bool ReceivePathFlag = false;
const bool ViewerFlag = false;
const bool GPSRecord = false;
const string GPSRecordPath = "";
const bool PathRecord = false;
const string PathRecordPath = "";
const bool PedRecord = false;
const string PedDataPath = "";
const bool IbeoRecord = false;
const string IbeoDataPath = "";
const bool RadarRecord = false;
const string RadarDataPath = "";
const string ReferenceFile = "";

bool MainFlag = true;
bool SocketFlag = false;
bool MCUSendSignal = false;
bool PathReceiveSignal = false;
bool PathErrorFlag = true;
int MobileyeFlag = 0;
int IbeoFlag = 0;
int RadarFlag = 0;
bool ViewerSenderFlag = false;
char GPSRaw[100] = {0};

GPSStruct GPS{};
GlobalPathStruct Global{};
LocalPathStruct Local{};
VehicleStruct Vehicle{};
MobileyeStruct Mobileye{};
IbeoVariable Ibeo{};
RadarStruct Radar{};

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
void CANClass::SetSocket(const std::string &ifname, const int canfd)
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
void UDPClass::SetSocket(const std::string &ip, const int port, const bool BindFlag)
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
void TCPClass::SetServerSocket(const std::string &ip, const uint16_t port)
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

/**
 * @brief Short description of `Key`.
 *
 * Detailed explanation of what `Key` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void Key() {}
/**
 * @brief Short description of `GPSParser`.
 *
 * Detailed explanation of what `GPSParser` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void GPSParser() {}
/**
 * @brief Short description of `PathReceiver`.
 *
 * Detailed explanation of what `PathReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void PathReceiver() {}
/**
 * @brief Short description of `VehicleReceiver`.
 *
 * Detailed explanation of what `VehicleReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void VehicleReceiver() {}
/**
 * @brief Short description of `MobileyeReceiver`.
 *
 * Detailed explanation of what `MobileyeReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void MobileyeReceiver() {}
/**
 * @brief Short description of `IbeoReceiver`.
 *
 * Detailed explanation of what `IbeoReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void IbeoReceiver() {}
/**
 * @brief Short description of `RadarReceiver`.
 *
 * Detailed explanation of what `RadarReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void RadarReceiver() {}
/**
 * @brief Short description of `MCUSender`.
 *
 * Detailed explanation of what `MCUSender` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void MCUSender() {}
/**
 * @brief Short description of `ViewerSender`.
 *
 * Detailed explanation of what `ViewerSender` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void ViewerSender() {}
#endif
