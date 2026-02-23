#if !defined(USE_PRIVATE_IMPL)
#include "../../common/include/modules/aeb-control/ACommunicator.h"

#include <cstring>

GPSStruct GPS{};
VehicleStruct Vehicle{};
GlobalPathStruct Global{};
LocalPathStruct Local{};
ControlStruct Control{};

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
 * @brief Short description of `UDPClass::SetServerSocket`.
 *
 * Detailed explanation of what `UDPClass::SetServerSocket` does,
 * including major steps and responsibilities.
 *
 * @param ip Input parameter.
 * @param port Input parameter.
 * @return void
 */
void UDPClass::SetServerSocket(const std::string &ip, const uint16_t port)
{
    (void)ip;
    (void)port;
    std::memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
    std::memset(SendBuffer, 0, sizeof(SendBuffer));
}

/**
 * @brief Short description of `UDPClass::SetClientSocket`.
 *
 * Detailed explanation of what `UDPClass::SetClientSocket` does,
 * including major steps and responsibilities.
 *
 * @param ip Input parameter.
 * @param port Input parameter.
 * @return void
 */
void UDPClass::SetClientSocket(const std::string &ip, const uint16_t port)
{
    (void)ip;
    (void)port;
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
 * @brief Short description of `GPSReceiver`.
 *
 * Detailed explanation of what `GPSReceiver` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void GPSReceiver() {}
/**
 * @brief Short description of `SRCCommunication`.
 *
 * Detailed explanation of what `SRCCommunication` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void SRCCommunication() {}
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
