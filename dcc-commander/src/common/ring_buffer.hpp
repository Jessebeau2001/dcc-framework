#pragma once

#include <stddef.h>
#include <stdint.h>

template <uint8_t Size = 32>
struct RingBuffer {
    static_assert(Size >= 2 && (Size & (Size - 1)) == 0,
              "Buffer size must be a power of 2 and at least 2");

    static constexpr uint8_t SIZE = Size;
    static constexpr uint8_t MASK = Size - 1;

    // Buffer elements and pointers accessed across contexts need volatile
    volatile uint8_t buf[SIZE];
    volatile uint8_t head = 0; // Written by producer (main code)
    volatile uint8_t tail = 0; // Written by consumer (ISR)
    
    inline size_t available_for_write() {
        return (static_cast<size_t>(tail) -
                static_cast<size_t>(head) - 1) & MASK;
    }

    inline void push(uint8_t value) {
        buf[head] = value;
        head = (head + 1) & MASK;
    }

    inline uint8_t pop() {
        auto value = buf[tail];
        tail = (tail + 1) & MASK;
        return value;
    }

    size_t write(const uint8_t* data, size_t length) {
        uint8_t current = head;
        for (size_t i = 0; i < length; ++i) {
            buf[current] = data[i];
            // Beware of integer promotion here
            current = (current + 1) & MASK; 
        }
        head = current;
        return length; // Seems stupid - just opening up the contract to posible partial writes later
    }
};