#pragma once

#include <types.h>
#include <stdint.h>
#include <common/handle.h>

using namespace types;

using RegType = unsigned char;

namespace HAL {

enum PortMode {
    MODE_INPUT = 0,
    MODE_OUTPUT = 1
};

} // namespace HAL


template <uintptr_t Address, typename T = RegType>
struct Register {
    using ValueType = T;
    static constexpr uintptr_t address = Address;

    static inline T read() {
        return *reinterpret_cast<volatile T*>(Address);
    }

    static inline void write(T value) {
        *reinterpret_cast<volatile T*>(Address) = value;
    }
};

template <uintptr_t Address, typename T = RegType>
struct ReadonlyRegister : private Register<Address, T> {
    using Base      = Register<Address, T>;
    using ValueType = typename Base::ValueType;

    static inline T read() {
        return Base::read();
    }
};

template <uintptr_t Address, typename T = RegType>
struct WriteonlyRegister : private Register<Address, T> {
    using Base      = Register<Address, T>;
    using ValueType = typename Base::ValueType;

    static inline void write(T value) {
        Base::write(value);
    }
};

template <typename Pin, typename DDR, typename Port>
struct IOPort {
    using ValueType = typename DDR::ValueType;
    using PinX  = Pin;
    using DDRX  = DDR;
    using PortX = Port;
};

template <typename IO, size_t Index>
struct IOPin {
    using ValueType = typename IO::ValueType;
    using Handle = BitHandle<Index, ValueType>;

    static constexpr void set_mode(HAL::PortMode mode) {
        if (mode == HAL::PortMode::MODE_INPUT) {
            Handle::template set<typename IO::DDRX>();
        } else {
            Handle::template clear<typename IO::DDRX>();
        }
    }

    static constexpr ValueType read() {
        return Handle::template read<typename IO::PINX>();
    }

    static constexpr ValueType write(ValueType value) {
        if (value > 1) {
            Handle::template set<typename IO::PortX>();
        } else {
            Handle::template clear<typename IO::PortX>();
        }
    }
};