/**
 * @file potentiometer.c
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "potentiometer.h"

extern Potentiometer_t potensio;


/***
 * @brief read adc value with polling methode
 * 
 * @param   handler    handler for adc channel
 * @return  val     adc value 
*/
__uint32_t potentiometer_adc_poll_read(ADC_HandleTypeDef *handler)
{
    __uint32_t val = 0;

    HAL_ADC_Start(handler);
    HAL_ADC_PollForConversion(handler, HAL_MAX_DELAY);
    val = HAL_ADC_GetValue(handler);
    HAL_ADC_Stop(handler);

    return val;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    __uint32_t value;
    value = HAL_ADC_GetValue(hadc);
    potensio.irq_callback(value);
    HAL_ADC_Start_IT(hadc);
}

