#pragma once

#include <HAL/register.h>

namespace HAL {

#if defined(ARDUINO_AVR_UNO)
    // https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
    
    // Digital IO Register Definitions (read state / write toggle)
    using _PINB = Register<0x03>;
    using _PINC = Register<0x06>;
    using _PIND = Register<0x09>;

    // Data Direction Register Definitions (read mode config / write mode config)
    using _DDRB = Register<0x04>;
    using _DDRC = Register<0x07>;
    using _DDRD = Register<0x0A>;

    // Data Register Definitions (read state config / write state config)
    using _PORTB = Register<0x05>;
    using _PORTC = Register<0x08>;
    using _PORTD = Register<0x0B>;
#elif defined(ARDUINO_AVR_MEGA2560)
    // Digital IO Register Definitions (read state / write toggle)
    using _PINA = Register<0x00>;
    using _PINB = Register<0x03>;
    using _PINC = Register<0x06>;
    using _PIND = Register<0x09>;
    using _PINE = Register<0x0C>;
    using _PINF = Register<0x0F>;

    // Data Direction Register Definitions (read mode config / write mode config)
    using _DDRA = Register<0x01>;
    using _DDRB = Register<0x04>;
    using _DDRC = Register<0x07>;
    using _DDRD = Register<0x0A>;
    using _DDRE = Register<0x0D>;
    using _DDRF = Register<0x10>;

    // Data Register Definitions (read state config / write state config)
    using _PORTA = Register<0x02>;
    using _PORTB = Register<0x05>;
    using _PORTC = Register<0x08>;
    using _PORTD = Register<0x0B>;
    using _PORTE = Register<0x0E>;
    using _PORTF = Register<0x11>;
#else
    #error "Unsuported/unconfigured board pinout"
#endif

}; // namespace HAL