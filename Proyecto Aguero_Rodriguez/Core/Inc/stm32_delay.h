#ifndef INC_STM32_DELAY_H_
#define INC_STM32_DELAY_H_

#include "main.h"

uint32_t MX_Delay_Init(void);

__STATIC_INLINE void HAL_Delay_US(volatile uint32_t microseconds)
{
	uint32_t clk_cycle_start = DWT->CYCCNT;
	microseconds *= (uint32_t)(HAL_RCC_GetHCLKFreq() / 1000000);
	while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

__STATIC_INLINE void HAL_Delay_MS(volatile uint32_t milliseconds)
{
	uint32_t clk_cycle_start = DWT->CYCCNT;
	milliseconds *= (uint32_t)(HAL_RCC_GetHCLKFreq() / 1000);
	while ((DWT->CYCCNT - clk_cycle_start) < milliseconds);
}

#endif
