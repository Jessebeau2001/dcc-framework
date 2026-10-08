#pragma once

#include <stdint.h>
#include <stddef.h>

namespace bitbang {
    void init();
    size_t available_for_write();

    struct Writer {
        static inline void write_byte(uint8_t byte);
        static inline void write_bit(bool bit);
    };
}
