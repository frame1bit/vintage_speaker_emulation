#include "main.h"
#include <stdint.h>
#include "button.h"
#include "timeout.h"


#if (CONFIG_ROTARY_SWITCH)
#define KEY_2_PORT      ROT_SW_GPIO_Port
#define KEY_2_PIN       ROT_SW_Pin
#endif
/** reserved for any button */



#define 	IO_KEY_SCAN_TIME			20
#define 	IO_KEY_JITTER_TIME			30
#define 	IO_KEY_CP_TIME				1000 	//for hold or long press
#define 	IO_KEY_CPH_TIME			    200		//for long long press

#define		IO_KEY_FULL_VAL			    4096


typedef enum _IO_KEY_STATE
{
	IO_KEY_STATE_IDLE,
	IO_KEY_STATE_JITTER,
	IO_KEY_STATE_PRESS_DOWN,
	IO_KEY_STATE_CP
} IO_KEY_STATE;

TIMER tmr_key_wait, tmr_key_scan;
IO_KEY_STATE io_key_state;

/**
 * @brief init key button
*/
void iobtn_key_init(void)
{
    io_key_state = IO_KEY_STATE_IDLE;
    TimeoutSet(&tmr_key_scan, IO_KEY_SCAN_TIME);
}

/****
 * @brief   get io button channel (can used in multi channel)
 * @param   none
 * @return  channel number
*/
static uint8_t iobtn_channel_key_get(void)
{
    uint8_t key_index = IO_CHANNEL_EMPTY;

    if ( 0 )
    {
        key_index = 0;
    }
#if (CONFIG_ROTARY_SWITCH)
    else if ( !HAL_GPIO_ReadPin(KEY_2_PORT, KEY_2_PIN) )
    {
        key_index = 1;
    }
#endif
    else 
    {
        // reserved
    }

    return (key_index);
}

/**
 * @brief
*/
IOKeyMsg iobtn_key_scan(void)
{
    static uint8_t pre_key_index = IO_CHANNEL_EMPTY;
    uint8_t key_index;
    IOKeyMsg Msg = {IO_CHANNEL_EMPTY, IO_KEY_UNKOWN_TYPE};

    if (!IsTimeout(&tmr_key_scan))
    {
        return Msg;
    }
    TimeoutSet(&tmr_key_scan, IO_KEY_SCAN_TIME);

    key_index = iobtn_channel_key_get();

    switch(io_key_state)
    {
        case IO_KEY_STATE_IDLE:
            if (key_index == IO_CHANNEL_EMPTY) return Msg;
            pre_key_index = key_index;
            TimeoutSet(&tmr_key_wait, IO_KEY_JITTER_TIME);
            io_key_state = IO_KEY_STATE_JITTER;

        case IO_KEY_STATE_JITTER:
            if (pre_key_index != key_index)
            {
                pre_key_index = IO_CHANNEL_EMPTY;
                io_key_state = IO_KEY_STATE_IDLE;
            }
            else if (IsTimeout(&tmr_key_wait))
            {
                TimeoutSet(&tmr_key_wait, IO_KEY_CP_TIME);
                io_key_state = IO_KEY_STATE_PRESS_DOWN;
                Msg.index = pre_key_index;
                Msg.type = IO_KEY_PRESSED;
            }
            break;

        case IO_KEY_STATE_PRESS_DOWN:
            if (pre_key_index != key_index)
            {
                io_key_state = IO_KEY_STATE_IDLE;
                Msg.index = pre_key_index;
                Msg.type = IO_KEY_RELEASED;
            }
            else if (IsTimeout(&tmr_key_wait))
            {
                TimeoutSet(&tmr_key_wait, IO_KEY_CPH_TIME);
                io_key_state = IO_KEY_STATE_CP;
                Msg.index = pre_key_index;
                Msg.type = IO_KEY_LONG_PRESSED;
            }
            break;

        case IO_KEY_STATE_CP:
            if (pre_key_index != key_index)
            {
                io_key_state = IO_KEY_STATE_IDLE;
                Msg.index = pre_key_index;
                Msg.type = IO_KEY_LONG_RELEASED;
                pre_key_index = IO_CHANNEL_EMPTY;
            }
            else if (IsTimeout(&tmr_key_wait))
            {
                TimeoutSet(&tmr_key_wait, IO_KEY_CPH_TIME);
                Msg.index = pre_key_index;
                Msg.type = IO_KEY_LONG_PRESS_HOLD;
            }
            break;

        default: 
            io_key_state = IO_KEY_STATE_IDLE;
            pre_key_index = IO_CHANNEL_EMPTY;
            break;
    }

    return (Msg);
}
