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
    #define HAS_GPIOG 1
    #define HAS_GPIOH 1
    #define HAS_GPIOJ 1
    #define HAS_GPIOK 1
    #define HAS_GPIOL 1
#else
    #error "Unsuported/unconfigured board"
#endif

#if defined(HAS_GPIOA)
    using PINA_  = Register<sfr_io8(0x00)>;
    using DDRA_  = Register<sfr_io8(0x00 + 1)>;
    using PORTA_ = Register<sfr_io8(0x00 + 2)>;
    using GPIO_A = IOPort<PINA_, DDRA_, PORTA_>;
#endif

#if defined(HAS_GPIOB)
    using PINB_  = Register<sfr_io8(0x03)>;
    using DDRB_  = Register<sfr_io8(0x03 + 1)>;
    using PORTB_ = Register<sfr_io8(0x03 + 2)>;
    using GPIO_B = IOPort<PINB_, DDRB_, PORTB_>;
#endif

#if defined(HAS_GPIOC)
    using PINC_  = Register<sfr_io8(0x06)>;
    using DDRC_  = Register<sfr_io8(0x06 + 1)>;
    using PORTC_ = Register<sfr_io8(0x06 + 2)>;
    using GPIO_C = IOPort<PINC_, DDRC_, PORTC_>;
#endif

#if defined(HAS_GPIOD)
    using PIND_  = Register<sfr_io8(0x09)>;
    using DDRD_  = Register<sfr_io8(0x09 + 1)>;
    using PORTD_ = Register<sfr_io8(0x09 + 2)>;
    using GPIO_D = IOPort<PIND_, DDRD_, PORTD_>;
#endif

#if defined(HAS_GPIOE)
    using PINE_  = Register<sfr_io8(0x0C)>;
    using DDRE_  = Register<sfr_io8(0x0C + 1)>;
    using PORTE_ = Register<sfr_io8(0x0C + 2)>;
    using GPIO_E = IOPort<PINE_, DDRE_, PORTE_>;
#endif

#if defined(HAS_GPIOF)
    using PINF_  = Register<sfr_io8(0x0F)>;
    using DDRF_  = Register<sfr_io8(0x0F + 1)>;
    using PORTF_ = Register<sfr_io8(0x0F + 2)>;
    using GPIO_F = IOPort<PINF_, DDRF_, PORTF_>;
#endif

#if defined(HAS_GPIOG)
    using PING_  = Register<sfr_io8(0x12)>;
    using DDRG_  = Register<sfr_io8(0x12 + 1)>;
    using PORTG_ = Register<sfr_io8(0x12 + 2)>;
    using GPIO_G = IOPort<PING_, DDRG_, PORTG_>;
#endif

#if defined(HAS_GPIOH)
    using PINT_  = Register<0x102>;
    using DDRT_  = Register<0x102 + 1>;
    using PORTT_ = Register<0x102 + 2>;
    using GPIO_T = IOPort<PINT_, DDRT_, PORTT_>;
#endif

#if defined(HAS_GPIOJ)
    using PINJ_  = Register<0x103>;
    using DDRJ_  = Register<0x103 + 1>;
    using PORTJ_ = Register<0x103 + 2>;
    using GPIO_J = IOPort<PINJ_, DDRJ_, PORTJ_>;
#endif

#if defined(HAS_GPIOK)
    using PINK_  = Register<0x106>;
    using DDRK_  = Register<0x106 + 1>;
    using PORTK_ = Register<0x106 + 2>;
    using GPIO_K = IOPort<PINK_, DDRK_, PORTK_>;
#endif

#if defined(HAS_GPIOL)
    using PINL_  = Register<109>;
    using DDRL_  = Register<109 + 1>;
    using PORTL_ = Register<109 + 2>;
    using GPIO_L = IOPort<PINL_, DDRL_, PORTL_>;
#endif

template <typename Writable>
constexpr void set_mode(PortMode mode) {
    Writable::write(mode);
}

}; // namespace HAL