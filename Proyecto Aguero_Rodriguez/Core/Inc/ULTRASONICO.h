#ifndef ULTRASONICO_H
#define ULTRASONICO_H

#include "stm32f1xx_hal.h"

/* Distancia máxima medida (cm) */
#define DISTANCIA_MAX_CM 30

/* Handle del sensor ultrasónico */
typedef struct
{
    GPIO_TypeDef *TRIG_Port;
    uint16_t      TRIG_Pin;

    GPIO_TypeDef *ECHO_Port;
    uint16_t      ECHO_Pin;

    TIM_HandleTypeDef *htim;

} ULTRASONIC_HandleTypeDef;

/* Inicialización del sensor */
void ultrasonic_init(ULTRASONIC_HandleTypeDef *hus);

/* Medición de distancia en cm */
uint16_t ultrasonic_measure_distance(ULTRASONIC_HandleTypeDef *hus);

#endif /* ULTRASONICO_H */
