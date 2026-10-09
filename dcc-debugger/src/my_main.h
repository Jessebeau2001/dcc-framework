#ifndef MY_MAIN_H
#define MY_MAIN_H

#include <string.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

void my_setup() {
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

void my_loop() {

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        // CCR1 contains the microsecond duration since the previous edge reset
        uint32_t elapsed_us = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        char buf[64];
        snprintf(buf, sizeof(buf), "Pulse Duration: %lu us\r\n", elapsed_us);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 10);
    }
}

#endif // MY_MAIN_H