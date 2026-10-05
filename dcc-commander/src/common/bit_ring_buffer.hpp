#pragma once

#include <stddef.h>
#include <stdint.h>


template <uint8_t Size = 32>
struct BitRingBuffer {
    static_assert(Size >= 2 && (Size & (Size - 1)) == 0,
                "Buffer size must be a power of 2 and at least 2");

    static constexpr uint8_t SIZE = Size;
    static constexpr uint8_t MASK = Size - 1;

    struct Marker {
        uint8_t bit = 0;
        uint8_t byte = 0;

        inline uint8_t next_byte() {
            return (byte + 1) & MASK;
        }

        inline uint8_t free_bits() {
            return 8 - bit;
        }

        inline void step_byte() {
            byte = next_byte();
        }

        inline void step_bit() {
            bit++;
            if (bit == 8) {
                bit = 0;
                step_byte();
            }
        }
    };

    volatile uint8_t buffer[SIZE];
    Marker head{}; // Mutated by producer
    Marker tail{}; // Mutated by consumer

    void push(uint8_t byte) {
        if (head.bit == 0) {
            buffer[head.byte] = byte;
            head.step_byte();
        } else {
            // Split write across two bytes
            uint8_t free_bits = head.free_bits();
            uint8_t next_byte = head.next_byte();

            // buffer[head.byte] |= (byte << head.bit);     // Write lower
            // buffer[next_byte]  = (byte >> free_bits);    // Write upper

            buffer[head.byte] |= (byte >> (8 - free_bits)); // Write upper
            buffer[next_byte]  = (byte << free_bits);       // Write lower

            head.step_byte();
        }
    }

    void push_bit(bool bit) { 
        // When currently aligned clear out entire value
        if (head.bit == 0) {
            buffer[head.byte] = 0;
        }

        if (bit) {
            buffer[head.byte] |= (1 << head.bit);
        }

        head.step_bit();
    }

    inline bool available_for_read() const {
        return tail.byte != head.byte;
    }

    inline uint8_t pop_byte() {
        return 0;
    }

    inline bool pop_bit() {
        // Read the current bit from MSB to LSB
        uint8_t bit_index = 7 - tail.bit;
        bool bit = (buffer[tail.byte] >> bit_index) & 1;

        tail.step_bit();

        return bit;
    }
};