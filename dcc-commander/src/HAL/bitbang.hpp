#pragma once

#include <stdint.h>
#include <stddef.h>

namespace bitbang {
    void PREFILL_FOR_TEST();
    void begin();
    size_t available_for_write();

    struct Writer {
        static void write_byte(uint8_t byte);
        static void write_bit(bool bit);
    };
}
