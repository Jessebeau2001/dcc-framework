#ifndef DCC_DECODER_H
#define DCC_DECODER_H

#include <stdint.h>
#include <stdbool.h>

#define MIN_PREAMBLES 12
#define FIELD_BITS 8

typedef enum {
    PREAMBLE,
    REC_ADDR,
    REC_INST,
    REC_CRC,
    COMPLETE
} DecoderState;

typedef struct {
    uint8_t preambles;
    uint8_t bit_idx;

    uint8_t address;
    uint8_t instruction;
    uint8_t crc;

    DecoderState state;
} Decoder;

void init_decoder(Decoder *decoder, uint8_t init_preamble) {
    decoder->preambles   = init_preamble;
    decoder->bit_idx     = 0;

    decoder->address     = 0;
    decoder->instruction = 0;
    decoder->crc         = 0;

    decoder->state       = PREAMBLE;
}

// From MSB to LSB
uint8_t set_bit(uint8_t byte, uint8_t idx, bool is_set) {
    uint8_t mask = 1 << (7 - idx);
    return is_set
        ? byte | mask
        : byte & ~mask;
}

void process_dcc_bit(Decoder *decoder, bool bit) {
    switch (decoder->state)
    {
    case PREAMBLE:
        if (bit == 1) {
            // Preamble state active - record '1' bit
            decoder->preambles += 1;
        } else if (decoder->preambles > MIN_PREAMBLES) {
            // Preamble valid & separator '0' bit received
            decoder->preambles = 0;      // Clear preambles
            decoder->state = REC_ADDR;   // Advance state
        } else {
            // Not enough preamble bits yet but seperator '0' received:
            // Do nothing
        }
        break;
    
    case REC_ADDR:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected address bit
            decoder->address = set_bit(decoder->address, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 0) {
            // Expected separator bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state = REC_INST;   // Advance state
        } else {
            // Expected separator bit but bit was not received
            init_decoder(decoder, 1);
        }
        break;
    
    case REC_INST:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected instruction bit
            decoder->instruction = set_bit(decoder->instruction, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 0) {
            // Expected separator bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state   = REC_CRC;  // Advance state
        } else {
            // Expected separator bit but bit was not received
            init_decoder(decoder, 1);
        }
        break;

    case REC_CRC:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected crc bit
            decoder->crc = set_bit(decoder->crc, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 1) {
            // Expected frame end bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state = COMPLETE;   // Advance exit state
        } else {
            // Expected frame end bit but bit was not received
            init_decoder(decoder, 0);
        }
        break;
    
    case COMPLETE:
        // For now drop bit if complete
        break;
    }
}

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