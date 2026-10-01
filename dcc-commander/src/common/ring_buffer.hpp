#pragma once

#include <stddef.h>
#include <stdint.h>

template <uint8_t Size = 32>
struct RingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Buffer size must be a power of 2");

    static constexpr uint8_t SIZE = Size;
    static constexpr uint8_t MASK = Size - 1;

    // Buffer elements and pointers accessed across contexts need volatile
    volatile uint8_t buf[SIZE];
    volatile uint8_t head = 0; // Written by producer (main code)
    volatile uint8_t tail = 0; // Written by consumer (ISR)
    
    inline size_t available_for_write() {
        const uint8_t h = head;
        const uint8_t t = tail;
        
        if (h >= t) {
            return (SIZE - 1) - (h - t);
        } else {
            return (t - h) - 1;
        }
    }

    // TODO: Maybe change to int, return num bytes written
    void write(const uint8_t* data, size_t length) {
        uint8_t current = head;
        for (size_t i = 0; i < length; ++i) {
            buf[current] = data[i];
            current = (current + 1) & MASK;
        }
        head = current;
    }
};