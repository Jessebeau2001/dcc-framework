#pragma once

#include <HAL/register.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <HAL/my_hal.h>
#include <HAL/dynamic.h>

#include <Arduino.h>

auto station_debouncer = train::StationPortDebouncer<
    HAL::GPIO_B::PIN,
    HAL::GPIO_C::PIN,
    HAL::GPIO_D::PIN>();
auto station_pool = train::StationPool();

const auto station_0 = station_pool.create_station(station_debouncer, 0);
const auto station_1 = station_pool.create_station(station_debouncer, 1);
const auto station_2 = station_pool.create_station(station_debouncer, 2);
const auto station_3 = station_pool.create_station(station_debouncer, 3);
const auto station_4 = station_pool.create_station(station_debouncer, 4);
const auto station_5 = station_pool.create_station(station_debouncer, 5);
const auto station_6 = station_pool.create_station(station_debouncer, 6);
const auto station_7 = station_pool.create_station(station_debouncer, 7);

void setup() {
    HAL::io_configure<HAL::GPIO_C>(HAL::IOMode::InputPullup);
    HAL::io_configure<HAL::LED_ONBOARD>(HAL::IOMode::Output);
}

void loop() {
    station_debouncer.update();
    if (train::check_occupancy(*station_1) == train::StationOcupancy::OCCUPIED) {
        HAL::io_write<HAL::LED_ONBOARD>(1);
    } else {
        HAL::io_write<HAL::LED_ONBOARD>(0);
    }
}