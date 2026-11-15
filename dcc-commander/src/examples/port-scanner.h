#pragma once

#include <HAL/register.h>
#include <common/reg/port_scanner.h>
#include <protocol/station.hpp>
#include <HAL/my_hal.h>

#include <Arduino.h>

using Sensors = PortScanner<HAL::GPIO_B::PIN>;
auto debounce_state = Sensors::State();

void setup() {
    using MyPin = HAL::GPIO_C::Pin<0>;

    HAL::io_configure<MyPin>(HAL::IOMode::Input);
    // HAL::io_configure<HAL::GPIO_B>(HAL::IOMode::Input);
}

void loop() {
    Sensors::update(debounce_state);
}