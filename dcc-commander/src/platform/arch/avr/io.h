#pragma once
#include <hal/io.hpp>
#include <hal/timer.hpp>

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

template<uintptr_t BaseAddr, uintptr_t TIFRAddr, uintptr_t TIMSKAddr>
struct Timer8 {
    using TCCRA = HAL::Register<BaseAddr + 0, u8>;
    using TCCRB = HAL::Register<BaseAddr + 1, u8>;
    using TCNT  = HAL::Register<BaseAddr + 2, u8>;
    using OCRA  = HAL::Register<BaseAddr + 3, u8>;
    using OCRB  = HAL::Register<BaseAddr + 4, u8>;

    using TIFR  = HAL::Register<TIFRAddr,     u8>;
    using TIMSK = HAL::Register<TIMSKAddr,    u8>;
};

template<uintptr_t BaseAddr, uintptr_t TIFRAddr, uintptr_t TIMSKAddr>
struct Timer16 {
    using TCCRA = HAL::Register<BaseAddr + 0,  u8>;
    using TCCRB = HAL::Register<BaseAddr + 1,  u8>;
    using TCCRC = HAL::Register<BaseAddr + 2,  u8>;
    //                                   + 3,  reserved
    using TCNT  = HAL::Register<BaseAddr + 4,  u16>;
    using ICR   = HAL::Register<BaseAddr + 6,  u16>;
    using OCRA  = HAL::Register<BaseAddr + 8,  u16>;
    using OCRB  = HAL::Register<BaseAddr + 10, u16>;
    using OCRC  = HAL::Register<BaseAddr + 12, u16>;

    using TIFR  = HAL::Register<TIFRAddr,      u8>;
    using TIMSK = HAL::Register<TIMSKAddr,     u8>;
};

}