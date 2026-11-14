#pragma once

#include <types.h>
#include <stdint.h>
#include <common/handle.h>

using namespace types;

using RegType = unsigned char;

namespace HAL {

enum class IOMode {
    Input,
    Output,
    InputPullup
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

template <typename PinReg, typename DDRReg, typename PortReg>
struct IOPort {
    using ValueType = typename DDRReg::ValueType;
    using Pin  = PinReg;
    using DDR  = DDRReg;
    using Port = PortReg;
};

template <typename IO, size_t Index>
struct IOPin {
    using ValueType = typename IO::ValueType;
    using Pin = BitHandle<typename IO::Pin, Index>;
    using DDR = BitHandle<typename IO::DDR, Index>;
    using Port = BitHandle<typename IO::Port, Index>;

    static constexpr bool read() {
        return Pin::read();
    }

    static constexpr void write(bool value) {
        Port::write(value);
    }
};

template<typename>
inline constexpr bool always_false_v = false;

template<typename IO>
constexpr void configure_io(HAL::IOMode mode) {
    using ValueType = typename IO::ValueType;
    constexpr ValueType HIGH = ~ValueType{0};
    constexpr ValueType LOW = ValueType{0};

    switch (mode) {
    case HAL::IOMode::Input:
        IO::DDR::write(HIGH);
        break;
    case HAL::IOMode::Output:
        IO::DDR::write(LOW);
        break;
    case HAL::IOMode::InputPullup:
        IO::DDR::write(LOW);    // Pin Input
        IO::Port::write(HIGH);  // Input pullup on port
    }
}
