#pragma once

#include <types.h>
#include <HAL/dynamic.h>
#include <common/handle.h>

using namespace types;

namespace train {

// useless shit
template <typename T>
struct LinkedList {
    struct Node {
        T data;

        Node *next;
        Node *prev;

        void set_next(Node *append) {
            if (append != nullptr)
                append->prev = this;
            this->next = append;
        }

        void set_prev(Node *append) {
            if (append != nullptr)
                append->next = this;
            this->prev = append;
        }

        bool look_ahead(T search) const {
            const Node* current = this;
            while (current != nullptr) {
                if (current->data == search)
                    return true;
                current = current->next;
            }
            return false;
        }
    };

    Node *head;
    Node *tail;
};

enum class Direction : u8 {
    FORWARD   =  0,
    BACKWARD  =  1,
    CURRENT   =  2,
    NOT_FOUND =  3
};

template <typename T, typename Flag>
struct Container {
    T data;
    u8 header;

    inline bool is_set(Flag flag) const {
        return (header & static_cast<u8>(flag));
    }

    inline void set(Flag flag) {
        header |= static_cast<u8>(flag);
    }

    inline void clear(Flag flag) {
        header &= ~static_cast<u8>(flag);
    }
};

template <typename T, u8 Size = 8>
class StationMap {
public:
    enum class Flag : u8 {
        IN_USE    = 0b10000000
    };

private:
    static constexpr u8 SIZE = Size;
    Container<T, Flag> data[SIZE];

public:
    inline void put(u8 index, T element) {
        data[index] = Container{
            .data = element,
            .header = Flag::IN_USE
        };
    }

    i16 index_of(T element) const {
        for (u8 i = 0; i < SIZE; i++) {
            Container<T, Flag>& current = data[i];
            if (current.is_set(Flag::IN_USE) && current.data == element)
                return i;
        }
        return -1;
    }

    Direction search(T from, T to) const {
        auto from_idx = index_of(from);
        if (from_idx < 0)
            return Direction::NOT_FOUND;

        auto to_idx = index_of(to);
        if (to_idx < 0)
            return Direction::NOT_FOUND;

        if (from_idx == to_idx) {
            return Direction::CURRENT;
        } else if (from_idx < to_idx) {
            return Direction::FORWARD;
        } else {
            return Direction::BACKWARD;
        }
    }
};



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

enum StationOcupancy {
    EMPTY,
    LEFT,
    OCCUPIED,
    RIGHT
};

void set_config_handle(SensorHandle& handle, u8 const* value_ptr, u8 i) {
    handle.addr = value_ptr;
    handle.mask = static_cast<u8>(1u << i);
}

template <typename SensorStates>
void link_config(StationConfig& config, const SensorStates& sensors, u8 left, u8 center, u8 right) {
    set_config_handle(config.sensor_left,   &sensors.state_left.value,   left);
    set_config_handle(config.sensor_center, &sensors.state_center.value, center);
    set_config_handle(config.sensor_right,  &sensors.state_right.value,  right);
}

class StationPool {
    static constexpr u8 MAX_STATIONS = 8u; // Essentially the register size

    Station stations[MAX_STATIONS];
    u8 station_count = 0;

public:
    template <typename SensorStates>
    Station const* create_station(const SensorStates& sensors, u8 sensor_shift) {
        if (station_count >= MAX_STATIONS)
            return nullptr;
        
        u8 index = station_count;   // Workable index is current count
        u8 id = index;              // id's 0-7, exactly the same as storage index
        u8 shift = sensor_shift;    // Sensor shift can be picked by the caller
        
        Station& station = stations[index];

        station.id = id;
        link_config(station.config, sensors, shift, shift, shift);

        station_count += 1;
        return &stations[index];
    };
};


StationOcupancy check_occupancy(const Station& station) {
    if (HAL::read_bit(station.config.sensor_left) != 0) {
        return StationOcupancy::LEFT;
    } else if (HAL::read_bit(station.config.sensor_center) != 0) {
        return StationOcupancy::OCCUPIED;
    } else if (HAL::read_bit(station.config.sensor_right) != 0) {
        return StationOcupancy::RIGHT;
    } else {
        return StationOcupancy::EMPTY;
    }
}

} // namespace train