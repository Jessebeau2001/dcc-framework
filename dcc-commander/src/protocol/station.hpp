#pragma once

#include <types.h>
#include <common/reg/register_helper.h>

using namespace types;

struct Station {
    u8 id;
};

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

class StationManager {
public:
    // TODO: replace with platform independant registry struct that acts as wrapper
    template <typename Reg>
    static inline bool is_occupied(const Station &station) {
        u8 idx = station.id;
        u8 value = Reg::read();
        return (value & (1 << idx)) != 0;
    }
};
