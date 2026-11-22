#pragma once

#if defined(__AVR_ATmega328P__)
    #include "chip/avr/ATmega328P.h"
#elif defined(__AVR_ATmega2560__)
    #include "chip/avr/ATmega2560.h"
#endif