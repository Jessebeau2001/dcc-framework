#pragma once

#if defined(ARDUINO_AVR_UNO)
    #include "board/arduino/uno.h"
#elif defined(ARDUINO_AVR_MEGA2560)
    #include "board/arduino/mega.h"
#endif
