#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DCC_PREAMBLE,
    DCC_REC_ADDR,
    DCC_REC_INST,
    DCC_REC_CRC,
    DCC_COMPLETE
} DecoderState;

struct DccDecoder {
    DecoderState state;

    uint8_t preambles;
    uint8_t bit_idx;

    uint8_t address;
    uint8_t instruction;
    uint8_t crc;
};

void init_decoder(struct DccDecoder *decoder, uint8_t n_preables);
uint8_t set_bit_at(uint8_t byte, uint8_t idx, bool is_set); // From MSB to LSB
void process_dcc_bit(struct DccDecoder *decoder, bool bit);

/*
11111111
11111111
0
00110111
0
01110100
0
01100011
1
*/

#endif