#ifndef ROTARY_KEY_H
#define ROTARY_KEY_H

#include "main.h"
#include "app_config.h"

/** rotary key A pin */
#define ROT_KEY_A_PORT  ROT_A_GPIO_Port
#define ROT_KEY_A_PIN   ROT_A_Pin

/* rotary key B pin */
#define ROT_KEY_B_PORT  ROT_B_GPIO_Port
#define ROT_KEY_B_PIN   ROT_B_Pin

typedef enum _CodeKeyType
{
    CODE_KEY_NONE = 0,
    CODE_KEY_FORWARD,
    CODE_KEY_BACKWARD,
    CODE_KEY2_FORWARD,
    CODE_KEY2_BACKWARD,
} CodeKeyType;

#define CodingKeyScanTimeSet 100

/** function prototype */
CodeKeyType rotary_key_scan(void);
void rotary_key_init(void);
void rotary_key_callback();


#endif /*ROTARY_KEY_H*/
