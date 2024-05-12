#ifndef BUTTON_H
#define BUTTON_H

#include "app_config.h"

#define IO_CHANNEL_EMPTY  (0xFF)

typedef enum _IOKeyType
{
	IO_KEY_UNKOWN_TYPE = 0,
	IO_KEY_PRESSED,
	IO_KEY_RELEASED,
	IO_KEY_LONG_PRESSED,
	IO_KEY_LONG_PRESS_HOLD,
	IO_KEY_LONG_RELEASED,
}IOKeyType;

typedef struct _IOKeyMsg
{
    uint16_t index;
    uint16_t type;
}IOKeyMsg;

/** prototype function */
IOKeyMsg iobtn_key_scan(void);
void iobtn_key_init(void);

#endif /*BUTTON_H*/
