#pragma once

#include <stdint.h>

namespace HAL {
    constexpr uintptr_t SFR_OFFSET = 0x20;

    constexpr uintptr_t sfr_io8(uintptr_t io_addr) {
        return io_addr + SFR_OFFSET;
    }
} // namespace HAL

