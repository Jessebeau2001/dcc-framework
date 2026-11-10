#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <Arduino.h>

#define PORTB_ADDR 0x05
#define DDRB_ADDR 0x04

using MY_PORTB = ReadonlyRegister<PORTB_ADDR>;
using Scanner = PortScanner<MY_PORTB>;
Scanner::State sensor_state_a = Scanner::State();

void setup() {
    // No abstraction for this yet, just keep as is for simple example
    *(volatile u8*)DDRB_ADDR = 0x00;
}

void loop() {
    Scanner::update(sensor_state_a);
}