#include "bitbang.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

namespace bitbang {

namespace {
	constexpr uint16_t compute_ticks(uint16_t us, uint32_t f_cpu, uint16_t prescaler) {
		// Formula: Target Ticks - 1
    	// Target Ticks = (Interval_in_seconds) * (Timer_Clock_Freq)
    	//              = (us / 1,000,000) * (sys_clk / prescaler)
    	// Rearranged for integer arithmetic without losing precision:
        return static_cast<uint16_t>(((f_cpu / 1000000UL) * us) / prescaler) - 1;
    }

	// NOTE: Certain configurations yield a scalar of 0, this won't work obviously
	constexpr uint16_t compute_tick_scalar(uint32_t f_cpu, uint16_t prescaler) {
		return static_cast<uint16_t>(f_cpu / (1000000UL * prescaler));
	}

	constexpr uint16_t compute_ticks(uint16_t us, uint16_t tick_scalar) {
		// In hardware CTC mode (WGM12), Timer 1 counts from 0 up to N.
		// That yields N + 1 timer clock cycles in a full period hence to -1.
		return (us * tick_scalar) - 1;
	}

    // System & Timer Configurations
    constexpr uint32_t 	F_CPU_HZ	= F_CPU;	// Chip speed
    constexpr uint16_t 	PRESCALER	= 8;		// Prescaler divider (1, 8, 64, 256, 1024)
    constexpr uint8_t 	BITBANG_PIN = PB2;		// Arduino Pin 10 maps to PB2 on ATmega328P

	constexpr uint16_t 	TICK_SCALAR	= compute_tick_scalar(F_CPU_HZ, PRESCALER);
	static_assert(TICK_SCALAR > 0, "Tick scalar cannot yield 0. The current prescalar to clockspeed configuration is unsuported.");

	// Ring buffer setup - TODO: Do we wrap in in struct with helpers or keep as is?
	constexpr 	uint8_t TX_BUFFER_SIZE	= 32; // Must be a power of 2
	constexpr 	uint8_t TX_BUFFER_MASK	= TX_BUFFER_SIZE - 1;

	// Ring buffer state
	volatile 	uint8_t tx_buf[TX_BUFFER_SIZE];
	volatile 	uint8_t head 			= 0; // Written by main code
	volatile 	uint8_t tail 			= 0; // Read by ISR

	// ISR state
	volatile 	bool    is_busy      	= false;
} // namespace

inline size_t available_for_write() {
	const uint8_t h = head;
	const uint8_t t = tail;
	
	if (h >= t) {
		return (TX_BUFFER_SIZE - 1) - (h - t);
	} else {
		return (t - h) - 1;
	}
}

void init() {
	cli();							// Disable global interrupts during setup

	DDRB |= (1 << BITBANG_PIN);		// Set BITBANG_PIN as output

	TCCR1A = 0;						// Reset Timer 1 Registers
	TCCR1B = 0;
	TCNT1  = 0;

	TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (Clear Timer on Compare Match.)
	TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8

	sei(); 							// Enable global interrupts
}

bool write_frame(const uint8_t* data, size_t length) {
	if (length == 0 || length > available_for_write()) {
		return false; 
	}

	uint8_t current = head;
	for (size_t i = 0; i < length; i++) {
		tx_buf[current] = data[i];
		current = (current + 1) & TX_BUFFER_MASK;
	}
	// Updated head pointer once at the end
	head = current;

	// Restart ISR if idle
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (!is_busy) {
			is_busy = true;

			// Load initial duration
			uint8_t initial_us = tx_buf[tail];
			tail = (tail + 1) & TX_BUFFER_MASK;
			
			OCR1A = compute_ticks(initial_us, TICK_SCALAR);

			TCNT1 = 0;               // Start cleanly from tick 0
			TIFR1 = (1 << OCF1A);    // Clear stale interrupt flags
			TIMSK1 |= (1 << OCIE1A); // Enable interrupt
		}
	}

	return true;
}

} // bitbang

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
	PINB = (1 << bitbang::BITBANG_PIN);

	// 2. Load the NEXT pulse duration
    if (bitbang::head != bitbang::tail) {
        uint8_t next_us = bitbang::tx_buf[bitbang::tail];
        bitbang::tail = (bitbang::tail + 1) & bitbang::TX_BUFFER_MASK;

        // Update target compare match for upcoming pulse
        OCR1A = bitbang::compute_ticks(next_us, bitbang::TICK_SCALAR);
    } else {
        // Frame complete: stop interrupts
        TIMSK1 &= ~(1 << OCIE1A);
        bitbang::is_busy = false;
    }
}
