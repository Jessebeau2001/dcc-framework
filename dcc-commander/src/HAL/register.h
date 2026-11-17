#pragma once

#include <types.h>
#include <stdint.h>
#include <common/handle.h>
#include <concepts.h>

using namespace types;

using RegType = unsigned char;

template <uintptr_t Address, typename T = RegType>
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

template <typename IO, size_t Index>
struct IOPin {
    using ValueType = bool;

    using PinReg = typename IO::PIN;
    using DDRReg = typename IO::DDR;
    using PortReg = typename IO::PORT;

    using PIN = BitHandle<PinReg, Index>;
    using DDR = BitHandle<DDRReg, Index>;
    using PORT = BitHandle<PortReg, Index>;
};

template <typename PinReg, typename DDRReg, typename PortReg>
struct IOPort {
    using ValueType = typename PinReg::ValueType;

    static_assert(traits::is_same_v<ValueType, typename DDRReg::ValueType> &&
                  traits::is_same_v<ValueType, typename PortReg::ValueType>,
                  "Can only define port for types with same width");

    using PIN  = PinReg;
    using DDR  = DDRReg;
    using PORT = PortReg;

    template<u8 N>
    using Pin = IOPin<IOPort, N>;
};

namespace HAL {

enum class IOMode {
    Input,
    Output,
    InputPullup
};

template<typename IO>
constexpr void io_write(typename IO::ValueType value) {
    IO::PORT::write(value);
}

template<typename IO>
constexpr typename IO::ValueType io_read() {
    return IO::PIN::read();
}

template<typename IO>
constexpr void io_configure(HAL::IOMode mode) {
    using ValueType = typename IO::ValueType;
    constexpr ValueType HIGH = ~ValueType{0};
    constexpr ValueType LOW = ValueType{0};

    switch (mode) {
    case HAL::IOMode::Input:
        IO::DDR::write(LOW);    // Pin Input
        IO::PORT::write(LOW);   // Pullup disabled
        break;
    case HAL::IOMode::Output:
        IO::DDR::write(HIGH);   // Pin Output
        break;
    case HAL::IOMode::InputPullup:
        IO::DDR::write(LOW);    // Pin Input
        IO::PORT::write(HIGH);  // Pullup enabled
    }
}

} // namespace HAL
