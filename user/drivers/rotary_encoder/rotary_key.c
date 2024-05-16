#include "rotary_key.h"
#include <stdint.h>
#include "timeout.h"

static uint8_t ClockWiseCnt;
static uint8_t CounterClockWiseCnt;
uint8_t CodingKeyInt;

uint32_t rotary_val[2];

TIMER CodingKeyScanTimer;

#define	ROTARY_TIMER	htim1


 void rotary_key_callback()
 {

	/** polling rotary key */
	//rotary_key_scan_poll();

 }


/**
 * @brief init rotary key
 */
void rotary_key_init(void)
{

#if (CONFIG_ROTARY_USING_TIMER)
   HAL_TIM_Encoder_Start(&ROTARY_TIMER, TIM_CHANNEL_ALL);
#else
    ClockWiseCnt = 0;
    CounterClockWiseCnt = 0;
    CodingKeyInt = 0;
#endif
    
}

CodeKeyType rotary_key_scan(void)
{

    CodeKeyType CodeKey = CODE_KEY_NONE;

    if (!IsTimeout(&CodingKeyScanTimer))
    {
        return CodeKey;
    }
    TimeoutSet(&CodingKeyScanTimer, 100);

#if (CONFIG_ROTARY_USING_TIMER)
    rotary_val[0] = __HAL_TIM_GET_COUNTER(&htim1)>>2;

    if (rotary_val[0] > rotary_val[1]) 
    {
        CodeKey = CODE_KEY_FORWARD;
    }
    else if (rotary_val[0] < rotary_val[1]) 
    {
        CodeKey = CODE_KEY_BACKWARD;
    }
    rotary_val[1] = rotary_val[0];

#else
    if (ClockWiseCnt > 1)
    {
        CodeKey = CODE_KEY_FORWARD;
    }
    else if (CounterClockWiseCnt > 1)
    {
        CodeKey = CODE_KEY_BACKWARD;
    }

    ClockWiseCnt = 0;
    CounterClockWiseCnt = 0;

#endif

    return CodeKey;

}

/**
 * @brief polling rotary in 1ms tick
*/
void rotary_key_scan_poll(void)
{

#if (CONFIG_ROTARY_USING_TIMER)

#else

    if (CodingKeyInt == 0)
    {
        if ((HAL_GPIO_ReadPin(ROT_A_GPIO_Port, ROT_A_Pin)) && !(HAL_GPIO_ReadPin(ROT_B_GPIO_Port, ROT_B_Pin)))
        {
            CodingKeyInt = 1;
        }
    }
    else
    {
        if (!(HAL_GPIO_ReadPin(ROT_A_GPIO_Port, ROT_A_Pin)))
        {
            if (HAL_GPIO_ReadPin(ROT_B_GPIO_Port, ROT_B_Pin))
            {
                // counterclockwise rotation
                CounterClockWiseCnt++;
            }
            else
            {
                // clockwise rotation
                ClockWiseCnt++;
            }

            CodingKeyInt = 0;
        }
    }

#endif

}

