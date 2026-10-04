#pragma once

#include <stdint.h>

namespace proto::dcc {
    union Instruction {
        struct {
            uint8_t speed    : 4;   // SSSS (0000 = Stop, 0001 = E-Stop, 0010-1111 = Steps 1-14)
            uint8_t light    : 1;   // C    (0 = Off, 1 = On)
            uint8_t direction: 1;   // D    (Direction 0 = Reverse, 1 = Forward)
            uint8_t idata    : 2;   // 01   (Instruction data)
        } fields;

        uint8_t value;
    };

    struct Command {
        uint8_t     address;        // Address is 8 bits
        Instruction instruction;
    };

    template<typename Writer>
    class DccWriter {
    private:
        uint8_t checksum = 0;

    public:
        DccWriter() { }

        void frame_open() {
            checksum = 0;

            Writer::write_byte(0xFF); // Write the preamble
            Writer::write_byte(0xFF); // We choose 16 preamble bits instead of 11
            Writer::write_bit(0);
        }

        void write_byte(uint8_t byte) {
            Writer::write_byte(byte);
            Writer::write_bit(0);

            checksum ^= byte; // Checsum is exclusive or of every byte send
        }

        void frame_close() {
            Writer::write_byte(checksum);
            Writer::write_bit(1);
        }
    };


    template <typename Writer>
    class DccController {
    private:
        Writer frame_writer;

    public:
        void send_command(const Command &command) {
            frame_writer.frame_open();
            frame_writer.write_byte(command.address);
            frame_writer.write_byte(command.instruction.value);
            frame_writer.frame_close();
        }
    };

    template<uint8_t TimingHigh, uint8_t TimingLow, auto Writer>
    struct TimingAdapter {
        static inline constexpr void write_byte(uint8_t byte) {
            for (uint8_t mask = 0x80; mask != 0; mask >>= 1) {
                write_bit((byte & mask) != 0);
            }
        }

        static inline constexpr void write_bit(bool bit) {
            auto timing = bit ? TimingHigh : TimingLow;
            Writer(timing);
            Writer(timing);
        }
    };
}