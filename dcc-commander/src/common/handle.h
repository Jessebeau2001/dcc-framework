#pragma once

#include <types.h>
#include <stddef.h>

using namespace types;


template<size_t Index, typename T = u8>
struct BitView {
    static_assert(Index >= 0, "Bit index cannot be negative");
    static_assert(Index < (sizeof(T) * 8), "Bit Index is out of range for type");

    static constexpr T MASK = static_cast<T>(1) << Index;

    static constexpr bool is_set(T value) {
        return (value & MASK) != 0;
    }

    static constexpr void set(T& value) {
        value |= MASK;
    }

    static constexpr void clear(T& value) {
        value &= ~MASK;
    }
};


template<typename Base, size_t Index>
struct BitHandle {
    using BaseType = typename Base::ValueType;

    static_assert(Index >= 0, "Bit index cannot be negative");
    static_assert(Index < (sizeof(BaseType) * 8), "Bit Index is out of range for type");
    
    static constexpr BaseType MASK = static_cast<BaseType>(1) << Index;

    static constexpr bool read() {
        return (Base::read() & MASK) != 0;
    }

    static constexpr void write(bool value) {
        if (value) {
            Base::write(Base::read() | MASK);
        } else {
            Base::write(Base::read() & ~MASK);
        }
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