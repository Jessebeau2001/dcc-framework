#pragma once

#include <types.h>
#include <dcc/types.h>
#include <common/container.h>
#include <string.h>

using namespace types;

template <u8 Size>
class CommandMap {
    enum class Flag : u8 {
        IN_USE    = 0b10000000
    };

    using Entry = Container<dcc::Command, Flag>;
    
    Entry storage[Size];
    u8 count_ = 0;

    void place_command(Entry& entry, dcc::Command& command) {
        // new (&this->command) proto::dcc::Command(&command);
        // Placement new doesnt work due to some funky arduino api stuff, fuck it
        // we'll use memcpy instead

        memcpy(&entry.data, &command, sizeof(dcc::Command));
        entry.set(Flag::IN_USE);
    }

    i16 find_empty_index() const {
        for (u8 i = 0; i < Size; i++) {
            if (storage[i].is_set(Flag::IN_USE) == 0)
                return i;
        }

        return -1;
    }

    i16 find_index_of(u16 train_addr) const {
        for (u8 i = 0; i < Size; i++) {
            if (storage[i].is_set(Flag::IN_USE) &&
                storage[i].data.address == train_addr
            ) {
                return i;
            }
        }

        return -1;
    }

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

    dcc::Command const* put(dcc::Command& command) {
        i16 index = find_index_of(command.address);

        if (index == -1) {              // Address not yet on heap
            if (is_full())              // Cannot allocate new slot, heap full
                return nullptr;
            index = find_empty_index(); // Find new free slot
            count_++;                   // Increment the count for new slot
        }

        Entry& entry = storage[index];
        place_command(entry, command);

        return &entry.data;
    }

    void remove(u16 address) {
        for (u8 i = 0; i < Size; i++) {
            Entry &slot = storage[i];

            if (slot.is_set(Flag::IN_USE) &&
                slot.data.address == address
            ) {
                slot.clear(Flag::IN_USE);
                count_--;
            }
        }
    }

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