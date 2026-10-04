#pragma once

#include <stdint.h>
#include <stddef.h>

namespace bitbang {
    void init();
    bool write_frame(const uint8_t* data, size_t length);
    size_t available_for_write();

    inline bool write_byte(uint8_t byte) {
        return write_frame(&byte, 1);
    }
}
