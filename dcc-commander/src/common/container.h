#pragma once

#include <types.h>

using namespace types;

template <typename T, typename Flag>
struct Container {

    T data;
    u8 header;

    inline bool is_set(Flag flag) const {
        return (header & static_cast<u8>(flag));
    }

    inline void set(Flag flag) {
        header |= static_cast<u8>(flag);
    }

    inline void clear(Flag flag) {
        header &= ~static_cast<u8>(flag);
    }
};