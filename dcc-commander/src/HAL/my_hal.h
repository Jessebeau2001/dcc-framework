#pragma once

#include <HAL/register.h>

namespace HAL {

constexpr uintptr_t SFR_OFFSET = 0x20;

constexpr uintptr_t sfr_io8(uintptr_t io_addr) {
    return io_addr + SFR_OFFSET;
}

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
#else
    #error "Unsuported/unconfigured board"
#endif

#if defined(HAS_GPIOA)
    using PINA_  = Register<sfr_io8(0x00)>;
    using DDRA_  = Register<sfr_io8(0x00 + 1)>;
    using PORTA_ = Register<sfr_io8(0x00 + 2)>;
#endif

#if defined(HAS_GPIOB)
    using PINB_  = Register<sfr_io8(0x03)>;
    using DDRB_  = Register<sfr_io8(0x03 + 1)>;
    using PORTB_ = Register<sfr_io8(0x03 + 2)>;
#endif

#if defined(HAS_GPIOC)
    using PINC_  = Register<sfr_io8(0x06)>;
    using DDRC_  = Register<sfr_io8(0x06 + 1)>;
    using PORTC_ = Register<sfr_io8(0x06 + 2)>;
#endif

#if defined(HAS_GPIOD)
    using PIND_  = Register<sfr_io8(0x09)>;
    using DDRD_  = Register<sfr_io8(0x09 + 1)>;
    using PORTD_ = Register<sfr_io8(0x09 + 2)>;
#endif

#if defined(HAS_GPIOE)
    using PINE_  = Register<sfr_io8(0x0C)>;
    using DDRE_  = Register<sfr_io8(0x0C + 1)>;
    using PORTE_ = Register<sfr_io8(0x0C + 2)>;
#endif

#if defined(HAS_GPIOF)
    using PINF_  = Register<sfr_io8(0x0F)>;
    using DDRF_  = Register<sfr_io8(0x0F + 1)>;
    using PORTF_ = Register<sfr_io8(0x0F + 2)>;
#endif


enum PortMode {
    MODE_INPUT = 0,
    MODE_OUTPUT = 1
};

template <typename Writable>
constexpr void set_mode(PortMode mode) {
    Writable::write(mode);
}

}; // namespace HAL