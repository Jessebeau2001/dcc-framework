#include "command_map.hpp"
#include <string.h>

template <u8 Size>
void CommandMap<Size>::place_command(Entry& entry, dcc::Command& command) {
    // new (&this->command) proto::dcc::Command(&command);
    // Placement new doesnt work due to some funky arduino api stuff, fuck it
    // we'll use memcpy instead

    memcpy(&entry.data, &command, sizeof(dcc::Command));
    entry.set(Flag::IN_USE);
}


template <u8 Size>
i16 CommandMap<Size>::find_empty_index() const {
    for (u8 i = 0; i < Size; i++) {
        if (storage[i].is_set(Flag::IN_USE) == 0)
            return i;
    }

    return -1;
}

template <u8 Size>
i16 CommandMap<Size>::find_index_of(u16 train_addr) const {
    for (u8 i = 0; i < Size; i++) {
        if (storage[i].is_set(Flag::IN_USE) &&
            storage[i].data.address == train_addr
        ) {
            return i;
        }
    }

    return -1;
}

template <u8 Size>
const dcc::Command *CommandMap<Size>::put(dcc::Command &command) {
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

template <u8 Size>
void CommandMap<Size>::remove(u16 address) {
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