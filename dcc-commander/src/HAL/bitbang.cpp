#include "bitbang.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

namespace {
    // System & Timer Configurations
    constexpr uint32_t 	F_CPU_HZ	= F_CPU;	// Chip speed
    constexpr uint16_t 	PRESCALER	= 8;		// Prescaler divider (1, 8, 64, 256, 1024)
    constexpr uint16_t 	INTERVAL_US	= 58;		// Desired delay in microseconds
    constexpr uint8_t 	BITBANG_PIN = PB2;		// Arduino Pin 10 maps to PB2 on ATmega328P

	// Ring buffer setup
	constexpr 	uint8_t TX_BUFFER_SIZE	= 32; // Must be a power of 2
	constexpr 	uint8_t TX_BUFFER_MASK	= TX_BUFFER_SIZE - 1;

	// Ring buffer state
	volatile 	uint8_t tx_buf[TX_BUFFER_SIZE];
	volatile 	uint8_t head 			= 0; // Written by main code
	volatile 	uint8_t tail 			= 0; // Read by ISR

	// ISR state
	volatile 	uint8_t	current_byte 	= 0;
	volatile 	uint8_t bit_index    	= 0; 
	volatile 	bool    is_busy      	= false;

	constexpr uint16_t COMPUTE_OCR1A(uint16_t us, uint32_t f_cpu, uint16_t prescaler) {
		// Formula: Target Ticks - 1
    	// Target Ticks = (Interval_in_seconds) * (Timer_Clock_Freq)
    	//              = (us / 1,000,000) * (sys_clk / prescaler)
    	// Rearranged for integer arithmetic without losing precision:
        return static_cast<uint16_t>(((f_cpu / 1000000UL) * us) / prescaler) - 1;
    }

    constexpr uint16_t TIMER_COMPARE_VALUE = COMPUTE_OCR1A(INTERVAL_US, F_CPU_HZ, PRESCALER);
}

namespace bitbang {
	constexpr uint8_t TX_BUFFER_SIZE = 32; // Must be a power of 2
	constexpr uint8_t TX_BUFFER_MASK = TX_BUFFER_SIZE - 1;

	volatile uint8_t tx_buf[TX_BUFFER_SIZE];
	volatile uint8_t head = 0; // Written by main code
	volatile uint8_t tail = 0; // Read by ISR

	volatile uint8_t  current_byte = 0;
	volatile uint8_t  bit_index    = 0; 
	volatile bool     is_busy      = false;

	inline size_t available_for_write() {
		const uint8_t h = head;
		const uint8_t t = tail;
		
		if (h >= t) {
			return (TX_BUFFER_SIZE - 1) - (h - t);
		} else {
			return (t - h) - 1;
		}
	}
}

void bitbang::init() {
	cli();								// Disable global interrupts during setup

	DDRB |= (1 << BITBANG_PIN);			// Set BITBANG_PIN as output

	/*
		TC  = Timer/Counter
		1   = Timer Number 1
		CR  = Control Register (Contains configuration)
			- TCCR1A: Configures output pin actions & lower PWM mode bits
			- TCCR1B: Configures prescaler, clock source & upper mode bits
			- (Split across A & B because 16 setting bits exceed the 8-bit bus)
		CNT = Counter Register - holds the actual live 16-bit tick count
	*/

	// Reset Timer 1 Registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	/*
		OCR1A = Output Compare Register 1A. This is the counter target
		When TCNT1 reaches OCR1A, three actions occur automatically in the same clock cycle:
		1. TCNT1 resets back to 0.
		2. The TIMER1_COMPA_vect interrupt flag fires
		3. The counting cycle repeats.
	*/

	TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (Clear Timer on Compare Match.)
	TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8

	// OCR1A = TIMER_COMPARE_VALUE;
	// TIMSK1 |= (1 << OCIE1A); 		// Enable Timer1 output compare A interrupt

	sei(); 							// Enable global interrupts
}

bool bitbang::write_frame(const uint8_t* data, size_t length) {
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

			// Prime the initial state so the ISR reads bit 0 immediately
            current_byte = tx_buf[tail];
            tail = (tail + 1) & TX_BUFFER_MASK;
            bit_index = 0;

            TCNT1 = 0;               // Start cleanly from tick 0
            TIFR1 = (1 << OCF1A);    // Clear stale interrupt flags
            TIMSK1 |= (1 << OCIE1A); // Enable interrupt
		}
    }

    return true;
}

ISR(TIMER1_COMPA_vect) {
	if (bitbang::bit_index == 0) { // Start bit
        PORTB &= ~(1 << BITBANG_PIN);
        bitbang::bit_index++;
    } else if (bitbang::bit_index <= 8) { // Data bits (LSB first)
        if (bitbang::current_byte & 0x01) {
            PORTB |= (1 << BITBANG_PIN);
        } else {
            PORTB &= ~(1 << BITBANG_PIN);
        }
        bitbang::current_byte >>= 1;
        bitbang::bit_index++;
    } else { // Stop bit
        PORTB |= (1 << BITBANG_PIN);
        
        // Fetch next byte from ring buffer
        if (bitbang::head != bitbang::tail) {
            bitbang::current_byte = bitbang::tx_buf[bitbang::tail];
            bitbang::tail = (bitbang::tail + 1) & bitbang::TX_BUFFER_MASK;
            bitbang::bit_index = 0; // Reset state for next byte
        } else {
            // No more data: stop timer interrupts to save CPU cycles
            TIMSK1 &= ~(1 << OCIE1A);
            bitbang::is_busy = false;
        }
    }
}
