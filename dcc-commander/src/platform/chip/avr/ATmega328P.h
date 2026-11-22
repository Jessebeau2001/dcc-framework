#pragma once
#include "../../arch/avr/io.h"

// This one can be found on the Arduino UNO board
// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

namespace plaform::avr {
    using GPIO_B = GPIO<sfr_io8(0x03)>;
    using GPIO_C = GPIO<sfr_io8(0x06)>;
    using GPIO_D = GPIO<sfr_io8(0x09)>;
} // namespace HAL