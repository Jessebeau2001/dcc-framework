#pragma once

#include <types.h>
#include <HardwareSerial.h>
#include <protocol/dcc_def.h>
#include <common/command_map.hpp>
#include <HAL/bitbang.hpp>

/*  The DCC Pakcet Format
    See https://www.nmra.org/sites/default/files/s-92-2004-07.pdf

    How to frame is built up:
    111111111111 0   0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1
    Preamble         Byte One    Byte Two    Byte Three (Error Detection Data Byte)
    
    // Byte One: Address Data Byte
    // Byte Two: Instruction Data Byte
*/

namespace dcc {
    static constexpr u8 DCC_MAX_PACKET_SIZE = 6U; // 44 bits / 8 = 5.5 bytes = ~6 bytes

    template<typename Writer>
    class DccSerializer {
    private:
        uint8_t checksum = 0;

    public:
        DccSerializer() { }

        void frame_open() {
            checksum = 0;

            Writer::write_byte(0xFF); // Write the preamble
            Writer::write_byte(0xFF); // We choose 16 preamble bits instead of 11
            Writer::write_bit(0);
        }

        void write_byte(uint8_t byte) {
            Writer::write_byte(byte);
            Writer::write_bit(0);

            checksum ^= byte; // Checksum is exclusive or of every byte send
        }

        void frame_close() {
            Writer::write_byte(checksum);
            Writer::write_bit(1);
        }

        void serialize(const Command &command) {
            frame_open();
            write_byte(command.address);
            write_byte(static_cast<uint8_t>(command.instruction));
            frame_close();
        }
    };


    template<typename Writer>
    class DccScheduler {
    private:
        enum State {
            WRITE_IDLE,
            WRITE_ROUND_ROBIN
        };

        static constexpr Command IDLE_CMD = {
            .address = 0xFF,
            .instruction = 0x00
        };

        static constexpr u8 WRITER_SPACE_REQUIRED = 8 * DCC_MAX_PACKET_SIZE; // Keep 8 packets of size available

        DccSerializer<Writer> serializer;
        State state = State::WRITE_IDLE;
        
        CommandMap scheduled;
        CommandMap::Cursor cursor = scheduled.create_cursor();

        void write_idle() {
            serializer.serialize(IDLE_CMD);
        }

        void write_next() {
            const Command *cmd = cursor.next();
            /*
                REMOVED: if (cmd != nullptr)
                What a fantastic problem to have again... multiple sources of truth...
                Techinically if the state is WRITE_RR then next() could never return a nullptr
            */
            serializer.serialize(*cmd);
        }
    
    public:
        void update() {
            if (!(Writer::available() > WRITER_SPACE_REQUIRED))
                return;

            switch (state) {
            case WRITE_IDLE:
                write_idle();
                break;
            
            case WRITE_ROUND_ROBIN:
                write_next();
                break;
            }
        }

        bool schedule(Command &command) {
            const Command *result = scheduled.put(command);
            // No need to transition based on the result of put,
            // if the map is full then the state needs to be WRITE_RR anyway.
            // We do handle the result in order to be transparent to the caller though
            state = State::WRITE_ROUND_ROBIN;
            return result != nullptr;
        }

        void clear(u8 address) {
            scheduled.remove(address);
            if (scheduled.is_empty())
                state = State::WRITE_IDLE;
        }
    };


    template <typename Serializer>
    class DccController {
    private:
        Serializer serializer;

    public:
        void send_command(const Command &command) {
            serializer.frame_open();
            serializer.write_byte(command.address);
            serializer.write_byte(static_cast<uint8_t>(command.instruction));
            serializer.frame_close();
        }
    };

    
    struct SerialWriter {
        static inline void write_byte(uint8_t byte) {
            for (size_t i = 0; i < 8; i++) {
                auto isFlipped = ((byte << i) & 0b10000000) != 0;
                write_bit(isFlipped);
            }
        }

        static inline void write_bit(bool bit) {
            Serial.print(bit ? '1' : '0');
        }
    };

    /*
    void encode_manual_frame_example(const Command &cmd, uint8_t *out) {
        // uint64_t buffer = 0;
        // uint8_t *out = reinterpret_cast<uint8_t*>(&buffer);

        auto instruction = static_cast<uint8_t>(cmd.instruction);
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
    }*/
}