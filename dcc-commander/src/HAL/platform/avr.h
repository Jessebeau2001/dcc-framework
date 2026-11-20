#pragma once
#include <hal/chip.h>

namespace bitbang {
    #if defined(ARDUINO_AVR_UNO)
        using BangPin = HAL::GPIO_B::Pin<0>; // Uno pin 8
    #elif defined(ARDUINO_AVR_MEGA2560)
        using BangPin = HAL::GPIO_H::Pin<5>; // Mega pin 8
    #else
        #error "Unsuported/unconfigured board pinout"
    #endif
} // bitbang