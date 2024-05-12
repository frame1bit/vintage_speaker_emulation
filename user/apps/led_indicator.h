/**
 * @file led_indicator.h
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include "led_rgb/led_rgb.h"


/** prototype function */
void led_indicator_init(void);
void led_indicator_run(void *handler);
void led_indicator_set_in_mode(uint8_t mode, uint8_t *event);
/** end of prototype function */

#endif /** end of LED_INDICATOR_H */