#pragma once

#include <types.h>
#include <HAL/dynamic.h>
#include <common/handle.h>

using namespace types;

// struct Station;

struct StationNode {
    // Station *station;

    StationNode *next;
    StationNode *prev;
};

struct GraphStationMap {
    StationNode *head;
    StationNode *tail;
};

template <u8 Size>
struct LinearStationMap {
    // Station stations[Size];
};

void set_next(StationNode &current, StationNode *append) {
    if (append != nullptr) {
        append->prev = &current;
    }
    current.next = append;
}

void set_prev(StationNode &current, StationNode *append) {
    if (append != nullptr) {
        append->next = &current;
    }
    current.prev = append;
}

// This system is kinda purpose build for 8 bit wide registers so might aswell hard lock the type here
using SensorHandle = HAL::DataView<u8>;

struct StationConfig {
    SensorHandle sensor_left;
    SensorHandle sensor_center;
    SensorHandle sensor_right;
};

struct Station {
    StationConfig config;
    u8 id;
};

enum StationOcupancy {
    EMPTY,
    LEFT,
    OCCUPIED,
    RIGHT
};

void set_config_handle(SensorHandle& handle, u8* value_ptr, u8 i) {
    handle.addr = value_ptr;
    handle.mask = static_cast<u8>(1u << i);
}

template <typename Sensors>
void link_config(StationConfig& config, Sensors& sensors, u8 left, u8 center, u8 right) {
    set_config_handle(config.sensor_left,   &sensors.state_left.value,   left);
    set_config_handle(config.sensor_center, &sensors.state_center.value, center);
    set_config_handle(config.sensor_right,  &sensors.state_right.value,  right);
}

template <typename Sensors>
void setup_station(Station& station, u8 id, Sensors& sensors, u8 sensor_shift) {
    station.id = id;
    link_config(station.config, sensors, sensor_shift, sensor_shift, sensor_shift);
}

template <typename RegLeft, typename RegCenter, typename RegRight>
struct StationPortDebouncer {
    using PortLeft              = PortScanner<RegLeft>;
    using PortCenter            = PortScanner<RegCenter>;
    using PortRight             = PortScanner<RegRight>;

    using LeftState             = typename PortLeft::State;
    using CenterState           = typename PortCenter::State;
    using RightState            = typename PortRight::State;

    LeftState   state_left      = LeftState();
    CenterState state_center    = CenterState();
    RightState  state_right     = RightState();

    inline void update() {
        PortLeft::update(state_left);
        PortCenter::update(state_center);
        PortRight::update(state_right);
    }
};

class StationTracker {
private:
public:
    StationTracker() { };

    // StationOcupancy check_occupancy(const Station& station) const {
    //     if ((state_left.value & (1 << station.pins.pin_left)) != 0) {
    //         return StationOcupancy::LEFT;
    //     } else if ((state_center.value & (1 << station.pins.pin_center)) != 0) {
    //         return StationOcupancy::OCCUPIED;
    //     } else if ((state_right.value & (1 << station.pins.pin_right)) != 0) {
    //         return StationOcupancy::RIGHT;
    //     } else {
    //         return StationOcupancy::EMPTY;
    //     }
    // }
};
