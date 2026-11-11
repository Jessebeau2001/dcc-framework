#pragma once

#include <types.h>

using namespace types;


template<u8 Index, typename T = u8>
struct BitView {
    static constexpr u8 MASK = 1 << Index;

    static constexpr bool is_set(u8 value) {
        return (value & MASK) != 0;
    }
};

template<u8 Index, typename T = u8>
struct BitRef {
    using View = BitView<Index>;

    u8& target;

    bool is_set() {
        return View::is_set(target);
    }
};