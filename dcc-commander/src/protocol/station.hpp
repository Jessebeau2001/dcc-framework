#pragma once

#include <types.h>
#include <common/reg/register_helper.h>
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

template <u8 PinL, u8 PinC, u8 PinR>
struct StationView {
    using Left                  = BitView<PinL>;
    using Center                = BitView<PinC>;
    using Right                 = BitView<PinR>;

    static constexpr u8 PIN_CENTER = PinC;
    static constexpr u8 PIN_LEFT   = PinL;
    static constexpr u8 PIN_RIGHT  = PinR;
};


template <u8 PinL, u8 PinC, u8 PinR>
struct StationConfig {
    using View = StationView<PinL, PinC, PinR>;
};

template <typename ConfigType>
struct Station {
    using Config = ConfigType;

    u8 id;
};


enum StationOcupancy {
    EMPTY,
    LEFT,
    OCCUPIED,
    RIGHT
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

    template <typename ConfigType>
    StationOcupancy check_occupancy() const {
        using View = typename ConfigType::View;

        if (View::Left::is_set(left_data)) {
            return StationOcupancy::LEFT;
        } else if (View::Center::is_set(center_data)) {
            return StationOcupancy::OCCUPIED;
        } else if (View::Right::is_set(right_data)) {
            return StationOcupancy::RIGHT;
        } else {
            return StationOcupancy::EMPTY;
        }

        return EMPTY;
    }
};
