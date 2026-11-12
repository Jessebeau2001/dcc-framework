#pragma once

#include <common/reg/register_helper.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <common/handle.h>

#define PINB_ADDR 0x03
#define DDRB_ADDR 0x04

#define PINC_ADDR 0x06
#define DDRC_ADDR 0x07

#define PIND_ADDR 0x09
#define DDRD_ADDR 0x0A

using _PINB = ReadonlyRegister<PINB_ADDR>;
using _PINC = ReadonlyRegister<PINC_ADDR>;
using _PIND = ReadonlyRegister<PIND_ADDR>;

auto station_tracker = StationTracker<_PINB, _PINC, _PIND>();

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

template <uintptr_t Address, typename T = u8>
void set_reg(u8 config) {
    *reinterpret_cast<volatile T*>(Address) = config;
}

void setup() {
    set_reg<DDRB_ADDR>(0);
    set_reg<DDRC_ADDR>(0);
    set_reg<DDRD_ADDR>(0);
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