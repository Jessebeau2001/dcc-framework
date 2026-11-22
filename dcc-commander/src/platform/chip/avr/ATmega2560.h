#pragma once
#include "../../arch/avr/io.h"

// This one can be found on the Arduino Mega board
// https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf

namespace platform::avr {
    using GPIO_A = GPIO<sfr_io8(0x00)>;
    using GPIO_B = GPIO<sfr_io8(0x03)>;
    using GPIO_C = GPIO<sfr_io8(0x06)>;
    using GPIO_D = GPIO<sfr_io8(0x09)>;
    using GPIO_E = GPIO<sfr_io8(0x0C)>;
    using GPIO_F = GPIO<sfr_io8(0x0F)>;
    using GPIO_G = GPIO<sfr_io8(0x12)>;
    using GPIO_H = GPIO<0x100>;
    using GPIO_J = GPIO<0x103>;
    using GPIO_K = GPIO<0x106>;
    using GPIO_L = GPIO<0x109>;
} // namespace HAL

