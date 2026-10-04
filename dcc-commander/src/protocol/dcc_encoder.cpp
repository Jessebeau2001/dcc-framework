#include "dcc_encoder.h"

namespace proto::dcc::legacy {

void encode_frame(const Command &cmd, uint8_t *out) {
    // uint64_t buffer = 0;
    // uint8_t *out = reinterpret_cast<uint8_t*>(&buffer);

    auto instruction = cmd.instruction.value;

    // Byte 0 & 1 [11111111 11111111]: 16-bit preamble
    out[0] = 0xFF;
    out[1] = 0xFF;

    // Byte 2 [0AAAAAAA]: Packet start bit + Address Lower 7 bits
    out[2] = cmd.address & 0b01111111;

    // Byte 3 [A0IIIIII]: address bit 7 + Data start bit + Instruction lower 6 bits
    out[3] = (cmd.address & 0b10000000) | (instruction & 0b00111111);

    // Byte 4 [II0EEEEE]: instruction upper 2 bits + data Start bit + ERC lower 5 bits
    out[4] = (instruction & 0b11000000) | (cmd.checksum & 0b00011111);

    // Byte 5 [EEE1....]: ERC upper 3 bits + Packet end bit
    out[5] = (cmd.checksum & 0b11100000) | 0b00010000;

    // GOAL: 11111111 11111111 0AAAAAAA A0IIIIII II0EEEEE EEE1....
}

} // namespace proto::dcc


