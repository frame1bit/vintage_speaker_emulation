/**
 * file: key.h 
*/
#ifndef KEY_H
#define KEY_H

#include "app_config.h"
#include "main.h"
#include "app_event_message.h"
#if (CONFIG_CAPSENSE_TOUCH_ENABLE)
#include "capsense.h"
#endif
#if (CONFIG_ROTARY_ENCODER_ENABLE)
#include "rotary_encoder/rotary_key.h"
#endif


typedef struct
{
	uint16_t dbclick_en;
	uint16_t dbclick_counter;
	uint16_t dbclick_timeout;

	uint32_t KeyMsg;
	uint32_t dbclick_msg;
}DBKey_t;

/** for double click feature */
#define  CFG_PARA_CLICK_MSG             MSG_VOL_UP
#define  CFG_PARA_DBCLICK_MSG           MSG_MODE
#define  CFG_PARA_DBCLICK_DLY_TIME      20

/**
 * use double click feature
*/
#define KEY_USE_DOUBLE_CLICK            (0)

/** prototype function */
void key_init(void);
MessageId key_scan(void);
/* end of prototype function */


/** extern function and variable */

/** end of extern functin and variable */

#endif /** KEY_H */
