// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

#ifndef APD_PICAR_CTRL_ACTIVITY_H_
#define APD_PICAR_CTRL_ACTIVITY_H_

#include <random>
#include <string>

// includes for used services
#include "apd/picar/actorinterface_proxy.h"
#include "apd/picar/linesensorinterface_proxy.h"
#ifdef ENABLE_ULTRASONIC
#    include "apd/picar/ultrasonicsensorinterface_proxy.h"
#endif
#include "ara/com/e2e_helper.h"
#include "ara/log/logger.h"
#include "apd/rest/server.h"
#include "apd/rest/routing.h"

namespace apd
{
namespace picar
{
namespace controller
{

using apd::picar::proxy::ActorInterfaceProxy;
using apd::picar::proxy::LineSensorInterfaceProxy;
#ifdef ENABLE_ULTRASONIC
using apd::picar::proxy::UltraSonicSensorInterfaceProxy;
#endif

enum class ControllerState
{
    kInitial,
    kForward,
    kForwardLeft,
    kForwardLeftSharp,
    kForwardRight,
    kForwardRightSharp,
    kBackwardLeft,
    kBackwardRight,
    kTurningBackward,
    kTurning,
    kObstacle,
    kSensorFailure,
    kStopped
};

enum class ControllerMode
{
    kFollowRoad,
    kStop
};

class ControllerActivity
{

public:
    ControllerActivity();

    /*!
     *  \brief Initializes controller activity.
     *
     *  Initializes controller activity. This is called during initialization of
     * the runtime.
     */
    void init(bool enableRest);

    /*!
     *  \brief Runs controller activity.
     *
     *  Executable unit triggered to perform controller activity.
     */
    void act();

private:
    static void SigTermHandler(int signal);
    bool RegisterSigTermHandler();
    void SubscribeLineSensor();
#ifdef ENABLE_ULTRASONIC
    void SubscribeUltraSonicSensor();
#endif
    void FollowRoad();
    void RoadFollowerWaitForCenter();
    ara::log::Logger& log_{ara::log::CreateLogger("PCTA", "PiCar Controller Activity", ara::log::LogLevel::kVerbose)};
    ara::core::String StateToString(ControllerState state);
    apd::rest::Router CreateRouter();
    apd::rest::Route::Upshot PutControlHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot PutConfigHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot ReturnCurrentConfig(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot ReturnCurrentValues(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    void Move(int angle, int speed);

    int step = 0;
    const int aStep = 22;
    const int bStep = 40;
    int offTrackCount_ = 1;
    int turningAngle_ = 90;
    int turningSpeed_ = 40;
    int forwardSpeed_ = 60;
    int backwardSpeed_ = -40;
    int detectionThreshold_ = 160;
    int tmpAngle_ = 90;
    int turningMax_ = 45;
    unsigned int noRightTurns_ = 0;
    unsigned int noLeftTurns_ = 0;
    int maxOffTrackCount_ = 0;
    int currentSpeed_ = 0;
    int currentAngle_ = 90;
    ControllerState loggedState_ = ControllerState::kInitial;
    ControllerState controllerState_ = ControllerState::kInitial;
    ControllerMode loggedMode_;
    ControllerMode controllerMode_;
    uint8_t LFStatusBool_;
    std::array<uint16_t, 5> LFStatusRaw_;
    std::shared_ptr<ActorInterfaceProxy> serviceActor_;
    std::shared_ptr<LineSensorInterfaceProxy> serviceLineSensor_;
#ifdef ENABLE_ULTRASONIC
    unsigned int distance_ = 0;
    std::shared_ptr<UltraSonicSensorInterfaceProxy> serviceUltrasonicSensor_;
    std::atomic_uint64_t ultrasonicsensorUpdate_;
#endif
    std::atomic_uint64_t linesensorUpdate_;

    std::unique_ptr<apd::rest::Server> restServer_;
};

}  // namespace controller
}  // namespace picar
}  // namespace apd

#endif  // APD_PICAR_CTRL_ACTIVITY_H_
