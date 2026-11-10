#pragma once

#include <types.h>
#include <stdint.h>

using namespace types;

namespace detail {

template <uintptr_t Address, typename T>
struct RegisterBase {
    using ValueType = T;
    static constexpr uintptr_t address = Address;

protected:
    static inline T read() {
        return *reinterpret_cast<volatile T*>(Address);
    }

    static inline void write(T value) {
        *reinterpret_cast<volatile T*>(Address) = value;
    }
};

}

template <uintptr_t Address, typename T = u8>
struct ReadonlyRegister : private detail::RegisterBase<Address, T> {
    using Base = detail::RegisterBase<Address, T>;
    using ValueType = typename Base::ValueType;

    static inline T read() {
        return Base::read();
    }
};