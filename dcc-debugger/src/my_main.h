
#ifndef MY_MAIN_H
#define MY_MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "protocol/dcc_decoder.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

#define HIGH_T 58U
#define LOW_T 100U
#define VARIANCE 8U

#define PULSE_BUF_SIZE 2048U
#define PULSE_BUF_MASK (PULSE_BUF_SIZE - 1U)

#define T_DUMP_MS 2500U

_Static_assert((PULSE_BUF_SIZE & PULSE_BUF_MASK) == 0U,
               "PULSE_BUF_SIZE must be a power of two");

#define UART_PRINT_LIT(huart, str) \
    HAL_UART_Transmit(huart, (uint8_t *)(str), (uint16_t)(sizeof(str) - 1), 100U)

/*
 * Single-producer / single-consumer ring buffer.
 *
 * Producer: TIM2 capture ISR
 * Consumer: main loop
 *
 * One slot is reserved to distinguish full from empty.
 * Usable capacity: 2047 samples.
 */
struct PulseBuffer {
    volatile uint32_t data[PULSE_BUF_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
};

typedef enum {
    PULSE_DECODER_BIT,
    PULSE_DECODER_INCOMPLETE,
    PULSE_DECODER_INVALID_PAIRING
} DecodePulseStatus;

struct PulseDecoder {
    uint8_t short_pulses;
    uint8_t long_pulses;

    bool latest_bit;
    DecodePulseStatus status;
};

struct Stats {
    volatile uint16_t pulses_dropped;
    volatile uint16_t pulses_captured;
};

struct ByteBuilder {
    uint8_t current_byte;
    uint8_t bit_idx;
};


static struct PulseBuffer pulse_buffer = { .head = 0U, .tail = 0U };
static struct Stats stats;
static struct PulseDecoder pulse_decoder;
static struct DccDecoder dcc_decoder;
static struct ByteBuilder byte_builder;

static uint32_t last_dump = 0U;

void my_setup(void)
{
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
    UART_PRINT_LIT(&huart2, "System started\r\n");
    init_decoder(&dcc_decoder, 0);
}

inline bool is_pulse_high(uint32_t pulse_us) {
    return pulse_us > (HIGH_T - VARIANCE) && pulse_us < (HIGH_T + VARIANCE);
}

inline bool is_pulse_low(uint32_t pulse_us) {
    return pulse_us > (LOW_T - VARIANCE) && pulse_us < (LOW_T + VARIANCE);
}

int print_bin(char *buf, uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        buf[7 - i] = (value & (1U << i)) ? '1' : '0';
    }
    buf[8] = '\0';
    return 9;
}

void buffer_write_pulse(
    struct PulseBuffer *buffer,
    struct Stats *stats,
    uint32_t pulse_ticks
) {
    uint16_t head = buffer->head;
    uint16_t next = (uint16_t)((head + 1U) & PULSE_BUF_MASK);

    stats->pulses_captured++;

    if (next == buffer->tail) {
        stats->pulses_dropped++;
        return;
    }

    buffer->data[head] = pulse_ticks;
    buffer->head = next; // Publish the entry only after writing the sample.
}


/*
 * Format and transmit one measurement.
 */
void print_pulse(uint32_t pulse_us)
{
    char buf[32];
    uint16_t size;

    if (is_pulse_high(pulse_us)) {
        buf[0] = '1';
        buf[1] = '\0';
        size = 1;
    } else if (is_pulse_low(pulse_us)) {
        buf[0] = '0';
        buf[1] = '\0';
        size = 1;
    } else if (pulse_us > 1800000) {
        buf[0] = '\r';
        buf[1] = '\n';
        buf[2] = '\0';
        size = 2;
    } else {
        size = (uint16_t)snprintf(buf, sizeof(buf), "[%lu]", (unsigned long)pulse_us);
    }

    HAL_UART_Transmit(&huart2, (uint8_t *)buf, size, 100U);
}

void byte_append_bit(struct ByteBuilder *builder, uint8_t bit) {
    // Append decoded bit MSB-first
    builder->current_byte |= (uint8_t)(bit << (7 - builder->bit_idx));
    builder->bit_idx++;

    if (builder->bit_idx > 7)
        builder->bit_idx = 0;
}

void decode_pulse(struct PulseDecoder *decoder, uint32_t pulse_us) {
    if (is_pulse_high(pulse_us)) {
        decoder->long_pulses = 0;
        decoder->short_pulses++;

        if (decoder->short_pulses < 2) {
            decoder->status = PULSE_DECODER_INCOMPLETE;
        } else {
            decoder->latest_bit = 1;
            decoder->short_pulses = 0;
            decoder->status = PULSE_DECODER_BIT;
        }
    } else if (is_pulse_low(pulse_us)) {
        decoder->short_pulses = 0;
        decoder->long_pulses++;

        if (decoder->long_pulses < 2) {
            decoder->status = PULSE_DECODER_INCOMPLETE;
        } else {
            decoder->latest_bit = 0;
            decoder->long_pulses = 0;
            decoder->status = PULSE_DECODER_BIT;
        }
    } else {
        // Invalid pulse - reset pairing
        decoder->short_pulses = 0;
        decoder->long_pulses = 0;
        decoder->status = PULSE_DECODER_INVALID_PAIRING;
    }
}

static void idk() {
    decode_pulse(&pulse_decoder, 32U);

    if (pulse_decoder.status != PULSE_DECODER_BIT)
        return;
    
    process_dcc_bit(&dcc_decoder, pulse_decoder.latest_bit);

    if (dcc_decoder.state == DCC_COMPLETE) {
        dcc_decoder.state = DCC_PREAMBLE;

        char msg[128];
        char instruction[9];
        char crc[9];
        const uint8_t expected_crc = dcc_decoder.address ^ dcc_decoder.instruction;

        print_bin(instruction, dcc_decoder.instruction);
        print_bin(crc, dcc_decoder.crc);

        int len = snprintf(
            msg,
            sizeof(msg),
            "Decoded a packet!\r\n"
            "  Addr: %u\r\n"
            "  Inst: 0b%s\r\n"
            "  CRC: 0b%s\r\n"
            "  Validity: %s\r\n\r\n",
            (unsigned)dcc_decoder.address,
            instruction,
            crc,
            (dcc_decoder.crc == expected_crc) ? "VALID" : "INVALID"
        );

        HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)len, 100U);
    }
}

/*
 * Print the samples that were queued when this function began.
 * The ISR continues capturing while UART transmission takes place.
 */
uint32_t process_pulse_buffer(struct PulseBuffer *buffer)
{
    uint16_t end_head = buffer->head;
    uint32_t pulses_processed = 0U;

    while (buffer->tail != end_head) {
        uint16_t tail = buffer->tail;
        uint32_t pulse_ticks = buffer->data[tail];

        /* Release the slot before transmitting, so the ISR can
         * reuse it while the comparatively slow UART is busy.
         */
        buffer->tail = (uint16_t)((tail + 1U) & PULSE_BUF_MASK);

        print_pulse(pulse_ticks);
        pulses_processed++;
    }

    return pulses_processed;
}

/*
 * Take a consistent snapshot of the 32-bit counters.
 * Cortex-M interrupt masking prevents the ISR from updating
 * these counters halfway through either read.
 */
void check_process_stats(struct Stats *stats, uint32_t pulses_processed) {
    uint32_t primask = __get_PRIMASK();
    uint32_t captured_snapshot;
    uint32_t dropped_snapshot;
    char buf[96];

    __disable_irq();

    captured_snapshot = stats->pulses_captured;
    dropped_snapshot = stats->pulses_dropped;

    if (primask == 0U)
        __enable_irq();

    int len = snprintf(
        buf,
        sizeof(buf),
        "\r\nProcessed: %lu, Captured: %lu, Dropped: %lu\r\n",
        (unsigned long)pulses_processed,
        (unsigned long)captured_snapshot,
        (unsigned long)dropped_snapshot
    );

    HAL_UART_Transmit(&huart2, (uint8_t *)buf, (uint16_t)len, 100U);
}

void my_loop(void)
{
    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_dump) >= T_DUMP_MS) {
        last_dump = now;
        
        uint32_t pulses_processed = process_pulse_buffer(&pulse_buffer);
        check_process_stats(&stats, pulses_processed);
    }
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 &&
        htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {

        uint32_t captured_ticks = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        buffer_write_pulse(&pulse_buffer, &stats, captured_ticks);
    }
}

#endif /* MY_MAIN_H */
