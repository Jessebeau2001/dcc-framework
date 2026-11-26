#pragma once

#include <common/reg/port_scanner.h>
#include <hal/core/dynamic.h>
#include <dcc/train.hpp>
#include <common/util.h>
#include <platform/board.h>
#include <common/handle.h>

#include <Arduino.h>

using IO_Left = platform::avr::GPIO_B;
using IO_Center = platform::avr::GPIO_C;
using IO_Right = platform::avr::GPIO_D;

auto station_debouncer = train::StationPortDebouncer<
    IO_Left::PIN,
    IO_Center::PIN,
    IO_Right::PIN>();

using DebouncerLeft   = PortDebouncer<IO_Left::PIN>;
using DebouncerCenter = PortDebouncer<IO_Center::PIN>;
using DebouncerRight  = PortDebouncer<IO_Right::PIN>;

using DebounceState8 = DebounceState<u8>;

auto debounce_state_left      = DebounceState8{};
auto debounce_state_center    = DebounceState8{};
auto debounce_state_right     = DebounceState8{};

using BindingLeft    = DebounceState8::Binding<debounce_state_left>;
using BindingCenter  = DebounceState8::Binding<debounce_state_center>;
using BindingRight   = DebounceState8::Binding<debounce_state_right>;

// Simple helper that auto binds shared ID for sensor and pin index to virtual regs
template <u8 Id>
using SimpleStation = train::StaticStation<Id, train::TripletView<Id, BindingLeft, BindingCenter, BindingRight>>;

using Station0 = SimpleStation<0>;
using Station1 = SimpleStation<1>;
using Station2 = SimpleStation<2>;
using Station3 = SimpleStation<3>;
using Station4 = SimpleStation<4>;
using Station5 = SimpleStation<5>;
using Station6 = SimpleStation<6>;
using Station7 = SimpleStation<7>;

using Track1 = train::Track<Station0, Station1, Station2, Station3>;
using Track2 = train::Track<Station4, Station5, Station6, Station7>;

train::TrainState train_1 = train::TrainState{};
train::TrainState train_2 = train::TrainState{};

void setup() {
    Serial.begin(115200);

    HAL::io_configure<IO_Left  >(HAL::IOMode::Input);
    HAL::io_configure<IO_Center>(HAL::IOMode::Input);
    HAL::io_configure<IO_Right >(HAL::IOMode::Input);
}

auto clock = util::Clock<1000>{};

void loop() {
    // First bulk debounce all 3x8 input pins
    DebouncerLeft  ::update(debounce_state_left);
    DebouncerCenter::update(debounce_state_center);
    DebouncerRight ::update(debounce_state_right);

    // Extrapolate train state from debounced sensor data and write to train
    Track1::resolve_train_position(train_1);
    Track2::resolve_train_position(train_2);

    if (clock.update(millis())) {
        // Todo show some debug output
    }
}