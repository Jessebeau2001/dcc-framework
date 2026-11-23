#pragma once

#include <types.h>
namespace HAL {
using namespace types;

template <uintptr_t Address, typename T>
struct Register {
    using ValueType = T;
    static constexpr uintptr_t ADDRESS = Address;

    static inline T read() {
        return *reinterpret_cast<volatile T*>(Address);
    }

    static inline void write(T value) {
        *reinterpret_cast<volatile T*>(Address) = value;
    }
};

template <uintptr_t Address, typename T>
struct ReadonlyRegister : private Register<Address, T> {
    using Base      = Register<Address, T>;
    using ValueType = typename Base::ValueType;

    static inline T read() {
        return Base::read();
    }
};

template <uintptr_t Address, typename T>
struct WriteonlyRegister : private Register<Address, T> {
    using Base      = Register<Address, T>;
    using ValueType = typename Base::ValueType;

    static inline void write(T value) {
        Base::write(value);
    }
};

template <typename T>
static constexpr T calc_shift(T m) {
    T s = 0;
    while (m != 0 && (m & 1) == 0) { m >>= 1; ++s; }
    return s;
}

// Note is always RMW
template<typename Reg, typename Reg::ValueType Mask>
struct WriteLens {
    using BaseType = typename Reg::ValueType;

    static constexpr BaseType MASK = Mask;
    static constexpr BaseType SHIFT = calc_shift(Mask);

    static constexpr void write(BaseType value) {
        BaseType current = Reg::read();
        current &= ~MASK;
        current |= (value << SHIFT) & MASK;
        Reg::write(current);
    }
};

} // namespace HAL
