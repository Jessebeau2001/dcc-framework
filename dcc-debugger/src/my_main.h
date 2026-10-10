
#ifndef MY_MAIN_H
#define MY_MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "dcc_decoder.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

/*
 * Assumption: TIM2 is configured for one timer tick per microsecond.
 * HAL_TIM_ReadCapturedValue() returns timer counts, not inherently us.
 */
#define HIGH_T 58U
#define LOW_T 100U
#define VARIANCE 8U

#define PULSE_BUF_SIZE 2048U
#define PULSE_BUF_MASK (PULSE_BUF_SIZE - 1U)

_Static_assert((PULSE_BUF_SIZE & PULSE_BUF_MASK) == 0U,
               "PULSE_BUF_SIZE must be a power of two");

/*
 * Single-producer / single-consumer ring buffer.
 *
 * Producer: TIM2 capture ISR
 * Consumer: main loop
 *
 * One slot is reserved to distinguish full from empty.
 * Usable capacity: 2047 samples.
 */
static volatile uint32_t pulse_buffer[PULSE_BUF_SIZE];
static volatile uint16_t pulse_head = 0U;
static volatile uint16_t pulse_tail = 0U;

static volatile uint32_t pulse_dropped = 0U;
static volatile uint32_t pulse_captured = 0U;

static uint32_t last_dump = 0U;

static Decoder dcc_decoder;


void my_setup(void)
{
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    const char *welcome_msg = "System started\r\n";

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)welcome_msg,
        (uint16_t)strlen(welcome_msg),
        100U
    );

    init_decoder(&dcc_decoder, 0);
}

inline bool is_pulse_high(uint32_t pulse_us) {
    return pulse_us > (HIGH_T - VARIANCE) && pulse_us < (HIGH_T + VARIANCE);
}

inline bool is_pulse_low(uint32_t pulse_us) {
    return pulse_us > (LOW_T - VARIANCE) && pulse_us < (LOW_T + VARIANCE);
}


/*
 * Called from the capture callback.
 * No formatting, UART output, or blocking operations here.
 */
static void record_pulse(uint32_t pulse_ticks)
{
    uint16_t head = pulse_head;
    uint16_t next = (uint16_t)((head + 1U) & PULSE_BUF_MASK);

    pulse_captured++;

    if (next == pulse_tail) {
        pulse_dropped++;
        return;
    }

    pulse_buffer[head] = pulse_ticks;

    /* Publish the entry only after writing the sample. */
    pulse_head = next;
}


/*
 * Format and transmit one measurement.
 * Called exclusively from main-loop context.
 */
static void print_pulse(uint32_t pulse_us)
{
    char buf[32];

    if (is_pulse_high(pulse_us)) {
        buf[0] = '1';
        buf[1] = '\0';
    } else if (is_pulse_low(pulse_us)) {
        buf[0] = '0';
        buf[1] = '\0';
    } else if (pulse_us > 1800000) {
        buf[0] = '\r';
        buf[1] = '\n';
        buf[2] = '\0';
    } else {
        snprintf(buf, sizeof(buf), "[%lu]", (unsigned long)pulse_us);
    }

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)buf,
        (uint16_t)strlen(buf),
        100U
    );
}


static uint8_t short_pulse_count = 0;
static uint8_t long_pulse_count = 0;
static uint8_t checked_byte = 0;
static uint8_t current_bit = 0;

void append_bit_for_print(uint8_t bit) {
    // Append decoded bit MSB-first
    checked_byte |= (uint8_t)(bit << (7 - current_bit));
    current_bit++;

    if (current_bit >= 8)
    {
        // Complete byte is ready in checked_byte.
        char buf[16];

        snprintf(buf, sizeof(buf), "0b%d%d%d%d%d%d%d%d\r\n",
            (checked_byte >> 7) & 1,
            (checked_byte >> 6) & 1,
            (checked_byte >> 5) & 1,
            (checked_byte >> 4) & 1,
            (checked_byte >> 3) & 1,
            (checked_byte >> 2) & 1,
            (checked_byte >> 1) & 1,
            checked_byte        & 1);

        HAL_UART_Transmit(
            &huart2,
            (uint8_t *)buf,
            (uint16_t)strlen(buf),
            100U
        );

        checked_byte = 0;
        current_bit = 0;
    }
}

static void process_pulse(uint32_t pulse_us)
{
    uint8_t decoded_bit;

    if (is_pulse_high(pulse_us))
    {
        long_pulse_count = 0;
        short_pulse_count++;

        if (short_pulse_count < 2)
            return;

        // Two consecutive short pulses = 1
        decoded_bit = 1;
        short_pulse_count = 0;
    }
    else if (is_pulse_low(pulse_us))
    {
        short_pulse_count = 0;
        long_pulse_count++;

        if (long_pulse_count < 2)
            return;

        // Two consecutive long pulses = 0
        decoded_bit = 0;
        long_pulse_count = 0;
    }
    else
    {
        // Invalid pulse: reset pairing
        short_pulse_count = 0;
        long_pulse_count = 0;
        return;
    }

    process_dcc_bit(&dcc_decoder, decoded_bit);

    if (dcc_decoder.state == COMPLETE) {
        dcc_decoder.state = PREAMBLE;

        const expected_crc = dcc_decoder.address ^ dcc_decoder.instruction;
        char msg[128];

        int len = snprintf(
            msg,
            sizeof(msg),
            "Decoded a packet!\r\n"
            "  Addr: %u\r\n"
            "  Inst: %u\r\n"
            "  CRC: %u\r\n"
            "  Validity: %s\r\n\r\n",
            (unsigned)dcc_decoder.address,
            (unsigned)dcc_decoder.instruction,
            (unsigned)dcc_decoder.crc,
            dcc_decoder.crc == expected_crc ? "VALID" : "INVALID"
        );

        HAL_UART_Transmit(
            &huart2,
            (uint8_t *)msg,
            (uint16_t)len,
            100U
        );
    }

    // append_bit_for_print(decoded_bit);
}

/*
 * Take a consistent snapshot of the 32-bit counters.
 * Cortex-M interrupt masking prevents the ISR from updating
 * these counters halfway through either read.
 */
static void print_snapshot_stats(uint32_t printed) {
    uint32_t primask = __get_PRIMASK();
    uint32_t captured_snapshot;
    uint32_t dropped_snapshot;
    char buf[96];

    __disable_irq();

    captured_snapshot = pulse_captured;
    dropped_snapshot = pulse_dropped;

    if (primask == 0U) {
        __enable_irq();
    }

    snprintf(
        buf,
        sizeof(buf),
        "\r\nPrinted: %lu, Captured: %lu, Dropped: %lu\r\n",
        (unsigned long)printed,
        (unsigned long)captured_snapshot,
        (unsigned long)dropped_snapshot
    );

    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)buf,
        (uint16_t)strlen(buf),
        100U
    );
}

/*
 * Print the samples that were queued when this function began.
 * The ISR continues capturing while UART transmission takes place.
 */
static void process_pulse_buffer(void)
{
    uint16_t end_head = pulse_head;
    uint32_t printed = 0U;

    while (pulse_tail != end_head) {
        uint16_t tail = pulse_tail;
        uint32_t pulse_ticks = pulse_buffer[tail];

        /*
         * Release the slot before transmitting, so the ISR can
         * reuse it while the comparatively slow UART is busy.
         */
        pulse_tail = (uint16_t)((tail + 1U) & PULSE_BUF_MASK);

        // print_pulse(pulse_ticks);
        process_pulse(pulse_ticks);
        printed++;
    }

    // print_snapshot_stats(printed);
}


void my_loop(void)
{
    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_dump) >= 2500U) {
        last_dump = now;
        process_pulse_buffer();
    }
}


/*
 * HAL timer input-capture callback.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 &&
        htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {

        uint32_t captured_ticks = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        record_pulse(captured_ticks);
    }
}

#endif /* MY_MAIN_H */
