#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <common/handle.h>

#define PINB_ADDR 0x03
#define DDRB_ADDR 0x04

using SensorsLeft   = PortScanner<ReadonlyRegister<PINB_ADDR>>;
using SensorsCenter = PortScanner<ReadonlyRegister<PINB_ADDR>>;
using SensorsRight  = PortScanner<ReadonlyRegister<PINB_ADDR>>;

auto state_left     = SensorsLeft::State();
auto state_center   = SensorsCenter::State();
auto state_right    = SensorsRight::State();


StationTracker tracker = StationTracker(
    state_left.value,
    state_center.value,
    state_right.value
);

using Station1Config = StationConfig<1, 1, 1>;

constexpr Station<Station1Config> STATION_1 {
    .id = 1
};

#include <Arduino.h>


void setup() {
    // No abstraction for this yet, just keep as is for simple example
    *(volatile u8*)DDRB_ADDR = 0x00;

}

void loop() {
    SensorsLeft::update(state_left);
    SensorsCenter::update(state_center);
    SensorsRight::update(state_right);

    auto occupancy = tracker.check_occupancy<Station1Config>();
    switch (occupancy) {
    case StationOcupancy::LEFT:
        Serial.println("Aproaching Left");
        break;
    case StationOcupancy::OCCUPIED:
        Serial.println("At Center");
        break;
    case StationOcupancy::RIGHT:
        Serial.println("Aproaching Right");
        break;
    case StationOcupancy::EMPTY:
        Serial.println("No data");
        break;
    }
}