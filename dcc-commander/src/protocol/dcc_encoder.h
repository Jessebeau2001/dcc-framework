#pragma once

#include <stdint.h>

/*  The DCC Pakcet Format
    See https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
*/

namespace proto::dcc::legacy {
    // Packet
    union Instruction {
        struct {
            uint8_t speed    : 4; // SSSS (0000 = Stop, 0001 = E-Stop, 0010-1111 = Steps 1-14)
            uint8_t light    : 1; // C    (0 = Off, 1 = On)
            uint8_t direction: 1; // D    (Direction 0 = Reverse, 1 = Forward)
            uint8_t idata    : 2; // 01   (Instruction data)
        } fields;

        uint8_t value;
    };

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

    // Frame
    struct Command {
        uint8_t     address; // Address is 8 bits
        Instruction instruction;
        uint8_t     checksum;
    };

    void encode_frame(const Command &command, uint8_t *frame);

    // 111111111111 0   0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1
    // Preamble         Byte One    Byte Two    Byte Three (Error Detection Data Byte)
    
    // Byte One: Address Data Byte
    // Byte Two: Instruction Data Byte
}