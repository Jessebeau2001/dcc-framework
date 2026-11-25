#pragma once

#include <common/reg/port_scanner.h>
#include <hal/core/dynamic.h>
#include <dcc/train.hpp>
#include <common/util.h>
#include <platform/board.h>
#include <common/handle.h>

#include <Arduino.h>

using IO_A = platform::avr::GPIO_B;
using IO_B = platform::avr::GPIO_C;
using IO_C = platform::avr::GPIO_D;

auto station_debouncer = train::StationPortDebouncer<
    IO_A::PIN,
    IO_B::PIN,
    IO_C::PIN>();

using DebounceLeft   = PortDebouncer<IO_A::PIN>;
using DebounceCenter = PortDebouncer<IO_B::PIN>;
using DebounceRight  = PortDebouncer<IO_C::PIN>;

using DebounceState8 = DebounceState<u8>;

auto state_left      = DebounceState8{};
auto state_center    = DebounceState8{};
auto state_right     = DebounceState8{};

using BindingLeft    = DebounceState8::Binding<state_left>;
using BindingCenter  = DebounceState8::Binding<state_center>;
using BindingRight   = DebounceState8::Binding<state_right>;

// Simple helper that auto binds shared ID for sensor and pin index to regs
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

const auto track_1 = train::TrackScanner({ station_0, station_1, station_2, station_3 });
train::TrainState train_1 = train::TrainState{};

void setup() {
    Serial.begin(115200);
    HAL::io_configure<IO_A>(HAL::IOMode::Input);
    HAL::io_configure<IO_B>(HAL::IOMode::Input);
    HAL::io_configure<IO_C>(HAL::IOMode::Input);
    HAL::io_configure<platform::avr::LED_ONBOARD>(HAL::IOMode::Output);
}

auto clock = util::Clock<1000>{};

void loop() {
    Track1::resolve_train_position(train_1);

    if (clock.update(millis())) {
        Serial.write("Hello Arie");
    }
}