#pragma once
#include <hal/core/register.hpp>
#include <common/handle.h>
#include <traits.h>

namespace HAL {
using namespace types;

template<typename T>
constexpr T val_invert(T value) {
    return static_cast<T>(~value);
}

constexpr bool val_invert(bool value) {
    return !value;
}

struct DefaultIOPolicy {
    template <typename IO>
    static constexpr void flip() {
        // Generic read modify write
        IO::PORT::write(val_invert(IO::PORT::read()));
    }
};

template <typename IO, size_t Index>
struct IOPin {
    using ValueType = bool;
    using Policy = typename IO::Policy;
    
    using PIN = BitHandle<typename IO::PIN, Index>;
    using DDR = BitHandle<typename IO::DDR, Index>;
    using PORT = BitHandle<typename IO::PORT, Index>;
};

template <typename PinReg, typename DDRReg, typename PortReg,
          typename IOPolicy = DefaultIOPolicy>
struct IOPort {
    using ValueType = typename PinReg::ValueType;
    using Policy = IOPolicy;

    static_assert(traits::is_same_v<ValueType, typename DDRReg::ValueType> &&
                  traits::is_same_v<ValueType, typename PortReg::ValueType>,
                  "Can only define port for types with same width");

    using PIN  = PinReg;
    using DDR  = DDRReg;
    using PORT = PortReg;

    template<u8 N>
    using Pin = IOPin<IOPort, N>;
};

enum class IOMode {
    Input,
    Output,
    InputPullup
};

template<typename IO>
constexpr typename IO::ValueType io_read() {
    return IO::PIN::read();
}

template<typename IO>
constexpr void io_write(typename IO::ValueType value) {
    IO::PIN::write(1);
}

template<typename IO>
constexpr void io_flip() {
    IO::Policy::template flip<IO>();
}

template<typename IO>
static constexpr void io_configure(IOMode mode) {
    using ValueType = typename IO::ValueType;
    constexpr ValueType LOW = ValueType{0};
    constexpr ValueType HIGH = val_invert(LOW);

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