#pragma once

#include <stdint.h>
#include <stddef.h>

namespace bitbang {
    void begin();
    size_t bytes_available_for_write();

    struct Writer {
        static void write_byte(uint8_t byte);
        static void write_bit(bool bit);

        static size_t available() {
            return bytes_available_for_write();
        }
    };
}
