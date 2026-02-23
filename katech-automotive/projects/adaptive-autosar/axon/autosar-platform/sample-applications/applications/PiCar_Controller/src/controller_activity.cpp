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

///////////////////////////////////////////////////////////////////////
// Activity specific implementation, skeleton can be generated from the model
// Discovery of services and sending/receiving of data according
// to the communication API
///////////////////////////////////////////////////////////////////////

#include "controller_activity.h"
#include "find_service.h"

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <thread>
#include <bitset>
#include "apd/rest/ogm/array.h"
#include "apd/rest/ogm/field.h"
#include "apd/rest/ogm/int.h"
#include "apd/rest/ogm/object.h"
#include "apd/rest/ogm/real.h"
#include "apd/rest/ogm/string.h"
#include "apd/rest/ogm/visit.h"

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

ControllerActivity::ControllerActivity()
{ }

std::atomic_bool continueExecution_(true);

void ControllerActivity::SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        continueExecution_ = false;
    }
}

bool ControllerActivity::RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

void ControllerActivity::init(bool enableRest)
{
    auto actorService = apd::picar::controller::FindActorService();
    auto lineSensorService = apd::picar::controller::FindLineSensorService();
#ifdef ENABLE_ULTRASONIC
    auto ultrasonicService = apd::picar::controller::FindUltraSonicSensorService();
#endif

    if (actorService.has_value()) {

        serviceActor_ = *actorService;

        log_.LogInfo() << "Actor Service found";
    }
    if (lineSensorService.has_value()) {

        serviceLineSensor_ = *lineSensorService;
        log_.LogInfo() << "LineSensor Service found";
    }
#ifdef ENABLE_ULTRASONIC
    if (ultrasonicService.has_value()) {

        serviceUltrasonicSensor_ = *ultrasonicService;
        log_.LogInfo() << " UltraSonicSensor Service found";
    }
#endif

    if (!RegisterSigTermHandler()) {
        log_.LogError() << "Unable to register signal handler";
    }

    if (enableRest) {
        controllerMode_ = ControllerMode::kStop;
        controllerState_ = ControllerState::kStopped;
        restServer_ = std::make_unique<apd::rest::Server>("ControllerRestServer", CreateRouter());
        restServer_->Start(apd::rest::StartupPolicy::kDetached);
    } else {
        controllerMode_ = ControllerMode::kFollowRoad;
        controllerState_ = ControllerState::kInitial;
    }
}

void ControllerActivity::act()
{
    // run in separate thread
    std::thread controller(&ControllerActivity::FollowRoad, this);

    // create sensor threads
    std::thread lineSensor(&ControllerActivity::SubscribeLineSensor, this);
#ifdef ENABLE_ULTRASONIC
    std::thread ultrasonicSensor(&ControllerActivity::SubscribeUltraSonicSensor, this);
#endif

    // join threads
    controller.join();
    lineSensor.join();
#ifdef ENABLE_ULTRASONIC
    ultrasonicSensor.join();
#endif
}

void ControllerActivity::SubscribeLineSensor()
{
    while (continueExecution_) {
        if (nullptr != serviceLineSensor_) {
            if (!serviceLineSensor_->lineSensorEvent.IsSubscribed()) {
                log_.LogInfo() << "not subscribed to lineSensorEvent yet";
                // subscribe to event
                auto subscription_result = serviceLineSensor_->lineSensorEvent.Subscribe(3);
                if (subscription_result.HasValue()) {
                    log_.LogInfo() << "Callback registered.";
                } else {
                    log_.LogError() << "Subscription failed with error: " << subscription_result.Error();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    continue;
                }
                log_.LogInfo() << "lineSensorEvent subscription complete";

                serviceLineSensor_->lineSensorEvent.SetReceiveHandler([this] {
                    serviceLineSensor_->lineSensorEvent.GetNewSamples([this](auto sample) {
                        auto const& values = sample->objectVector;
                        uint8_t lfstatus = 0;
                        static uint32_t evCtr;
                        evCtr++;
                        for (int i = 0; i < 5; i++) {
                            LFStatusRaw_.at(i) = values.at(i);
                            bool classifiedValue = values.at(i) <= detectionThreshold_;
                            lfstatus = (lfstatus << 1) | classifiedValue;
                        }
                        if (lfstatus != LFStatusBool_ || evCtr >= 40) {
                            // log only if values changed or once per second (data produced every 25ms)
                            std::bitset<5> x(lfstatus);
                            log_.LogDebug()
                                << "Linesensor Values Received: " << x.to_string() << " raw: " << values.at(0)
                                << values.at(1) << values.at(2) << values.at(3) << values.at(4);
                            evCtr = 0;
                        }
                        LFStatusBool_ = lfstatus;
                        linesensorUpdate_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch())
                                                .count();
                    });
                });
                log_.LogInfo() << "lineSensorEventReceived SetReceiveHandler() complete";
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

/**
 * Call proxy and update internal state for REST API
 *
 * speed is given as reference, because it can be changed by REST API in the
 * mean time.
 */
void ControllerActivity::Move(int angle, int speed)
{
    if (controllerMode_ == ControllerMode::kStop) {
        // handle exiting the follow road mode
        currentSpeed_ = 0;
        currentAngle_ = 90;
        controllerState_ = ControllerState::kStopped;
    } else {
        currentSpeed_ = speed;
        currentAngle_ = angle;
    }
    serviceActor_->Move(angle, speed);
}

#ifdef ENABLE_ULTRASONIC
void ControllerActivity::SubscribeUltraSonicSensor()
{
    while (continueExecution_) {
        if (nullptr != serviceUltrasonicSensor_) {
            if (!serviceUltrasonicSensor_->ultrasonicSensorEvent.IsSubscribed()) {
                log_.LogInfo() << "not subscribed to ultrasonicSensorEvent yet";
                // m_proxy got initialized via callback.
                // subscribe to event
                auto subscription_result = serviceUltrasonicSensor_->ultrasonicSensorEvent.Subscribe(3);
                if (subscription_result.HasValue()) {
                    log_.LogInfo() << "Callback registered.";
                } else {
                    log_.LogError() << "Subscription failed with error: " << subscription_result.Error();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    continue;
                }
                log_.LogInfo() << "ultrasonicSensorEvent subscription complete";

                serviceUltrasonicSensor_->ultrasonicSensorEvent.SetReceiveHandler([this] {
                    serviceUltrasonicSensor_->ultrasonicSensorEvent.GetNewSamples([this](auto sample) {
                        auto const& value = sample;
                        if (distance_ != int(*value)) {
                            log_.LogInfo() << "Ultrasonic distance: " << int(*value);
                        }
                        distance_ = *value;
                        ultrasonicsensorUpdate_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch())
                                                      .count();
                    });
                });
                log_.LogInfo() << "ultrasonicSensorEventReceived SetReceiveHandler() complete";
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
#endif

void ControllerActivity::RoadFollowerWaitForCenter()
{
    int logCtr = 0;
    while (controllerMode_ == ControllerMode::kFollowRoad) {
        uint8_t lfstatus;
        lfstatus = LFStatusBool_;
        std::bitset<5> x(lfstatus);
        if (lfstatus == 0b00000 || lfstatus == 0b10000 || lfstatus == 0b00001) {
            log_.LogInfo() << "Back in center " << x.to_string();
            return;
        }
        if (logCtr % 40 == 0) {
            log_.LogInfo() << "Waiting for center " << x.to_string();
            logCtr++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

void ControllerActivity::FollowRoad()
{
    // reset steering angle
    Move(90, 0);
    std::uint64_t now;
    std::uint64_t freshnessLinesensor;
#ifdef ENABLE_ULTRASONIC
    std::uint64_t freshnessUltrasonicsensor;
#endif
    std::uint8_t LFStatus;
    while (continueExecution_) {
        now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
                  .count();
        freshnessLinesensor = now - linesensorUpdate_;
#ifdef ENABLE_ULTRASONIC
        freshnessUltrasonicsensor = now - ultrasonicsensorUpdate_;
#endif
        LFStatus = LFStatusBool_;
        std::bitset<5> x(LFStatus);
        if (controllerMode_ == ControllerMode::kStop) {
            controllerState_ = ControllerState::kStopped;
            if (loggedMode_ != controllerMode_) {
                log_.LogInfo() << "Stopped by HMI";
                loggedMode_ = controllerMode_;
            }
            Move(90, 0);
#ifdef ENABLE_ULTRASONIC
        } else if (distance_ < 10) {
            controllerState_ = ControllerState::kObstacle;
            if (loggedState_ != controllerState_) {
                log_.LogInfo() << "Blocked by obstacle";
                loggedState_ = controllerState_;
            }
            Move(90, 0);
#endif
        } else if (freshnessLinesensor >= 100) {
            controllerState_ = ControllerState::kSensorFailure;
            if (loggedState_ != controllerState_) {
                log_.LogError() << "Line sensor data too old (" << freshnessLinesensor << " ms)";
                loggedState_ = controllerState_;
            }
            Move(90, 0);
#ifdef ENABLE_ULTRASONIC
        } else if (freshnessUltrasonicsensor >= 100) {
            controllerState_ = ControllerState::kSensorFailure;
            if (loggedState_ != controllerState_) {
                log_.LogError() << "Ultrasonic sensor data too old (" << freshnessUltrasonicsensor << " ms)";
                loggedState_ = controllerState_;
            }
            Move(90, 0);
#endif
        } else {
            // Calculate Steering Angle
            if (LFStatus == 0b00000 || LFStatus == 0b10001) {
                step = 0;
            } else if (LFStatus == 0b10000 || LFStatus == 0b00001) {
                step = aStep;
            } else if (LFStatus == 0b01000 || LFStatus == 0b11000 || LFStatus == 0b00010 || LFStatus == 0b00011) {
                step = bStep;
            } else {
                step = 0;
            }

            // Calculate direction
            // Go straight
            if (LFStatus == 0b00000 || LFStatus == 0b10001) {
                offTrackCount_ = 0;
                turningAngle_ = 90;
                Move(turningAngle_, forwardSpeed_);
                controllerState_ = ControllerState::kForward;
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Forward";
                    loggedState_ = controllerState_;
                }

                // turn right
            } else if (LFStatus == 0b10000 || LFStatus == 0b01000 || LFStatus == 0b11000) {
                offTrackCount_ = 0;
                turningAngle_ = 90 - step;
                controllerState_ = ControllerState::kForwardRight;
                Move(turningAngle_, turningSpeed_);
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turn right";
                    loggedState_ = controllerState_;
                }
                ++noRightTurns_;
            }
            // turn left
            else if (LFStatus == 0b00001 || LFStatus == 0b00010 || LFStatus == 0b00011) {
                offTrackCount_ = 0;
                turningAngle_ = 90 + step;
                controllerState_ = ControllerState::kForwardLeft;
                Move(turningAngle_, turningSpeed_);
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turn left";
                    loggedState_ = controllerState_;
                }
                ++noLeftTurns_;
            }
            // turn back + right
            else if (LFStatus == 0b10100 || LFStatus == 0b11100 || LFStatus == 0b01100 || LFStatus == 0b11110) {
                controllerState_ = ControllerState::kBackwardRight;
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turn back right";
                    loggedState_ = controllerState_;
                }
                ++offTrackCount_;
                if (offTrackCount_ > maxOffTrackCount_) {
                    Move(90, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(130, backwardSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    RoadFollowerWaitForCenter();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(50, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(50, turningSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            // turn back + left
            else if (LFStatus == 0b00101 || LFStatus == 0b00111 || LFStatus == 0b00110 || LFStatus == 0b01111) {
                controllerState_ = ControllerState::kBackwardLeft;
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turn back left";
                    loggedState_ = controllerState_;
                }
                ++offTrackCount_;
                if (offTrackCount_ > maxOffTrackCount_) {
                    Move(90, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(50, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(50, backwardSpeed_);
                    RoadFollowerWaitForCenter();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(130, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(130, turningSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }  // THis is special case, while turning
            else if (LFStatus == 0b01110) {
                controllerState_ = ControllerState::kTurning;
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turning";
                    loggedState_ = controllerState_;
                }
                if (noLeftTurns_ > noRightTurns_) {
                    Move(90, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(45, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(45, backwardSpeed_);
                    RoadFollowerWaitForCenter();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(135, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(135, turningSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                } else {
                    Move(90, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(135, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(135, backwardSpeed_);
                    RoadFollowerWaitForCenter();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    Move(45, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(45, turningSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            // turn backward
            else if (LFStatus == 0b11111) {
                ++offTrackCount_;
                controllerState_ = ControllerState::kTurningBackward;
                if (loggedState_ != controllerState_) {
                    log_.LogInfo() << "Turning backward";
                    loggedState_ = controllerState_;
                }
                if (offTrackCount_ > maxOffTrackCount_) {
                    if (turningAngle_ == 90) {
                        tmpAngle_ = 90 - turningMax_;
                    } else {
                        tmpAngle_ = (turningAngle_ - 90) / abs(90 - turningAngle_);
                        tmpAngle_ = tmpAngle_ * turningMax_;
                        tmpAngle_ = tmpAngle_ + 90;
                    }
                    Move(90, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(tmpAngle_, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    Move(tmpAngle_, backwardSpeed_);
                    RoadFollowerWaitForCenter();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(tmpAngle_, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    Move(turningAngle_, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    Move(turningAngle_, forwardSpeed_);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            } else {
                offTrackCount_ = 0;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}

apd::rest::Router ControllerActivity::CreateRouter()
{
    ara::core::String p("/status");
    apd::rest::Pattern statusPattern(p.c_str());

    apd::rest::Route getStatusRoute{apd::rest::RequestMethod::kGet,
        statusPattern,
        [this](const apd::rest::Route& route, const apd::rest::ServerRequest& request, apd::rest::ServerReply& reply)
            -> apd::rest::Route::Upshot { return ReturnCurrentValues(route, request, reply); }};

    ara::core::String ctrl("/mode");
    apd::rest::Pattern controlPattern(ctrl.c_str());
    apd::rest::Route putControlRoute{apd::rest::RequestMethod::kPut,
        controlPattern,
        [this](const apd::rest::Route& route, const apd::rest::ServerRequest& request, apd::rest::ServerReply& reply)
            -> apd::rest::Route::Upshot { return PutControlHandler(route, request, reply); }};

    ara::core::String cfg("/config");
    apd::rest::Pattern configPattern(cfg.c_str());

    apd::rest::Route getConfigRoute{apd::rest::RequestMethod::kGet,
        configPattern,
        [this](const apd::rest::Route& route, const apd::rest::ServerRequest& request, apd::rest::ServerReply& reply)
            -> apd::rest::Route::Upshot { return ReturnCurrentConfig(route, request, reply); }};

    apd::rest::Route putConfigRoute{apd::rest::RequestMethod::kPut,
        configPattern,
        [this](const apd::rest::Route& route, const apd::rest::ServerRequest& request, apd::rest::ServerReply& reply)
            -> apd::rest::Route::Upshot { return PutConfigHandler(route, request, reply); }};
    log_.LogInfo() << "Initialized rest server";
    return apd::rest::Router{getStatusRoute, getConfigRoute, putConfigRoute, putControlRoute};
}

apd::rest::Route::Upshot ControllerActivity::PutControlHandler(const apd::rest::Route& route,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply& reply)
{
    (void)route;
    (void)reply;
    (void)request;
    const auto& rootObj = request.GetObject().get();
    if (rootObj.HasField("mode")) {
        auto it = rootObj.Find("mode");
        auto& value = (*it).GetValue();
        ara::core::String mode{""};
        apd::rest::ogm::Visit(&value, [&mode](apd::rest::ogm::String* str) { mode = str->GetValue(); });
        if (mode == "follow_road") {
            controllerMode_ = ControllerMode::kFollowRoad;
        } else if (mode == "stop") {
            controllerMode_ = ControllerMode::kStop;
        }
    }
    reply.Send(apd::rest::ogm::Object::Make());
    return apd::rest::Route::Upshot::Accept;
}

apd::rest::Route::Upshot ControllerActivity::PutConfigHandler(const apd::rest::Route& route,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply& reply)
{
    (void)route;
    (void)reply;

    const auto& rootObj = request.GetObject().get();
    if (rootObj.HasField("forward_speed")) {
        auto it = rootObj.Find("forward_speed");
        auto& value = (*it).GetValue();
        int64_t forward_speed = -1;
        apd::rest::ogm::Visit(
            &value, [&forward_speed](apd::rest::ogm::Int* integer) { forward_speed = integer->GetValue(); });
        if (forward_speed != forwardSpeed_) {
            log_.LogInfo() << "New forward speed" << forward_speed << " (old: " << forwardSpeed_ << ")";
        }
        forwardSpeed_ = forward_speed;
    }
    if (rootObj.HasField("backward_speed")) {
        auto it = rootObj.Find("backward_speed");
        auto& value = (*it).GetValue();
        int64_t backward_speed = -1;
        apd::rest::ogm::Visit(
            &value, [&backward_speed](apd::rest::ogm::Int* integer) { backward_speed = integer->GetValue() * -1; });
        if (backward_speed != backwardSpeed_) {
            log_.LogInfo() << "New backward speed" << backward_speed << " (old: " << backwardSpeed_ << ")";
        }
        backwardSpeed_ = backward_speed;
    }
    if (rootObj.HasField("turning_speed")) {
        auto it = rootObj.Find("turning_speed");
        auto& value = (*it).GetValue();
        int64_t turning_speed = -1;
        apd::rest::ogm::Visit(
            &value, [&turning_speed](apd::rest::ogm::Int* integer) { turning_speed = integer->GetValue(); });
        if (turning_speed != turningSpeed_) {
            log_.LogInfo() << "New turning speed" << turning_speed << " (old: " << turningSpeed_ << ")";
        }
        turningSpeed_ = turning_speed;
    }
    if (rootObj.HasField("detection_threshold")) {
        auto it = rootObj.Find("detection_threshold");
        auto& value = (*it).GetValue();
        int64_t detection_threshold = -1;
        apd::rest::ogm::Visit(&value,
            [&detection_threshold](apd::rest::ogm::Int* integer) { detection_threshold = integer->GetValue(); });
        if (detection_threshold != detectionThreshold_) {
            log_.LogInfo() << "New detection threshold " << detection_threshold << " (old: " << detectionThreshold_
                           << ")";
        }
        detectionThreshold_ = detection_threshold;
    }
    reply.Send(apd::rest::ogm::Object::Make());
    return apd::rest::Route::Upshot::Accept;
}

apd::rest::Route::Upshot ControllerActivity::ReturnCurrentConfig(const apd::rest::Route& route,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply& reply)
{
    (void)route;
    (void)request;

    apd::rest::Pointer<apd::rest::ogm::Object> rootObj = apd::rest::ogm::Object::Make();

    rootObj->Insert(apd::rest::ogm::Field::Make("forward_speed", apd::rest::ogm::Int::Make(forwardSpeed_)));
    rootObj->Insert(apd::rest::ogm::Field::Make("turning_speed", apd::rest::ogm::Int::Make(turningSpeed_)));
    rootObj->Insert(apd::rest::ogm::Field::Make("backward_speed", apd::rest::ogm::Int::Make(abs(backwardSpeed_))));
    rootObj->Insert(apd::rest::ogm::Field::Make("detection_threshold", apd::rest::ogm::Int::Make(detectionThreshold_)));
    auto laneArray = apd::rest::ogm::Array::Make();
    for (const uint16_t value : LFStatusRaw_) {
        laneArray->Append(apd::rest::ogm::Int::Make(value));
    }
    rootObj->Insert(apd::rest::ogm::Field::Make("lane_raw", std::move(laneArray)));
    reply.Send(std::move(rootObj));
    return apd::rest::Route::Upshot::Accept;
}

ara::core::String ControllerActivity::StateToString(ControllerState state)
{
    switch (state) {
    case ControllerState::kInitial:
        return "initial";
    case ControllerState::kForward:
        return "forward";
    case ControllerState::kForwardLeft:
        return "turn_left";
    case ControllerState::kForwardLeftSharp:
        return "turn_left_sharp";
    case ControllerState::kForwardRight:
        return "turn_right";
    case ControllerState::kForwardRightSharp:
        return "turn_right_sharp";
    case ControllerState::kBackwardLeft:
        return "backward_left";
    case ControllerState::kBackwardRight:
        return "backward_right";
    case ControllerState::kTurningBackward:
        return "turning_backward";
    case ControllerState::kTurning:
        return "turning";
    case ControllerState::kObstacle:
        return "obstacle";
    case ControllerState::kSensorFailure:
        return "sensor_failure";
    case ControllerState::kStopped:
        return "stopped";
    default:
        return "default";
    }
}

apd::rest::Route::Upshot ControllerActivity::ReturnCurrentValues(const apd::rest::Route& route,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply& reply)
{
    (void)route;
    (void)request;

    apd::rest::Pointer<apd::rest::ogm::Object> rootObj = apd::rest::ogm::Object::Make();
    apd::rest::Pointer<apd::rest::ogm::Array> updatesArray = apd::rest::ogm::Array::Make();

    rootObj->Insert(apd::rest::ogm::Field::Make("angle", apd::rest::ogm::Int::Make(turningAngle_)));
    auto laneArray = apd::rest::ogm::Array::Make();
    uint8_t lfstatus = LFStatusBool_;
    for (int i = 4; i >= 0; i--) {
        laneArray->Append(apd::rest::ogm::Int::Make((lfstatus >> i) & 1));
    }
    rootObj->Insert(apd::rest::ogm::Field::Make("lane", std::move(laneArray)));
    auto laneArrayRaw = apd::rest::ogm::Array::Make();
    for (const uint16_t value : LFStatusRaw_) {
        laneArrayRaw->Append(apd::rest::ogm::Int::Make(value));
    }
    rootObj->Insert(apd::rest::ogm::Field::Make("lane_raw", std::move(laneArrayRaw)));
#ifdef ENABLE_ULTRASONIC
    rootObj->Insert(apd::rest::ogm::Field::Make("obstacle", apd::rest::ogm::Int::Make(distance_)));
#else
    rootObj->Insert(apd::rest::ogm::Field::Make("obstacle", apd::rest::ogm::Int::Make(-1)));
#endif
    rootObj->Insert(apd::rest::ogm::Field::Make("odo", apd::rest::ogm::Int::Make(12345)));
    rootObj->Insert(apd::rest::ogm::Field::Make("speed", apd::rest::ogm::Int::Make(abs(currentSpeed_))));
    rootObj->Insert(
        apd::rest::ogm::Field::Make("state", apd::rest::ogm::String::Make(StateToString(controllerState_).c_str())));
    reply.Send(std::move(rootObj));
    return apd::rest::Route::Upshot::Accept;
}

}  // namespace controller
}  // namespace picar
}  // namespace apd
