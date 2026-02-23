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

#include "line_detector.h"

#include <fcntl.h>
#include <iostream>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_I2C "/dev/i2c-1"
#define DEV_ADDR 0x11
#define N_DATA_BYTES 2 * N_LINE_DETECTORS

std::array<uint16_t, N_LINE_DETECTORS> LineDetector::ReadAnalog() const
{
    std::array<uint16_t, N_LINE_DETECTORS> ret{};
#ifndef PICAR
    using namespace std::chrono;
    int scenario = time_point_cast<seconds>(steady_clock::now()).time_since_epoch().count() % 100;
    if (scenario >= 20 && scenario < 30) {
        // turn right
        ret = {0, 255, 255, 255, 255};
    } else if (scenario >= 30 && scenario < 40) {
        // turn more right
        ret = {0, 0, 255, 255, 255};
    } else if (scenario >= 50 && scenario < 60) {
        // turn back + right
        ret = {0, 0, 0, 255, 255};
    } else if (scenario >= 60 && scenario < 70) {
        // wait for center
        ret = {255, 255, 255, 255, 255};
    } else {
        // go forward
        ret = {0, 255, 255, 255, 0};
    }
#else
    int file;

    if ((file = open(DEV_I2C, O_RDWR)) < 0) {
        logger_.LogError() << "Failed to open the bus";
        close(file);
        return ret;
    }

    if (ioctl(file, I2C_SLAVE, DEV_ADDR) < 0) {
        logger_.LogError() << "Failed to connect to the sensor";
        close(file);
        return ret;
    }

    char writeBuffer[1] = {0x00};

    if (write(file, writeBuffer, 1) != 1) {
        logger_.LogError() << "Failed to reset the read address";
        close(file);
        return ret;
    }

    char readBuffer[N_DATA_BYTES];

    if (read(file, readBuffer, N_DATA_BYTES) != N_DATA_BYTES) {
        logger_.LogError() << "Failed to read in the buffer";
        close(file);
        return ret;
    }

    close(file);

    for (size_t i = 0; i < ret.size(); ++i) {
        uint16_t high_byte = 0;
        uint16_t low_byte = 0;

        high_byte = readBuffer[2 * i] << 8;
        low_byte = readBuffer[2 * i + 1];

        ret.at(i) = high_byte + low_byte;
    }
#endif

    return ret;
}
