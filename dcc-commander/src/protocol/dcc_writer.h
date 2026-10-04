#pragma once

#include <stdint.h>

/*  The DCC Pakcet Format
    See https://www.nmra.org/sites/default/files/s-92-2004-07.pdf

    How to frame is built up:
    111111111111 0   0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1
    Preamble         Byte One    Byte Two    Byte Three (Error Detection Data Byte)
    
    // Byte One: Address Data Byte
    // Byte Two: Instruction Data Byte
*/

namespace proto::dcc {
    enum class Speed : uint8_t {
        Stop       = 0b00000, // 0
        E_Stop     = 0b00001, // 1
        Stop_I     = 0b10000, // 16
        E_Stop_I   = 0b10001, // 17
        Step1      = 0b00010, // 2
        Step2      = 0b10010, // 18
        Step3      = 0b00011, // 3
        Step4      = 0b10011, // 19

        Step5      = 0b00100, // 4
        Step6      = 0b10100, // 20
        Step7      = 0b00101, // 5
        Step8      = 0b10101, // 21
        Step9      = 0b00110, // 6
        Step10     = 0b10110, // 22
        Step11     = 0b00111, // 7
        Step12     = 0b10111, // 23

        Step13     = 0b01000, // 8
        Step14     = 0b11000, // 24
        Step15     = 0b01001, // 9
        Step16     = 0b11001, // 25
        Step17     = 0b01010, // 10
        Step18     = 0b11010, // 26
        Step19     = 0b01011, // 11
        Step20     = 0b11011, // 27

        Step21     = 0b01100, // 12
        Step22     = 0b11100, // 28
        Step23     = 0b01101, // 13
        Step24     = 0b11101, // 29
        Step25     = 0b01110, // 14
        Step26     = 0b11110, // 30
        Step27     = 0b01111, // 15
        Step28     = 0b11111  // 31
    };

    enum class Direction : uint8_t {
        FORWARD  = 0,
        BACKWARD = 1
    };

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

    void encode_manual_frame_example(const Command &cmd, uint8_t *out) {
        // uint64_t buffer = 0;
        // uint8_t *out = reinterpret_cast<uint8_t*>(&buffer);

        auto instruction = cmd.instruction.value;
        uint8_t checksum = 0; // Should be bitwise xor on all bytes

        // Byte 0 & 1 [11111111 11111111]: 16-bit preamble
        out[0] = 0xFF;
        out[1] = 0xFF;

        // Byte 2 [0AAAAAAA]: Packet start bit + Address Lower 7 bits
        out[2] = cmd.address & 0b01111111;

        // Byte 3 [A0IIIIII]: address bit 7 + Data start bit + Instruction lower 6 bits
        out[3] = (cmd.address & 0b10000000) | (instruction & 0b00111111);

        // Byte 4 [II0EEEEE]: instruction upper 2 bits + data Start bit + ERC lower 5 bits
        out[4] = (instruction & 0b11000000) | (checksum & 0b00011111);

        // Byte 5 [EEE1....]: ERC upper 3 bits + Packet end bit
        out[5] = (checksum & 0b11100000) | 0b00010000;

        // GOAL: 11111111 11111111 0AAAAAAA A0IIIIII II0EEEEE EEE1....
    }
}