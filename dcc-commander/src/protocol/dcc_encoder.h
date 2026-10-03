#pragma once

#include <stdint.h>

/*  The DCC Pakcet Format
    See https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
*/

namespace proto::dcc {
    // Packet
    union Instruction {
        struct {
            uint8_t speed    : 4; // SSSS (0000 = Stop, 0001 = E-Stop, 0010-1111 = Steps 1-14)
            uint8_t light    : 1; // C    (0 = Off, 1 = On)
            uint8_t direction: 1; // D    (Direction 0 = Reverse, 1 = Forward)
            uint8_t command  : 2; // 01   (Instruction data)
        } fields;

        uint8_t value;
    };
    
    // Frame
    struct Command {
        uint8_t     address; // Address is 8 bits
        Instruction instruction;
        uint8_t     checksum;
    };

    void encode_command(const Command &command, uint8_t *buffer);

    // 111111111111 0   0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1
    // Preamble         Byte One    Byte Two    Byte Three (Error Detection Data Byte)
    
    // Byte One: Address Data Byte
    // Byte Two: Instruction Data Byte
}