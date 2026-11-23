#pragma once

#include <hal/timer.hpp>
#include <platform/arch/avr/io.h>
#include <Arduino.h>

using platform::avr::sfr_io8;

using TCCR0A_ = HAL::Register<sfr_io8(0x24), u8>;
using TCCR0B_ = HAL::Register<sfr_io8(0x25), u8>;
using TCNT0_  = HAL::Register<sfr_io8(0x26), u8>;
using OCR0A_  = HAL::Register<sfr_io8(0x27), u8>;
using OCR0B_  = HAL::Register<sfr_io8(0x28), u8>;

using TIFR0_  = HAL::Register<sfr_io8(0x15), u8>;
using TIMSK0_ = HAL::Register<0x6E, u8>;

using TCCR0_ = HAL::AvrTCCR<
    TCCR0A_,
    TCCR0B_,
    HAL::Timer0Policy>;

using Timer0  = HAL::Timer<
    TCCR0_,
    TCNT0_,
    OCR0A_, OCR0B_,
    TIMSK0_, TIFR0_>;

void setup() {
    Timer0::wgm_set(Timer0::WGM::CTC);
    Timer0::OCRA::write(55);
}

void loop() {
    
}