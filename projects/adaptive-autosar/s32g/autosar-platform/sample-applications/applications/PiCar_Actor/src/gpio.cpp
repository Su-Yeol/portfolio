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

#include "gpio.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#define GPIO_PATH "/sys/class/gpio/"

gpio::gpio(unsigned int number, bool inverted)
    : number_{number}
    , inverted_{inverted}
{
    std::ostringstream s;

    s << "gpio" << number_;
    name_ = std::string(s.str());

    path_ = GPIO_PATH + name_ + "/";

    exportGpio();
}

int gpio::setDirection(const std::string& direction)
{
    return write(path_, "direction", direction);
}

int gpio::setValue(int value)
{
    if (inverted_) {
        value = (value + 1) % 2;
    }
    return write(path_, "value", value);
}

int gpio::getValue() const
{
    std::string input = read(path_, "value");

    if (inverted_) {
        return input == "0" ? 1 : 0;
    } else {
        return input == "0" ? 0 : 1;
    }
}

gpio::~gpio()
{
    write(path_, "value", 0);
}

int gpio::write(const std::string& path, const std::string& filename, const std::string& value)
{
    std::ofstream fs;

    fs.open((path + filename).c_str());
    if (!fs.is_open()) {
        perror("gpio: write failed to open file");
        return -1;
    }

    fs << value;
    fs.close();

    return 0;
}

int gpio::write(const std::string& path, const std::string& filename, int value)
{
    std::stringstream s;

    s << value;
    return write(path, filename, s.str());
}

std::string gpio::read(const std::string& path, const std::string& filename) const
{
    std::ifstream fs;

    fs.open((path + filename).c_str());
    if (!fs.is_open()) {
        perror("gpio: read failed to open file ");
    }

    std::string input;
    getline(fs, input);
    fs.close();

    return input;
}

int gpio::exportGpio()
{
    return write(GPIO_PATH, "export", number_);
}

int gpio::unexportGpio()
{
    return write(GPIO_PATH, "unexport", number_);
}
