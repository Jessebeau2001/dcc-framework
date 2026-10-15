#pragma once

#include <types.h>
#include <stddef.h>

namespace bitbang {
    using namespace types;

    void begin();
    size_t bytes_available_for_write();

    struct Writer {
        static void write_byte(u8 byte);
        static void write_bit(bool bit);

        static size_t available() {
            return bytes_available_for_write();
        }
    };
}
