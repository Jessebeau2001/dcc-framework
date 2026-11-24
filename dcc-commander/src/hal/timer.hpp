#pragma once

#include <types.h>
#include "core/register.hpp"


namespace HAL {
using types::u8;

// Idk about all these enums yet, probably keep as helping had for caller, but move to BSP
// Then caller can fully decide themsevles how to get these value into register
enum class AvrWaveformGeneratorMode : u8 {
    Normal              = 0b000,
    PhaseCorrectPWM     = 0b001,
    CTC                 = 0b010,
    FastPWM             = 0b011,
    PhaseCorrectPWM_TOP = 0b101,
    FastPWM_TOP         = 0b111
};

enum class AvrCompareOutputMode : u8 {
    Normal = 0b00,
    Toggle = 0b01, // Does nothing when WGM02 not set
    Clear  = 0b10,
    Set    = 0b11
};

enum class AvrClockSelect : u8 {
    Off              = 0b000,
    CLK1             = 0b001,
    CLK8             = 0b010,
    CLK64            = 0b011,
    CLK256           = 0b100,
    CLK1024          = 0b101,
    External_Falling = 0b110,
    External_Rising  = 0b111
};

struct Timer0Policy {
    static constexpr u8 WGM0 = 0U;
    static constexpr u8 WGM1 = 1U;
    static constexpr u8 WGM2 = 3U;

    using WGM = AvrWaveformGeneratorMode;
    using COM = AvrCompareOutputMode;

    // Sucks dont like it
    template <typename TCCR>
    // requires TCCR::RegA,RegB
    static constexpr void wgm_set(WGM mode) {
        const auto code = static_cast<u8>(mode);

        u8 a = TCCR::A::read();
        u8 b = TCCR::B::read();

        a &= ~((1 << WGM1) | (1 << WGM0));
        b &= ~(1 << WGM2);

        a |= (code & 0b011);
        if (code & 0b100) { b |= (1 << WGM2); }

        TCCR::A::write(a);
        TCCR::B::write(b);
    }
};

template <typename RegA_, typename RegB_, typename Policy_>
struct AvrTCCR {
    using A = RegA_;
    using B = RegB_;
    
    // Note sure if I like this yet
    using Policy = Policy_;

    // Note sure if I like this yet
    using WGM01 = WriteLens<A, 0b00000011>;
    using WGM02 = WriteLens<B, 0b00001000>;
    using COMA  = WriteLens<A, 0b11000000>;
    using COMB  = WriteLens<A, 0b00110000>;
    using FOCA  = WriteLens<B, 0b10000000>;
    using FOCB  = WriteLens<B, 0b01000000>;
    using CS    = WriteLens<B, 0b00000111>;
};

// This sucks - really dont like it atm
template <typename TCCR_, typename TCNT_,
          typename OCRA_, typename OCRB_,
          typename TISMK_, typename TIFR_>
struct Timer {
    using TCCR = TCCR_;     // Control
    using TCNT = TCNT_;     // Counter
    using OCRA = OCRA_;     // Output Compare A
    using OCRB = OCRB_;     // Output Compare B
    using TIMSK = TISMK_;   // Interupt Mask
    using TIFR = TIFR_;     // Interupt Flag

    using WGM = typename TCCR::Policy::WGM;
    using COM = typename TCCR::Policy::COM;

    static constexpr void wgm_set(WGM mode) {
        TCCR::Policy::template wgm_set<TCCR>(mode);
    }
};

} // namespace HAL
