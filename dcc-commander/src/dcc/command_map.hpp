#pragma once

#include <types.h>
#include <dcc/types.h>
#include <common/container.h>

using namespace types;

template <u8 Size>
class CommandMap {
    enum class Flag : u8 {
        IN_USE    = 0b10000000
    };

    using Entry = Container<dcc::Command, Flag>;
    
    Entry storage[Size];
    u8 count_ = 0;

    void place_command(Entry& entry, dcc::Command& command);
    i16 find_empty_index() const;
    i16 find_index_of(u16 train_addr) const;

public:
    class Cursor {
        const Entry *data;
        u8 cursor = 0;

    public:
        Cursor(const Entry *data) : data(data) { }

        const dcc::Command *next() {
            for (u8 i = 0; i < Size; i++) {
                Entry const* container = &data[cursor];

                cursor += 1;
                if (cursor >= Size)
                    cursor = 0;

                if (container->is_set(Flag::IN_USE))
                    return &container->data;
            }
            return nullptr;
        }
    };

    const dcc::Command *put(dcc::Command &command);
    void remove(u16 address);

    Cursor create_cursor() const {
        return Cursor(storage);
    }

    static constexpr u8 SIZE = Size;

    inline u8 count() const {
        return count_;
    }

    inline bool is_empty() const {
        return count_ == 0;
    }

    inline bool is_full() const {
        return count_ >= Size;
    }
};