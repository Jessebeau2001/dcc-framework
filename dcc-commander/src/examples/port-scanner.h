#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <Arduino.h>

#define PORTB_ADDR 0x05
#define DDRB_ADDR 0x04

auto scanner = PortScanner<ReadonlyRegister<PORTB_ADDR>>();

void setup() {
    *(volatile u8*)DDRB_ADDR = 0x00;
}

void loop() {
    scanner.update();
}