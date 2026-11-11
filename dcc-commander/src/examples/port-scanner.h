#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <common/handle.h>

#define PORTB_ADDR 0x05
#define DDRB_ADDR 0x04

using PORTB_IDR = ReadonlyRegister<PORTB_ADDR>;
using Sensors = PortScanner<PORTB_IDR>;
using SensorView = BitView<3>;

Sensors::State sensor_state_a = Sensors::State();

#include <Arduino.h>

void setup() {
    // No abstraction for this yet, just keep as is for simple example
    *(volatile u8*)DDRB_ADDR = 0x00;

    u8 a = 0;
    u8 b = 0;
    u8 c = 0;

    Station<1> my_station = Station<1>();
    StationTracker tracker = StationTracker(a, b, c);
    tracker.check_occupancy<1>(my_station);
}

void loop() {
    Sensors::update(sensor_state_a);
    if (SensorView::is_set(sensor_state_a.value)) {
        // Pin is flipped
    }
}