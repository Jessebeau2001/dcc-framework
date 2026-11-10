#pragma once

#include <types.h>
#include <stdint.h>

using namespace types;

namespace detail {

template <uintptr_t Address>
struct RegisterBase {
    static constexpr uintptr_t address = Address;

protected:
    static inline u8 read() {
        return *reinterpret_cast<volatile u8*>(Address);
    }

    static inline void write(u8 value) {
        *reinterpret_cast<volatile u8*>(Address) = value;
    }
};

}

template <uintptr_t Address>
struct ReadonlyRegister : private detail::RegisterBase<Address> {
    using Base = detail::RegisterBase<Address>;

    static inline u8 read() {
        return Base::read();
    }
};