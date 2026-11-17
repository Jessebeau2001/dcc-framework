#pragma once

#include <HAL/register.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <HAL/my_hal.h>
#include <HAL/dynamic.h>

#include <Arduino.h>

using StationSensors = StationPortDebouncer<
    HAL::GPIO_B::PIN,
    HAL::GPIO_C::PIN,
    HAL::GPIO_D::PIN
>;

auto station_debouncer = StationSensors();
auto station_1 = Station{};

void setup() {
    setup_station(station_1, 1u, station_debouncer, 0u);
    // link_config(station_1.config, station_debouncer, 1, 1, 1);

    // using MyPin = HAL::GPIO_C::Pin<0>;
    // using MyPin2 = HAL::GPIO_B::Pin<4>;

    // HAL::DynamicPin<u8> dpin = HAL::make_dynamic<MyPin>();
    // dpin = HAL::make_dynamic<MyPin2>();

    // HAL::io_configure<MyPin>(HAL::IOMode::Input);
    // HAL::io_configure<HAL::GPIO_B>(HAL::IOMode::Input);
}

void loop() {
    station_debouncer.update();
}