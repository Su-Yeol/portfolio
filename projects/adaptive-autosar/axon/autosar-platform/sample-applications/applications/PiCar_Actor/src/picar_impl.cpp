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

#include "picar_impl.h"
#include "pwm_i2c.h"
#include "pwm_pca9685.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_I2C "/dev/i2c-1"

PicarImpl::PicarImpl()
{
    int file;
    uint8_t test = 0x00;
    if ((file = open(DEV_I2C, O_RDWR)) < 0) {
        close(file);
        throw;
    }

    if ((ioctl(file, I2C_SLAVE, 0x14)) == 0 && write(file, &test, 1) == 1) {
        // this is a picar x
        pwm_steering_servo = std::make_unique<pwm_i2c>(0x22, 0x40, 0x80);
        pwm_left_back_wheel = std::make_unique<pwm_i2c>(0x2C, 0, 0x0447);
        pwm_right_back_wheel = std::make_unique<pwm_i2c>(0x2D, 0, 0x0447);

        gpio_left_back_wheel = std::make_unique<gpio>(23, true);
        gpio_right_back_wheel = std::make_unique<gpio>(24, false);
    } else if ((ioctl(file, I2C_SLAVE, 0x48)) == 0 && write(file, &test, 1) == 1) {
        // this is a picar s
        pwm_steering_servo = std::make_unique<pwm_pca9685>(0, 1050000, 1950000);
        pwm_left_back_wheel = std::make_unique<pwm_pca9685>(4, 0, PWM_PERIOD);
        pwm_right_back_wheel = std::make_unique<pwm_pca9685>(5, 0, PWM_PERIOD);

        gpio_left_back_wheel = std::make_unique<gpio>(27);
        gpio_right_back_wheel = std::make_unique<gpio>(17);
    } else {
        throw;
    }
    close(file);
    gpio_left_back_wheel->setDirection("out");
    gpio_right_back_wheel->setDirection("out");
}

int PicarImpl::turn(int angle)
{
    if (angle < 45) {
        angle = 45;
    } else if (angle > 135) {
        angle = 135;
    }

    return pwm_steering_servo->setDutyCycle(pwm_steering_servo->highValue_
        - (pwm_steering_servo->highValue_ - pwm_steering_servo->lowValue_) * (angle - 45) / (135 - 45));
}
int PicarImpl::forward()
{
    gpio_left_back_wheel->setValue(1);
    gpio_right_back_wheel->setValue(1);

    return 0;
}

int PicarImpl::backward()
{
    gpio_left_back_wheel->setValue(0);
    gpio_right_back_wheel->setValue(0);

    return 0;
}

int PicarImpl::speed(int speed)
{
    // prevent div by zero
    int dutyCycle = (speed == 0 ? speed : pwm_left_back_wheel->highValue_ / 100. * speed);

    pwm_left_back_wheel->setDutyCycle(dutyCycle);
    pwm_right_back_wheel->setDutyCycle(dutyCycle);

    return 0;
}

PicarImpl::~PicarImpl()
{
    pwm_left_back_wheel->setDutyCycle(0);
    pwm_right_back_wheel->setDutyCycle(0);
    pwm_steering_servo->setDutyCycle(
        (pwm_steering_servo->highValue_ - pwm_steering_servo->lowValue_) / 2 + pwm_steering_servo->lowValue_);
}
