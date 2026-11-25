#pragma once
#include <types.h>

namespace util {
    using namespace types;

    template <u16 DelayMs>
    struct Clock {
        static constexpr u16 DELAY_MS = DelayMs;
        u32 previous_ms = 0;

        inline bool update(u32 current_ms) {
            if (current_ms - previous_ms < DelayMs)
                return false;
            previous_ms = current_ms;
            return true;
        }
    };
}
