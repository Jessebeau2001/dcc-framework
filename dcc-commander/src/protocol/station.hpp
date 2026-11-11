#pragma once

#include <types.h>
#include <common/reg/register_helper.h>
#include <common/handle.h>

using namespace types;

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

template <u8 Index>
struct StationView {
    using Left   = BitView<Index>;
    using Center = BitView<Index>;
    using Right  = BitView<Index>;
};

template <u8 Id>
struct StationConfig {
    static constexpr u8 id = Id;
    using View = StationView<Id>;
};

template <u8 Id>
struct Station {
    using Config = StationConfig<Id>;
    // Can add geuine runtime data here
};

enum StationOcupancy {
    EMPTY,
    ARPOACH_LEFT,
    OCCUPIED,
    APROACH_RIGHT
};

// constexpr StationConfig STATION_1 {
//     .id = 1,
//     .pin_index = 0
// };

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


    template <u8 Id>
    StationOcupancy check_occupancy(const Station<Id> &station) const {
        using View = typename Station<Id>::Config::View;
        if (View::Left::is_set(left_data)) {
            return StationOcupancy::ARPOACH_LEFT;
        } else if (View::Center::is_set(center_data)) {
            return StationOcupancy::OCCUPIED;
        } else if (View::Right::is_set(right_data)) {
            return StationOcupancy::APROACH_RIGHT;
        } else {
            return EMPTY;
        }
    }

    StationOcupancy check_occupancy_(u8 index) const {
        u8 mask = 1 << index;
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
