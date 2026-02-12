#include "ULTRASONICO.h"
#include "main.h"
#include "gpio.h"

void ultrasonic_init(ULTRASONIC_HandleTypeDef *hus)
{
    /* Iniciar timer base */
    HAL_TIM_Base_Start(hus->htim);

    /* Trigger en bajo */
    HAL_GPIO_WritePin(hus->TRIG_Port,
                      hus->TRIG_Pin,
                      GPIO_PIN_RESET);
}

uint16_t ultrasonic_measure_distance(ULTRASONIC_HandleTypeDef *hus)
{
    uint32_t Value1 = 0;
    uint32_t Value2 = 0;
    uint16_t Distance = DISTANCIA_MAX_CM;

    /* Pulso de trigger (10 us) */
    HAL_GPIO_WritePin(hus->TRIG_Port, hus->TRIG_Pin, GPIO_PIN_SET);
    __HAL_TIM_SET_COUNTER(hus->htim, 0);
    while (__HAL_TIM_GET_COUNTER(hus->htim) < 10);
    HAL_GPIO_WritePin(hus->TRIG_Port, hus->TRIG_Pin, GPIO_PIN_RESET);

    /* Esperar flanco de subida del ECHO (timeout 1 ms) */
    __HAL_TIM_SET_COUNTER(hus->htim, 0);
    while (!HAL_GPIO_ReadPin(hus->ECHO_Port, hus->ECHO_Pin))
    {
        if (__HAL_TIM_GET_COUNTER(hus->htim) > 1000)
            return DISTANCIA_MAX_CM;
    }

    Value1 = __HAL_TIM_GET_COUNTER(hus->htim);

    /* Esperar flanco de bajada del ECHO (timeout aprox 30 cm) */
    while (HAL_GPIO_ReadPin(hus->ECHO_Port, hus->ECHO_Pin))
    {
        if (__HAL_TIM_GET_COUNTER(hus->htim) > Value1 + 1800)
            return DISTANCIA_MAX_CM;
    }

    Value2 = __HAL_TIM_GET_COUNTER(hus->htim);

    /* Cálculo de distancia */
    Distance = (uint16_t)(((Value2 - Value1) * 0.034f) / 2.0f);

    /* Saturación */
    if (Distance > DISTANCIA_MAX_CM)
        Distance = DISTANCIA_MAX_CM;

    return Distance;
}
















