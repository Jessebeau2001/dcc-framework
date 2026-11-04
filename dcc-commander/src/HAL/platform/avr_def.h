#pragma once
#include <avr/io.h>

namespace bitbang::pins {
#if defined(ARDUINO_AVR_UNO)

inline void setup_tx() {
    DDRB |= (1 << PB0);     // Uno pin 8
}

inline void flip_tx() {
    PINB = (1 << PB2) | (1 << PB3);
}

#elif defined(ARDUINO_AVR_MEGA2560)

inline void setup_tx() {
    DDRH |= (1 << PH5);     // Mega Pin 8
}

inline void flip_tx() {
    PINH = (1 << PH5);
}

#else
#error "Unsuported/unconfigured board pinout"
#endif
} // bitbang::pins