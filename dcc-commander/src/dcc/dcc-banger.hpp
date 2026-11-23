#pragma once
#include <types.h>
#include <common/bit_ring_buffer.hpp>
#include <hal/io.hpp>
#include <util/atomic.h>

namespace dcc {
using types::u8;
using types::u16;
using types::u32;

enum class TxState : uint8_t {
    RISE,
    FALL,
    EXIT
};

template <u16 BitHighUs, u16 BitLowUs, u32 CPUFreq, u16 Prescaler>
struct ConfigurableTimings {
    static constexpr u16 PRESCALER		= Prescaler; // Prescaler divider (1, 8, 64, 256, 1024)
    static constexpr u16 BIT_HIGH_US 	= BitHighUs;
    static constexpr u16 BIT_LOW_US  	= BitLowUs;

    // Formula: (us * f_cpu) / (1000000UL * prescaler) - 1
    static constexpr u32 compute_ticks(u32 f_cpu, u16 prescaler, u16 us) {
        return static_cast<u32>((static_cast<u64>(us) * f_cpu) / (1'000'000UL * prescaler) - 1);
    }

    static constexpr u16 BIT_HIGH_TICKS	= compute_ticks(CPUFreq, PRESCALER, BIT_HIGH_US);
    static constexpr u16 BIT_LOW_TICKS	= compute_ticks(CPUFreq, PRESCALER, BIT_LOW_US);
    
    static_assert(compute_ticks(CPUFreq, PRESCALER, BIT_HIGH_US) <= UINT16_MAX,
                  "BIT_HIGH_TICKS overflows 16-bit range!");
    static_assert(compute_ticks(CPUFreq, PRESCALER, BIT_LOW_US) <= UINT16_MAX,
                  "BIT_LOW_TICKS overflows 16-bit range!");
};

using DefaultTimings = ConfigurableTimings<58U, 100U, F_CPU, 8U>;

template <typename HWHandle, typename Timings>
class Bitbanger {
    BitRingBuffer<128> 	tx_buf;
    volatile TxState    tx_state        = TxState::RISE;
    volatile bool    	bit_to_write    = 0;

    inline void set_channel_compare() {
        HWHandle::set_compare(bit_to_write ? Timings::BIT_HIGH_TICKS : Timings::BIT_LOW_TICKS);
    }

    inline void kickstart() {
        if (HWHandle::is_routine_active())
            return;
        
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        bit_to_write    = tx_buf.pop_bit();
        tx_state        = TxState::RISE;
        set_channel_compare();
        HWHandle::arm_timer();
		}
    }

public:
    static inline void configure() {
        HWHandle::configure();
    }

    inline void routine() {
        HWHandle::flip_pin();

        switch (tx_state) {
        case TxState::RISE:
            set_channel_compare();
            tx_state = TxState::FALL;
            break;

        case TxState::FALL:
            if (tx_buf.is_bit_available_for_read()) {
                bit_to_write = tx_buf.pop_bit();;
                tx_state = TxState::RISE;
            } else {
                tx_state = TxState::EXIT;
            }
            break;

        case TxState::EXIT:
            HWHandle::disable_interupt();
            break;
        }
    }

    size_t available() const {
        return tx_buf.bytes_available_for_write();
    }

    void write_byte(u8 byte) {
        tx_buf.push_byte(byte);
        kickstart();
    }

    void write_bit(bool bit) {
        tx_buf.push_bit(bit);
        kickstart();
    }
};
    
} // namespace dcc

