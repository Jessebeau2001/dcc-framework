#pragma once
#include <hal/io.hpp>

namespace platform::avr {
using HAL::DefaultIOPolicy;
using HAL::Register;
using HAL::IOPort;

struct AvrIOPolicy : HAL::DefaultIOPolicy {
    template <typename IO>
    static constexpr void flip() {
        IO::PIN::write(1);
    }
};

template <uintptr_t Address>
using GPIO = IOPort<
    Register<Address,     u8>,
    Register<Address + 1, u8>,
    Register<Address + 2, u8>,
    AvrIOPolicy>;

constexpr uintptr_t SFR_OFFSET = 0x20;

constexpr uintptr_t sfr_io8(uintptr_t io_addr) {
    return io_addr + SFR_OFFSET;
}

}