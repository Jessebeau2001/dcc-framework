#pragma once

#include <HAL/register.h>
#include <common/reg/port_scanner.h>
#include <dcc/station.hpp>
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

const auto station_map = train::LinearStationMap({
    station_0->id,
    station_1->id,
    station_2->id,
    station_3->id,
    station_4->id,
    station_5->id,
    station_6->id,
    station_7->id
});

void setup() {
    HAL::io_configure<HAL::GPIO_B>(HAL::IOMode::Input);
    HAL::io_configure<HAL::GPIO_C>(HAL::IOMode::Input);
    HAL::io_configure<HAL::GPIO_D>(HAL::IOMode::Input);
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