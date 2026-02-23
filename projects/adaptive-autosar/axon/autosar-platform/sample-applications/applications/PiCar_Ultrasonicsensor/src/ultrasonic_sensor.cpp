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

#include "ultrasonic_sensor.h"
#include "ultrasonic_sensor_hcsr04.h"
#include "ultrasonic_sensor_sfsr02.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_I2C "/dev/i2c-1"

#include <thread>

std::unique_ptr<UltrasonicSensor> UltrasonicSensor::CreateSensor()
{
    int file;
    // std::unique_ptr<UltraSonicSensor> sensor;
    uint8_t test = 0x00;
    if ((file = open(DEV_I2C, O_RDWR)) < 0) {
        close(file);
        return nullptr;
    }

    if ((ioctl(file, I2C_SLAVE, 0x14)) == 0 && write(file, &test, 1) == 1) {
        // this is a picar x
        close(file);
        return std::make_unique<UltrasonicSensorHCSR04>(27, 22);
    } else if ((ioctl(file, I2C_SLAVE, 0x48)) == 0 && write(file, &test, 1) == 1) {
        // this is a picar s
        close(file);
        return std::make_unique<UltrasonicSensorSFSR02>(20);
    }
    return nullptr;
}

int UltrasonicSensor::GetDistance(int nAvg) const
{
    int sum = 0;

    for (int i = 0; i < nAvg; ++i) {
        sum += distance();
    }

    return sum / nAvg;
}

int UltrasonicSensor::distance(gpio& gpioUltrasonicSensorTrigger, gpio& gpioUltrasonicSensorEcho) const
{
    gpioUltrasonicSensorTrigger.setDirection("out");
    gpioUltrasonicSensorTrigger.setValue(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    gpioUltrasonicSensorTrigger.setValue(1);
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    gpioUltrasonicSensorTrigger.setValue(0);
    gpioUltrasonicSensorEcho.setDirection("in");

    auto pulseStart = std::chrono::steady_clock::now();
    auto pulseEnd = std::chrono::steady_clock::now();
    auto timeoutStart = std::chrono::steady_clock::now();

    while (gpioUltrasonicSensorEcho.getValue() == 0) {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        pulseStart = std::chrono::steady_clock::now();
        if (pulseStart - timeoutStart > timeout_) {
            return -1;
        }
    }

    while (gpioUltrasonicSensorEcho.getValue() == 1) {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        pulseEnd = std::chrono::steady_clock::now();
        if (pulseStart - timeoutStart > timeout_) {
            return -1;
        }
    }

    int distance{0};

    auto pulseWidth = std::chrono::duration_cast<std::chrono::microseconds>(pulseEnd - pulseStart).count();

    distance = pulseWidth * 343. / 20000.;

    if (distance >= 0) {
        return distance;
    } else {
        return -1;
    }
}
