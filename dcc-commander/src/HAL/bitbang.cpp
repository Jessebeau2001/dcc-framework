#include "bitbang.h"

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

namespace {
    // System & Timer Configurations
    constexpr uint32_t F_CPU_HZ		= F_CPU; 	  // 16 MHz
    constexpr uint16_t PRESCALER	= 8;          // Prescaler divider (1, 8, 64, 256, 1024)

    constexpr uint16_t INTERVAL_US	= 58;         // Desired delay in microseconds

    // Arduino Pin 10 maps to PB2 on ATmega328P
    constexpr uint8_t BITBANG_PIN = PB2;

    constexpr uint16_t COMPUTE_OCR1A(uint16_t us, uint32_t f_cpu, uint16_t prescaler) {
		// Formula: Target Ticks - 1
    	// Target Ticks = (Interval_in_seconds) * (Timer_Clock_Freq)
    	//              = (us / 1,000,000) * (sys_clk / prescaler)
    	// Rearranged for integer arithmetic without losing precision:
        return static_cast<uint16_t>(((f_cpu / 1000000UL) * us) / prescaler) - 1;
    }

    constexpr uint16_t TIMER_COMPARE_VALUE = COMPUTE_OCR1A(INTERVAL_US, F_CPU_HZ, PRESCALER);
}

ISR(TIMER1_COMPA_vect) {
  // On AVR writing 1 to the input register causes a flip
  PINB = (1 << BITBANG_PIN);

  // --- ALTERNATIVE: State-machine bitbanging (e.g., transmitting data) ---
  /*
  static uint8_t bitIdx = 0;
  static uint8_t dataByte = 0xA5; // 0b10100101

  if (dataByte & (1 << bitIdx)) {
    PORTB |= (1 << BITBANG_PIN);  // Drive HIGH
  } else {
    PORTB &= ~(1 << BITBANG_PIN); // Drive LOW
  }

  if (++bitIdx >= 8) {
    bitIdx = 0;
    // Load next byte or disable interrupt
  }
  */
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

	OCR1A = TIMER_COMPARE_VALUE;
	
	TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (Clear Timer on Compare Match.)
	TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8
	TIMSK1 |= (1 << OCIE1A); 		// Enable Timer1 output compare A interrupt

	sei(); 							// Enable global interrupts
}