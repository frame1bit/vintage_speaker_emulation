/***
 * file: led
 */
#ifndef LED_RGB_H
#define LED_RGB_H

#include <stdint.h>
#include "timeout.h"
#include "main.h"

typedef struct
{
    uint8_t is_blink : 1;   // blink state 0: no blink (steady) 1: blink
    uint8_t state : 1;
    uint8_t execute : 1;   // execute state to prevent cotinues write to led

    TIMER timer;        // timer timout used for
    uint32_t periode; 

} LED_t;

typedef struct
{
    LED_t r;  /** state of led RGB */
    LED_t g;  /** state of led RGB */
    LED_t b;  /** state of led RGB */

    void (*gpio_toggle)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    void (*gpio_write)(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
} LED_RGB_t;

/***
 * enum for led rgb
 */
enum
{
    LED_ID_RED = 0,
    LED_ID_GREEN,
    LED_ID_BLUE,
    LED_ID_NUM
};

/***
 * @brief   init led rgb for aux indicator
 */
void led_rgb_init(LED_RGB_t *led);
void led_rgb_callback(LED_RGB_t *led);

#endif /** end of LED_RGB_H */
