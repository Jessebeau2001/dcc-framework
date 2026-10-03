#pragma once

#include <Arduino.h>

namespace {
    volatile uint32_t pulse_count = 0;
    unsigned long last_time = 0;
}

ISR(INT0_vect) {
    pulse_count++;
}

void setup() {
    EICRA |= (1 << ISC01) | (1 << ISC00); // Trigger INT0 on Rising Edge
    EIMSK |= (1 << INT0);                 // Enable INT0 interrupt
    DDRD  &= ~(1 << DDD2);                // Set Pin 2 (PD2) as Input

    Serial.begin(9600);
}

void loop() {
    unsigned long current_time = micros();
    if (current_time - last_time >= 1000000UL) {
        cli();
        uint32_t count = pulse_count;
        pulse_count = 0;
        sei();

        unsigned long elapsed = current_time - last_time;
        last_time = current_time;

        float frequency = (count * 1000000.0) / elapsed;

        Serial.print("Measured Frequency: ");
        Serial.print(frequency, 2);
        Serial.println(" Hz");
    }
}