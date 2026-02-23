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

#include "pwm_pca9685.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#define PWM_PATH "/sys/class/pwm/pwmchip0/"

using namespace std;

pwm_pca9685::pwm_pca9685(unsigned int number, unsigned int lowValue, unsigned int highValue)
    : pwm(lowValue, highValue)
    , number_{number}

{
    std::ostringstream s;

    s << "pwm" << number_;
    name_ = std::string(s.str());

    path_ = PWM_PATH + name_ + "/";

    exportPwm();
    setPeriod();
}

int pwm_pca9685::setPeriod()
{
    return write(path_, "period", PWM_PERIOD);
}

int pwm_pca9685::setDutyCycle(unsigned int dutyCycle)
{
    return write(path_, "duty_cycle", dutyCycle);
}

pwm_pca9685::~pwm_pca9685()
{
    setDutyCycle(0);
    unexportPwm();
}

int pwm_pca9685::write(const std::string& path, const std::string& filename, unsigned int value)
{
    std::ofstream fs;

    fs.open((path + filename).c_str());
    if (!fs.is_open()) {
        perror("pwm: write failed to open file");
        return -1;
    }

    fs << value;
    fs.close();

    return 0;
}

int pwm_pca9685::exportPwm()
{
    return write(PWM_PATH, "export", number_);
}

int pwm_pca9685::unexportPwm()
{
    return write(PWM_PATH, "unexport", number_);
}
