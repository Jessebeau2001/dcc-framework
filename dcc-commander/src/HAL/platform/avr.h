#pragma once

#include <avr/io.h>

#if defined(ARDUINO_AVR_UNO)
    #define BITBANG_DDR     DDRB
    #define BITBANG_PINR    PINB
    #define BITBANG_PIN     PB0     // Uno pin 8
#elif defined(ARDUINO_AVR_MEGA2560)
    #define BITBANG_DDR     DDRH
    #define BITBANG_PINR    PINH
    #define BITBANG_PIN     PH5     // Mega pin 8
#else
    #error "Unsuported/unconfigured board pinout"
#endif

namespace bitbang::tx {
    inline void setup() { BITBANG_DDR |= (1 << BITBANG_PIN); }
    inline void flip()  { BITBANG_PINR = (1 << BITBANG_PIN); }
} // bitbang::tx