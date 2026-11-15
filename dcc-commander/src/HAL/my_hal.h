#pragma once

#include <HAL/register.h>

namespace HAL {

constexpr uintptr_t SFR_OFFSET = 0x20;

constexpr uintptr_t sfr_io8(uintptr_t io_addr) {
    return io_addr + SFR_OFFSET;
}

template <uintptr_t Address>
using GPIO = IOPort<
    Register<Address>,
    Register<Address + 1>,
    Register<Address + 2>
>;

#if defined(ARDUINO_AVR_UNO)
    // https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

    #define HAS_GPIOB 1
    #define HAS_GPIOC 1
    #define HAS_GPIOD 1
#elif defined(ARDUINO_AVR_MEGA2560)
    // https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf
    
    #define HAS_GPIOA 1
    #define HAS_GPIOB 1
    #define HAS_GPIOC 1
    #define HAS_GPIOD 1
    #define HAS_GPIOE 1
    #define HAS_GPIOF 1
    #define HAS_GPIOG 1
    #define HAS_GPIOH 1
    #define HAS_GPIOJ 1
    #define HAS_GPIOK 1
    #define HAS_GPIOL 1
#else
    #error "Unsuported/unconfigured board"
#endif

#if defined(HAS_GPIOA)
    using GPIO_A = GPIO<sfr_io8(0x00)>;
#endif

#if defined(HAS_GPIOB)
    using GPIO_B = GPIO<sfr_io8(0x03)>;
#endif

#if defined(HAS_GPIOC)
    using GPIO_C = GPIO<sfr_io8(0x06)>;
#endif

#if defined(HAS_GPIOD)
    using GPIO_D = GPIO<sfr_io8(0x09)>;
#endif

#if defined(HAS_GPIOE)
    using GPIO_E = GPIO<sfr_io8(0x0C)>;
#endif

#if defined(HAS_GPIOF)
    using GPIO_F = GPIO<sfr_io8(0x0F)>;
#endif

#if defined(HAS_GPIOG)
    using GPIO_G = GPIO<sfr_io8(0x12)>;
#endif

#if defined(HAS_GPIOH)
    using GPIO_H = GPIO<0x100>;
#endif

#if defined(HAS_GPIOJ)
    using GPIO_J = GPIO<0x103>;
#endif

#if defined(HAS_GPIOK)
    using GPIO_K = GPIO<0x106>;
#endif

#if defined(HAS_GPIOL)
    using GPIO_L = GPIO<0x109>;
#endif

}; // namespace HAL
