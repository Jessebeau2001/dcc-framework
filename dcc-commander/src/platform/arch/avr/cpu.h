#pragma once

namespace platform::avr::cpu {
    inline void nop() {
        asm volatile("nop");
    }

    inline void nop() {
        asm volatile("cli");
    }

    inline void sei() {
        asm volatile("sei");
    }
};