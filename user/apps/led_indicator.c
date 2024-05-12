/**
 * @file led_indicator.c
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "led_indicator.h"
#include "sys_app.h"

LED_RGB_t led_rgb;

#define SET_LED_FUNCTION_STATIC(red, green, blue) led_rgb.r.state=red,    \
                                    led_rgb.g.state=green,      \
                                    led_rgb.b.state=blue,       \
                                    led_rgb.r.is_blink=0,       \
                                    led_rgb.g.is_blink=0,       \
                                    led_rgb.b.is_blink=0,       \
                                    led_rgb.r.execute=0,        \
                                    led_rgb.g.execute=0,        \
                                    led_rgb.b.execute=0
                                    

#define SET_LED_FUNCTION_BLINK(red, green, blue) led_rgb.r.is_blink=red,    \
                                    led_rgb.g.is_blink=green,   \
                                    led_rgb.b.is_blink=blue,    \
                                    led_rgb.r.state=0,          \
                                    led_rgb.g.state=0,          \
                                    led_rgb.b.state=0,          \
                                    led_rgb.r.execute=0,        \
                                    led_rgb.g.execute=0,        \
                                    led_rgb.b.execute=0

#define SET_LED_FUNCTION_BLINK_PERIODE(rt, gt, bt) led_rgb.r.periode=rt,    \
                                    led_rgb.g.periode=gt,       \
                                    led_rgb.b.periode=bt


void led_indicator_init(void)
{
    led_rgb_init(&led_rgb);
}

inline void led_indicator_run(void *handler)
{
    led_rgb_callback(&led_rgb);
}

void led_indicator_set_in_mode(uint8_t mode, uint8_t *event)
{
    switch( mode ) {
        case SYS_MODE_SPOTIFY_CONNECT:
            if ( *event == WIFI_EVENT_CONNECTING ) 
            {
                SET_LED_FUNCTION_BLINK(1, 1, 0);
                //SET_LED_FUNCTION_BLINK_PERIODE(0, 1000, 0);
            } 
            else if ( *event == WIFI_EVENT_CONNECTED ) 
            {
                SET_LED_FUNCTION_STATIC(0, 1, 0);
            }
        break;

        case SYS_MODE_BT_A2DP:
            if ( *event == BT_EVENT_PAIRING ) 
            {
                SET_LED_FUNCTION_BLINK(0, 0, 1);
                //SET_LED_FUNCTION_BLINK_PERIODE(0, 0, 500);
            }
            else if ( *event == BT_EVENT_CONNECTED ){
        	    SET_LED_FUNCTION_STATIC(0, 0, 1);
            }
        break;

        case SYS_MODE_BOOTING:
        case SYS_MODE_NETWORK_CONFIG:
        case SYS_MODE_IDLE:
        case SYS_MODE_FACTORY_RESET:
        	SET_LED_FUNCTION_BLINK(1, 1, 1);
            SET_LED_FUNCTION_BLINK_PERIODE(500, 500, 500);
        break;
        case SYS_MODE_STANDBY:

        break;
    }
} 
