#include <Arduino.h>
#include "HAL/bitbang.h"

volatile uint32_t pulse_count = 0;

// External Interrupt on Pin 2 (INT0)
ISR(INT0_vect) {
    pulse_count++;
}

void setup_counter() {
    EICRA |= (1 << ISC01) | (1 << ISC00); // Trigger INT0 on Rising Edge
    EIMSK |= (1 << INT0);                 // Enable INT0 interrupt
    DDRD  &= ~(1 << DDD2);                // Set Pin 2 (PD2) as Input
}

void setup() {
    setup_counter();
    bitbang::init();
    Serial.begin(9600);
}

void loop() {
    pulse_count = 0;
    _delay_ms(1000); // Count pulses for exactly 1 second
    
    // Read count atomically
    cli();
    uint32_t count = pulse_count;
    sei();

    // For a 10 us toggle (20 us full wave period), frequency should be ~50,000 Hz
    Serial.print("Measured Frequency: ");
    Serial.print(count);
    Serial.println(" Hz");
}