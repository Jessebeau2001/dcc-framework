#pragma once

#include <HAL/register.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <common/handle.h>
#include <HAL/my_hal.h>


auto station_tracker = StationTracker<HAL::_PINB, HAL::_PINC, HAL::_PIND>();

static constexpr StationPinConfig STATION1_PINOUT = StationPinConfig{
    .pin_left = 1,
    .pin_center = 1,
    .pin_right = 1
};

Station my_station = Station{
    .pins = STATION1_PINOUT,
    .id = 1
};

#include <Arduino.h>

void setup() {
    HAL::_DDRB::write(0);
    HAL::_DDRC::write(0);
    HAL::_DDRD::write(0);
}

void loop() {
    auto occupancy = station_tracker.check_occupancy(my_station);
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