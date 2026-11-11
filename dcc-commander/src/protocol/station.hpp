#pragma once

#include <types.h>
#include <common/reg/register_helper.h>
#include <common/handle.h>

using namespace types;

struct Station;

struct StationNode {
    Station *station;

    StationNode *next;
    StationNode *prev;
};

struct GraphStationMap {
    StationNode *head;
    StationNode *tail;
};

template <u8 Size>
struct LinearStationMap {
    Station stations[Size];
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

struct StationConfig {
    u8 pin_left;
    u8 pin_center;
    u8 pin_right;
};

struct Station {
    StationConfig config;
    u8 id;
};



enum StationOcupancy {
    EMPTY,
    ARPOACH_LEFT,
    OCCUPIED,
    APROACH_RIGHT
};

class StationTracker {
private:
    const u8& left_data;
    const u8& center_data;
    const u8& right_data;

public:
    StationTracker(const u8& left_data, const u8& center_data, const u8& right_data)
      : left_data(left_data),
        center_data(center_data),
        right_data(right_data) { }

    StationOcupancy check_occupancy(const Station& station) const {
        u8 mask = 1 << station.id;
        if ((left_data & mask) != 0) {
            return StationOcupancy::ARPOACH_LEFT;
        } else if ((center_data & mask) != 0) {
            return StationOcupancy::OCCUPIED;
        } else if ((right_data & mask) != 0) {
            return StationOcupancy::APROACH_RIGHT;
        } else {
            return StationOcupancy::EMPTY;
        }
    }
};
