#include "rotary_key.h"
#include <stdint.h>
#include "timeout.h"

static uint8_t ClockWiseCnt;
static uint8_t CounterClockWiseCnt;
uint8_t CodingKeyInt;

TIMER CodingKeyScanTimer;

#define	ROTARY_TIMER	htim1

 #if (CONFIG_ROTARY_ENCODER_ENABLE)
 void rotary_key_callback()
 {

	/** polling rotary key */
	//rotary_key_scan_poll();

 }
 #endif



/**
 * @brief init rotary key
 */
void rotary_key_init(void)
{
    ClockWiseCnt = 0;
    CounterClockWiseCnt = 0;
    CodingKeyInt = 0;

    HAL_TIM_Base_Start_IT(&ROTARY_TIMER);
}

CodeKeyType rotary_key_scan(void)
{
    CodeKeyType CodeKey = CODE_KEY_NONE;

    if (!IsTimeout(&CodingKeyScanTimer))
    {
        return CodeKey;
    }
    TimeoutSet(&CodingKeyScanTimer, 50);

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

    return CodeKey;
}

/**
 * @brief polling rotary in 1ms tick
*/
void rotary_key_scan_poll(void)
{
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
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &ROTARY_TIMER)
	{
		/** polling rotary key */
		rotary_key_scan_poll();
	}
}
