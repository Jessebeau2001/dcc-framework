#include "bitbang.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "common/bit_ring_buffer.hpp"

namespace bitbang::detail {
/*  In hardware CTC mode (WGM12), Timer1 counts from 0 up to N.
    That yields N + 1 timer clock cycles in a full period hence the -1.

    Formula:
        Target Ticks = (Interval_in_seconds) * (Timer_Clock_Freq) - 1
        Target Ticks = (us / 1,000,000)      * (sys_clk / prescaler) - 1
*/
constexpr u32 compute_ticks(u32 f_cpu, u16 prescaler, u16 us) {
    // Formula: (us * f_cpu) / (1000000UL * prescaler) - 1
    return static_cast<u32>((static_cast<u64>(us) * f_cpu) / (1'000'000UL * prescaler) - 1);
}

enum class TxState : uint8_t {
    RISE,
    FALL,
    EXIT
};

// System & Timer Configurations
constexpr u16 	    PRESCALER		= 8;		// Prescaler divider (1, 8, 64, 256, 1024)
constexpr u16	    BIT_HIGH_US 	= 58;		// Bit high time in microseconds
constexpr u16	    BIT_LOW_US  	= 100;		// Bit low time in microseconds

// Computed timing constants
constexpr u16 	    BIT_HIGH_TICKS	= compute_ticks(F_CPU, PRESCALER, BIT_HIGH_US);
constexpr u16 	    BIT_LOW_TICKS	= compute_ticks(F_CPU, PRESCALER, BIT_LOW_US);

static_assert(compute_ticks(F_CPU, PRESCALER, BIT_HIGH_US) <= UINT16_MAX,
              "BIT_HIGH_TICKS overflows 16-bit range!");

static_assert(compute_ticks(F_CPU, PRESCALER, BIT_LOW_US) <= UINT16_MAX,
              "BIT_LOW_TICKS overflows 16-bit range!");

// ISR state & Ring Buffer
BitRingBuffer<128> 	tx_buf;
volatile TxState    tx_state        = TxState::RISE;
volatile bool    	current_bit		= 0;

inline void timer_set_compare() {
    OCR1A = current_bit ? BIT_HIGH_TICKS : BIT_LOW_TICKS;
}

inline bool timer_is_busy() {
    return (TIMSK1 & (1 << OCIE1A)) != 0;
}

inline void timer_stop() {
    TIMSK1 &= ~(1 << OCIE1A);
}

inline void flip_tx_pins() {
	PINB = (1 << PB2) | (1 << PB3);
}

inline void kickstart_ISR() {
	if (!timer_is_busy()) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			// Load first byte and reset state
			current_bit     = tx_buf.pop_bit();
            tx_state        = TxState::RISE;
            
            timer_set_compare();
			TCNT1 = 0;               // Start cleanly from tick 0
			TIFR1 = (1 << OCF1A);    // Clear stale interrupt flags
			TIMSK1 |= (1 << OCIE1A); // Enable interrupt
		}
	}
}

} // namespace bitbang::detail

namespace bitbang {
using namespace detail;

void begin() {
    cli();							// Disable global interrupts during setup

    DDRB |= (1 << PB2); 			// Set PB2 as output
    DDRB |= (1 << PB3);
    // PINB  = (1 << PB3); 			// Flip PB3 opposed to PB2

    TCCR1A = 0;						// Reset Timer 1 Registers
    TCCR1B = 0;
    TCNT1  = 0;

    TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (Clear Timer on Compare Match.)
    TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8

    sei(); 							// Enable global interrupts
}

size_t bytes_available_for_write() {
    return tx_buf.bytes_available_for_write();
}

void Writer::write_byte(u8 byte) {
    tx_buf.push_byte(byte);
    kickstart_ISR();
}

void Writer::write_bit(bool bit) {
    tx_buf.push_bit(bit);
    kickstart_ISR();
}

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

ISR(TIMER1_COMPA_vect) {
    using namespace bitbang::detail;

    switch (tx_state) {
    case TxState::RISE:
        flip_tx_pins();
        timer_set_compare();
        tx_state = TxState::FALL;
        break;

    case TxState::FALL:
        flip_tx_pins();

        if (tx_buf.is_bit_available_for_read()) {
            current_bit = tx_buf.pop_bit();;
            tx_state = TxState::RISE;
        } else {
            tx_state = TxState::EXIT;
        }

        break;

    case TxState::EXIT:
        flip_tx_pins();
        timer_stop();
        break;
    }
}