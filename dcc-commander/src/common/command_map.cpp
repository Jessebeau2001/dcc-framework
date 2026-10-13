#include "command_map.hpp"

#include <string.h>

void CommandMap::Container::place_command(dcc::Command &command) {
    // new (&this->command) proto::dcc::Command(&command);
    // Placment new doesnt work due to some funky arduino stuff, fuck it
    // well use memcpy instead

    memcpy(&this->command, &command, sizeof(dcc::Command));
    this->set(Container::Flag::IN_USE);
}


u8 CommandMap::find_empty_index() const {
    for (u8 i = 0; i < Size; i++) {
        if (heap[i].is_set(Container::Flag::IN_USE) == 0)
            return i;
    }

    return -1;
}

u8 CommandMap::find_index_of(u8 address) const {
    for (u8 i = 0; i < Size; i++) {
        if (heap[i].is_set(Container::Flag::IN_USE) &&
            heap[i].command.address == address
        ) {
            return i; 
        }
    }

    return -1;
}

const dcc::Command *CommandMap::put(dcc::Command &command) {
    int index = find_index_of(command.address);

    if (index == -1) {              // Address not yet on heap
        if (is_full())              // Cannot allocate new slot, heap full
            return nullptr;
        index = find_empty_index(); // Find new free slot
        count_++;                   // Increment the count for new slot
    }

    Container *container = &heap[index];
    container->place_command(command);

    return &container->command;
}

void CommandMap::remove(uint8_t address) {
    for (int i = 0; i < Size; i++) {
        Container &slot = heap[i];

        if (slot.is_set(Container::Flag::IN_USE) &&
            slot.command.address == address
        ) {
            slot.clear(Container::Flag::IN_USE);
            count_--;
        }
    }
}