/**
 * @file potentiometer.h
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <stdint.h>
#include "main.h"

typedef
struct _potentiometer
{
    ADC_HandleTypeDef *adc_handler;

    uint32_t (*poll_read)(ADC_HandleTypeDef *handler);

    uint32_t adc_value;     /** adc value from interrupt */

} Potentiometer_t;


/** prototype function */
uint32_t potentiometer_adc_poll_read(ADC_HandleTypeDef *hadc);
/** end of prototype function */

#endif /** end of POTENTIOMETER */