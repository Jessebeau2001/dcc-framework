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
        volatile uint8_t bit = 0;
        volatile uint8_t byte = 0;

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

    inline size_t available_for_write() const {
        // Calculate free whole-byte slots in the ring.
        // Reserve one slot to distinguish a full buffer from an empty one.
        // MASK wraps the distance around the power-of-two buffer.
        return (static_cast<size_t>(tail.byte) + SIZE - head.byte - 1) & MASK;
    }

    inline bool available_for_read() const {
        if (tail.byte != head.byte) return true;
        if (tail.bit != head.bit) return true;
        return false;
    }

    void push_byte(uint8_t byte) {
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
            buffer[head.byte] |= (1 << (7 - head.bit));
        }

        head.step_bit();
    }

    inline uint8_t pop_byte() {
        uint8_t value;
        if (tail.bit == 0) {
            value = buffer[tail.byte];
            tail.step_byte();
        } else {
            uint8_t upper = buffer[tail.byte] << tail.bit; 
            tail.step_byte();
            uint8_t lower = buffer[tail.byte] >> (8 - tail.bit);

            value = upper | lower;
        }
        return value;
    }

    inline bool pop_bit() {
        bool bit = (buffer[tail.byte] >> (7 - tail.bit)) & 1;
        tail.step_bit();
        return bit;
    }
};