#include "bitbang.h"
#include <Arduino.h>
// #include <stdint.h>

constexpr uint8_t BITBANG_PIN = 10;
constexpr uint16_t INTERVAL_US = 10;

void bitbang::init() {
	// Disable global interrupts during setup
	cli();

	// Set BITBANG_PIN as output
	DDRB |= (1 << BITBANG_PIN);

	// Reset Timer 1 control registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	/*
	* Calculate Compare Value:
	* System Clock = 16 MHz
	* Prescaler = 8 -> Timer Clock = 2 MHz (0.5 us per tick)
	*
	* Formula: OCR1A = (INTERVAL_US * 2) - 1
	* For 10 us: (10 * 2) - 1 = 19 ticks
	*/
	OCR1A = (INTERVAL_US * 2) - 1;
	
	TCCR1B |= (1 << WGM12); 		// Turn on CTC mode (WGM12 = 1)
	TCCR1B |= (1 << CS11); 			// Set CS11 bit for Prescaler 8
	TIMSK1 |= (1 << OCIE1A); 		// Enable Timer1 output compare A interrupt

	sei(); 							// Enable global interrupts
}

ISR(TIMER1_COMPA_vect) {
  // Ultra-fast bit-toggle using PINB register (1 clock cycle / 62.5 ns)
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