#pragma once

#include <types.h>
#include <HAL/register.h>
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

struct Station {
    const u8 id;
};

enum StationOcupancy {
    EMPTY,
    LEFT,
    OCCUPIED,
    RIGHT
};


template <typename RegLeft, typename RegCenter, typename RegRight>
class StationTracker {
private:
    using PortLeft      = PortScanner<RegLeft>;
    using PortCenter    = PortScanner<RegCenter>;
    using PortRight     = PortScanner<RegRight>;

    using LeftState     = typename PortLeft::State;
    using CenterState   = typename PortCenter::State;
    using RightState    = typename PortRight::State;

    LeftState state_left        = LeftState();
    CenterState state_center    = CenterState();
    RightState state_right      = RightState();

public:
    StationTracker() { };


    inline void update() {
        PortLeft::update(state_left);
        PortCenter::update(state_center);
        PortRight::update(state_right);
    }


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
