/***
 * file:    led_rgb.c
 * desc:    implementation for led_rgb
 * 
 * non PWM mode
 * TODO: enable PWM fiture
*/

#include <stdint.h>
#include "led_rgb.h"


/*** 
 * @brief   init led rgb for aux indicator
*/
void led_rgb_init(LED_RGB_t *led)
{
    led->gpio_write = HAL_GPIO_WritePin;
    led->gpio_toggle = HAL_GPIO_TogglePin;
}

/***
 * @brief   control rgb led state
 * @param   led:  led context
 * @return  None
*/
void led_rgb_callback(LED_RGB_t *led)
{
    /*** led red state */
    if (led->r.is_blink) 
    {
        if ( IsTimeout(&led->r.timer) ) {
            TimeoutSet(&led->r.timer, led->r.periode);
            led->gpio_toggle(LED_R_GPIO_Port, LED_R_Pin);
        }
    }
    else 
    {
        if ( !led->r.execute )
        {
            led->gpio_write(LED_R_GPIO_Port, LED_R_Pin, led->r.state);
            led->r.execute = 1;
        }
    }

    /** led green state */
    if (led->g.is_blink) 
    {
        if ( IsTimeout(&led->g.timer) ) {
            TimeoutSet(&led->g.timer, led->g.periode);
            led->gpio_toggle(LED_G_GPIO_Port, LED_G_Pin);
        }
    }
    else 
    {
        if ( !led->g.execute )
        {
            led->gpio_write(LED_G_GPIO_Port, LED_G_Pin, led->g.state);
            led->g.execute = 1;
        }
    }

    /** led blue state */
    if (led->b.is_blink) 
    {
        if ( IsTimeout(&led->b.timer) ) {
            TimeoutSet(&led->b.timer, led->b.periode);
            led->gpio_toggle(LED_B_GPIO_Port, LED_B_Pin);
        }
    }
    else 
    {
        if ( !led->b.execute)
        {
            led->gpio_write(LED_B_GPIO_Port, LED_B_Pin, led->b.state);
            led->b.execute = 1;
        }
    }


}