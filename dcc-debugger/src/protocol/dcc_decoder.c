#include "dcc_decoder.h"

#include <stdbool.h>

#define MIN_PREAMBLES 12
#define FIELD_BITS 8

void init_decoder(struct DccDecoder *decoder, uint8_t n_preables)
{
    decoder->preambles   = n_preables;
    decoder->bit_idx     = 0;

    decoder->address     = 0;
    decoder->instruction = 0;
    decoder->crc         = 0;

    decoder->state       = DCC_PREAMBLE;
}

uint8_t set_bit_at(uint8_t byte, uint8_t idx, bool is_set) {
    uint8_t mask = 1 << (7 - idx);
    return is_set
        ? byte | mask
        : byte & ~mask;
}

void process_dcc_bit(struct DccDecoder *decoder, bool bit) {
    switch (decoder->state)
    {
    case DCC_PREAMBLE:
        if (bit == 1) {
            // Preamble state active - record '1' bit
            decoder->preambles += 1;
        } else if (decoder->preambles > MIN_PREAMBLES) {
            // Preamble valid & separator '0' bit received
            decoder->preambles = 0;      // Clear preambles
            decoder->state = DCC_REC_ADDR;   // Advance state
        } else {
            // Not enough preamble bits yet but seperator '0' received:
            // Do nothing
        }
        break;
    
    case DCC_REC_ADDR:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected address bit
            decoder->address = set_bit_at(decoder->address, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 0) {
            // Expected separator bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state = DCC_REC_INST;   // Advance state
        } else {
            // Expected separator bit but bit was not received
            init_decoder(decoder, 1);
        }
        break;
    
    case DCC_REC_INST:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected instruction bit
            decoder->instruction = set_bit_at(decoder->instruction, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 0) {
            // Expected separator bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state   = DCC_REC_CRC;  // Advance state
        } else {
            // Expected separator bit but bit was not received
            init_decoder(decoder, 1);
        }
        break;

    case DCC_REC_CRC:
        if (decoder->bit_idx < FIELD_BITS) {
            // Write expected crc bit
            decoder->crc = set_bit_at(decoder->crc, decoder->bit_idx, bit);
            decoder->bit_idx += 1;
        } else if (bit == 1) {
            // Expected frame end bit & recieved it
            decoder->bit_idx = 0;        // Reset bit_idx
            decoder->state = DCC_COMPLETE;   // Advance exit state
        } else {
            // Expected frame end bit but bit was not received
            init_decoder(decoder, 0);
        }
        break;
    
    case DCC_COMPLETE:
        // For now drop bit if complete
        break;
    }
}
