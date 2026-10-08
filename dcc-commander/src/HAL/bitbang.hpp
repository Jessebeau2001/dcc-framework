#pragma once

#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "common/bit_ring_buffer.hpp"

namespace bitbang::detail {
// Formula: Target Ticks - 1
// Target Ticks = (Interval_in_seconds) * (Timer_Clock_Freq)
//              = (us / 1,000,000)      * (sys_clk / prescaler)

// NOTE: Certain configurations yield a scalar of 0, this won't work obviously
constexpr uint16_t compute_tick_scalar(uint32_t f_cpu, uint16_t prescaler) {
    return static_cast<uint16_t>(f_cpu / (1000000UL * prescaler));
}

constexpr uint16_t compute_ticks(uint16_t us, uint16_t tick_scalar) {
    // In hardware CTC mode (WGM12), Timer1 counts from 0 up to N.
    // That yields N + 1 timer clock cycles in a full period hence the -1.
    return (us * tick_scalar) - 1;
}

inline bool timer_is_busy() {
    return (TIMSK1 & (1 << OCIE1A)) != 0;
}

inline void timer_stop() {
    TIMSK1 &= ~(1 << OCIE1A);
}

// System & Timer Configurations
constexpr uint32_t 	F_CPU_HZ		= F_CPU;	// Chip speed
constexpr uint16_t 	PRESCALER		= 8;		// Prescaler divider (1, 8, 64, 256, 1024)
constexpr uint8_t	BIT_HIGH_US 	= 58;		// Bit high time in microseconds
constexpr uint8_t	BIT_LOW_US  	= 100;		// Bit low time in microseconds

constexpr uint16_t 	TICK_SCALAR		= compute_tick_scalar(F_CPU_HZ, PRESCALER);
static_assert(TICK_SCALAR > 0, "Tick scalar cannot yield 0. The current prescalar to clockspeed configuration is unsuported.");

// Computed timing constants
constexpr uint8_t 	BIT_HIGH_TICKS	= compute_ticks(BIT_HIGH_US, TICK_SCALAR);
constexpr uint8_t 	BIT_LOW_TICKS	= compute_ticks(BIT_LOW_US, TICK_SCALAR);

// ISR state & Ring Buffer
BitRingBuffer<128> 	tx_buf;
volatile uint8_t 	current_byte	= 0;
volatile uint8_t 	current_bit		= 0;
volatile bool 		was_risen 		= false;

inline uint8_t compute_current_ticks() {
    bool is_bit_set = (current_byte & (1 << (7 - current_bit))) != 0;
    return compute_ticks(is_bit_set ? BIT_HIGH_TICKS : BIT_LOW_TICKS, TICK_SCALAR);
}

inline void kickstart_ISR() {
	if (!timer_is_busy()) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			// Load byte and reset state
			current_byte = tx_buf.pop_byte();
			current_bit = 0;
			was_risen = false;

			// Configure initial timing
			OCR1A = compute_current_ticks();

			TCNT1 = 0;               // Start cleanly from tick 0
			TIFR1 = (1 << OCF1A);    // Clear stale interrupt flags
			TIMSK1 |= (1 << OCIE1A); // Enable interrupt
		}
	}
}

} // namespace bitbang::detail

namespace bitbang {
using namespace detail;

void init() {
    cli();							// Disable global interrupts during setup

    DDRB |= (1 << PB2); 			// Set BITBANG_PIN as output
    DDRB |= (1 << PB3);
    PINB  = (1 << PB3); 			// Flip PB3 opposed to PB2

    TCCR1A = 0;						// Reset Timer 1 Registers
    TCCR1B = 0;
    TCNT1  = 0;

    TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (Clear Timer on Compare Match.)
    TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8

    sei(); 							// Enable global interrupts
}

size_t available_for_write() {
    return tx_buf.available_for_write();
}

struct Writer {
    static inline void write_byte(uint8_t byte) {
        tx_buf.push_byte(byte);
        kickstart_ISR();
    }

    static inline void write_bit(bool bit) {
        tx_buf.push_bit(bit);
        kickstart_ISR();
    }
};

} // namespace bitbang

/*	What registers do we use:
	TC  = Timer/Counter
	1   = Timer Number 1
	CR  = Control Register (Contains configuration)
		- TCCR1A: Configures output pin actions & lower PWM mode bits
		- TCCR1B: Configures prescaler, clock source & upper mode bits
		- (Split across A & B because 16 setting bits exceed the 8-bit bus)
	CNT = Counter Register - holds the actual live 16-bit tick count
	OCR1A = Output Compare Register 1A. This is the counter target

	When TCNT1 reaches OCR1A, three actions occur automatically in the same clock cycle:
	1. TCNT1 resets back to 0.
	2. The TIMER1_COMPA_vect interrupt flag fires
	3. The counting cycle repeats.
*/

// entry somewhere else in the program:
// Configures first byte
// Resets bit index to 0
// Resets was_risen to false
// Preloads first timing

// This results in the first two ISR triggers:
// 1. Rise
// 2. Lower then load next bit

ISR(TIMER1_COMPA_vect) {
	using namespace bitbang::detail; 

	PINB = (1 << PB2) | (1 << PB3);		// Always flip the pins

	was_risen = !was_risen;
		
	if (was_risen) {					// Falling edge, prepare next timing
		current_bit += 1;
		
		if (current_bit >= 8) {
			if (tx_buf.available_for_read()) {
				current_byte = tx_buf.pop_byte();
				current_bit = 0;
			} else {
				timer_stop(); 			// Buffer exhausted, stop interrupts
				return;					// Early exit
			}
		}
		OCR1A = compute_current_ticks();
	}
}

