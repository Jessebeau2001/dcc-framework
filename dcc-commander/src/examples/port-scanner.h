#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <common/handle.h>

#define PINB_ADDR 0x03
#define DDRB_ADDR 0x04

using Sensors = PortScanner<ReadonlyRegister<PINB_ADDR>>;

Sensors::State state_left = Sensors::State();
Sensors::State state_center = Sensors::State();
Sensors::State state_right = Sensors::State();


StationTracker tracker = StationTracker(
    state_left.value,
    state_center.value,
    state_right.value
);

constexpr Station STATION_1 {
    .id = 1
};

u8 test = 8;

#include <Arduino.h>


void setup() {
    // No abstraction for this yet, just keep as is for simple example
    *(volatile u8*)DDRB_ADDR = 0x00;

    tracker.check_occupancy(STATION_1);

    using t = DataHandle<test, 1>;
}

void loop() {
    // Sensors::update(sensor_state_a);
    // if (SensorView::is_set(sensor_state_a.value)) {
    //     // Pin is flipped
    // }
}