#pragma once
#include "../../arch/avr/io.h"

// This one can be found on the Arduino UNO board
// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

namespace platform::avr {
    using GPIO_B = GPIO<sfr_io8(0x03)>;
    using GPIO_C = GPIO<sfr_io8(0x06)>;
    using GPIO_D = GPIO<sfr_io8(0x09)>;

    using Timer0 = Timer8<sfr_io8(0x24), sfr_io8(0x15), 0x6E>;
    using Timer1 = Timer16<       0x80 , sfr_io8(0x16), 0x6F>;
    using Timer2 = Timer8<        0xB0 , sfr_io8(0x17), 0x70>;
    
} // namespace HAL