#pragma once

#include <stdint.h>
#include "protocol/dcc.hpp"

class CommandMap {
private:
    struct Container {
        enum class Flag : uint8_t {
            IN_USE    = 0b10000000
        };

        // No Padding! AVR is 8 bit
        proto::dcc::Command command; // 2 bytes
        uint8_t header;       // 1 byte

        inline bool is_set(Flag flag) const {
            return (header & static_cast<uint8_t>(flag));
        }

        inline void set(Flag flag) {
            header |= static_cast<uint8_t>(flag);
        }

        inline void clear(Flag flag) {
            header &= ~static_cast<uint8_t>(flag);
        }

        void place_command(proto::dcc::Command &command);
    };

    static constexpr uint8_t Size = 16U;

    Container heap[Size];
    uint8_t count_ = 0;

    int find_empty_index() const;
    int find_index_of(uint8_t address) const;
public:
    bool put(proto::dcc::Command &command);
    void remove(uint8_t address);

    static inline constexpr uint8_t size() {
        return Size;
    }

    inline uint8_t count() const {
        return count_;
    }

    inline bool is_full() const {
        return count_ >= Size;
    }
};