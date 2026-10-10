
#ifndef MY_MAIN_H
#define MY_MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

/*
 * Assumption: TIM2 is configured for one timer tick per microsecond.
 * HAL_TIM_ReadCapturedValue() returns timer counts, not inherently us.
 */
#define HIGH_T 58U
#define LOW_T 100U
#define VARIANCE 6U

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


void my_setup(void)
{
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}


/*
 * Called from the capture callback.
 * No formatting, UART output, or blocking operations here.
 */
static void checkpulse_buffered(uint32_t pulse_ticks)
{
    uint16_t head = pulse_head;
    uint16_t next =
        (uint16_t)((head + 1U) & PULSE_BUF_MASK);

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
static void printpulse(uint32_t pulse_us)
{
    char buf[32];

    if (pulse_us > (HIGH_T - VARIANCE) &&
        pulse_us < (HIGH_T + VARIANCE)) {
        buf[0] = '1';
        buf[1] = '\0';
    } else if (pulse_us > (LOW_T - VARIANCE) &&
               pulse_us < (LOW_T + VARIANCE)) {
        buf[0] = '0';
        buf[1] = '\0';
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


/*
 * Print the samples that were queued when this function began.
 * The ISR continues capturing while UART transmission takes place.
 */
static void printpulse_buffer(void)
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
        pulse_tail =
            (uint16_t)((tail + 1U) & PULSE_BUF_MASK);

        printpulse(pulse_ticks);
        printed++;
    }

    /*
     * Take a consistent snapshot of the 32-bit counters.
     * Cortex-M interrupt masking prevents the ISR from updating
     * these counters halfway through either read.
     */
    {
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

        (void)snprintf(
            buf,
            sizeof(buf),
            "\r\nPrinted: %lu, Captured: %lu, Dropped: %lu\r\n",
            (unsigned long)printed,
            (unsigned long)captured_snapshot,
            (unsigned long)dropped_snapshot
        );

        (void)HAL_UART_Transmit(
            &huart2,
            (uint8_t *)buf,
            (uint16_t)strlen(buf),
            100U
        );
    }
}


void my_loop(void)
{
    uint32_t now = HAL_GetTick();

    if ((uint32_t)(now - last_dump) >= 2500U) {
        last_dump = now;
        printpulse_buffer();
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

        checkpulse_buffered(captured_ticks);
        // printpulse(captured_ticks);
    }
}

#endif /* MY_MAIN_H */
