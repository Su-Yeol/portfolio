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

#include "pwm_i2c.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_I2C "/dev/i2c-1"
#define DEV_ADDR 0x14
#define PRESCALER_ADDRESS(channel) (channel / 0x04 + 0x38)
#define PERIOD_ADDRESS(channel) (channel / 0x04 + 0x3C)

using namespace std;

pwm_i2c::pwm_i2c(uint8_t channel, unsigned int lowValue, unsigned int highValue)
    : pwm(lowValue, highValue)
    , channel_(channel)
{
    setPeriod();
}

int pwm_i2c::write_i2c(uint8_t address, uint8_t high_byte, uint8_t low_byte)
{
    int file, ret;

    if ((file = open(DEV_I2C, O_RDWR)) < 0) {
        // logger_.LogError() << "Failed to open the bus";
        close(file);
        return file;
    }

    if ((ret = ioctl(file, I2C_SLAVE, DEV_ADDR)) < 0) {
        // logger_.LogError() << "Failed to connect to the robot hat";
        close(file);
        return ret;
    }

    char writeBuffer[3] = {address, high_byte, low_byte};

    if ((ret = write(file, writeBuffer, 3)) != 3) {
        // logger_.LogError() << "Failed to write to " << address;
        return ret;
    }
    std::cout << "Writing " << ret << ":" << std::hex << static_cast<int>(address) << "  " << std::hex
              << static_cast<int>(high_byte) << "  " << std::hex << static_cast<int>(low_byte) << std::endl;
    close(file);
    return 0;
}

int pwm_i2c::setPeriod()
{
    // PWM timer operates at 72MHz
    // 60 Hz = 72 MHz / (1095+1) / (1095+1)
    // ^ desired frequency
    //          ^ SoC frequency
    //                   ^ period value (0x0447)
    //                              ^ prescaler value (0x0447)
    int retval = write_i2c(PERIOD_ADDRESS(channel_), 0x04, 0x47);
    if (retval < 0)
        return retval;
    return write_i2c(PRESCALER_ADDRESS(channel_), 0x04, 0x47);
}

int pwm_i2c::setDutyCycle(unsigned int dutyCycle)
{
    return write_i2c(channel_, dutyCycle >> 8, dutyCycle & 0xFF);
}

pwm_i2c::~pwm_i2c()
{
    setDutyCycle(0);
}
