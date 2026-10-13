#pragma once

#include <types.h>
#include "protocol/dcc_def.h"

using namespace types;

class CommandMap {
private:
    struct Container {
        enum class Flag : u8 {
            IN_USE    = 0b10000000
        };

        // No Padding! AVR is 8 bit
        dcc::Command command;    // 2 bytes
        u8 header;                      // 1 byte

        inline bool is_set(Flag flag) const {
            return (header & static_cast<uint8_t>(flag));
        }

        inline void set(Flag flag) {
            header |= static_cast<uint8_t>(flag);
        }

        inline void clear(Flag flag) {
            header &= ~static_cast<uint8_t>(flag);
        }

        void place_command(dcc::Command &command);
    };

    static constexpr u8 Size = 16U;

    Container heap[Size];
    u8 count_ = 0;

    u8 find_empty_index() const;
    u8 find_index_of(uint8_t address) const;

public:
    bool put(dcc::Command &command);
    void remove(uint8_t address);

    static inline constexpr u8 size() {
        return Size;
    }

    inline u8 count() const {
        return count_;
    }

    inline bool is_full() const {
        return count_ >= Size;
    }
};