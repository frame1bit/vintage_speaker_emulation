#include "capsense.h"
#include "timeout.h"
#include "iqs333.h"

extern I2C_HandleTypeDef hi2c1;

/** timeout value */
#define CAP_KEY_SCAN_TIME		20
#define CAP_KEY_JITTER_TIME		30
#define CAP_KEY_CP_TIME			1000
#define CAP_KEY_CPH_TIME		200

/* capsense key state */
typedef enum _CAP_KEY_STATE
{
    CAP_KEY_STATE_IDLE,
    CAP_KEY_STATE_JITTER,
    CAP_KEY_STATE_PRESS_DOWN,
    CAP_KEY_STATE_CP
} CAP_KEY_STATE;

TIMER tmr_cap_key_wait;
TIMER tmr_cap_key_scan;
CAP_KEY_STATE cap_key_state;
static uint8_t pre_key_index = CAP_KEY_EMPTY;

TIMER iqs_pwm_timeout;
TIMER iqs_pwm_timeout2; 
TIMER iqs_standby_timeout;

uint8_t is_pwm_change, is_pwm_change2;


/**
 * @brief init capsense key
*/
int32_t capsense_init(void)
{
    cap_key_state = CAP_KEY_STATE_IDLE;
    pre_key_index = CAP_KEY_EMPTY;
    TimeoutSet(&tmr_cap_key_scan, 0);

    TimeoutSet(&iqs_pwm_timeout, 200);
    //TimeoutSet(&iqs_pwm_timeout2, 200);

    return (iqs333_init(&hi2c1));
}

/**
 * @brief on running init state machine, used on iqs333
 *          reinit, reconfigure purpose
*/
void capsense_running_init_state(void)
{
    iqs333_init_state();
}


/**
 * @brief check is pwm OFF
*/
static uint8_t is_pwm_off(void)
{
    uint8_t i = 0;
    
    for (i = 0; i < 8; i += 1) {
        if (iqs333_get_pwmbuff(i) != PWM_OFF)
            return 0;
    }

    return 1;
}

/***
 * @brief   set all pwm off
*/
static void set_all_pwm_off(void)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i += 1) {
        iqs333_set_pwmbuff(i, PWM_OFF);
    }
}

/** @brief set pwm index to ON 
 * 
*/
static void set_pwm_index_on(uint8_t index)
{
    set_all_pwm_off();

    /**
     * [b7-b5] 111 = PWM CMP4:0 incremented and stops at value in PWM_LIM
     * [b4-b0] 1111 = Duty Cycle
     * Set to ON
    */
    iqs333_set_pwmbuff(index, 0x7F);
} 

void capsense_process_pwm(void)
{
    if (iqs333_get_state() != IQS_STATE_IDLE)
        return;

    /** write buffered pwm ------------------
     * TODO: check routine cycle !!
    */
    if (is_pwm_change)
    {
        if (IsTimeout(&iqs_pwm_timeout))
        {
            iqs333_write_pwmbuff();
            TimeoutSet(&iqs_pwm_timeout, 200);
            is_pwm_change = 0;
        }
    }

    if (is_pwm_change2)
    {
        if (IsTimeout(&iqs_pwm_timeout2))
        {
            iqs333_write_pwmbuff();
            TimeoutSet(&iqs_pwm_timeout2, 200);
            is_pwm_change2 = 0;
        }
    }
    
}

void capsense_pwm_off(void)
{
    if (iqs333_get_state() != IQS_STATE_IDLE)
        return;

    if (is_pwm_change2)
    {
        if (IsTimeout(&iqs_pwm_timeout2))
        {
            iqs333_write_pwmbuff();
            TimeoutSet(&iqs_pwm_timeout2, 200);
            is_pwm_change2 = 0;
        }
    }
}

/**
 * @brief   capsense channel mapping
 * @note    channel (deprecated)
*/
static uint8_t capsense_get_touch_channel(uint8_t channel)
{
    uint8_t touch_data = 0;
    uint8_t ret = 0;

    if (iqs333_get_state() != IQS_STATE_IDLE)
        return CAP_KEY_EMPTY;

    touch_data = iqs333_read_touch();
    
    switch (touch_data)
    {
    case 0x00:
        /** turn off pwm if any pwm is on*/
        if (is_pwm_off() == 0)
        {
            if ( !is_pwm_change )
            {
                set_all_pwm_off();
                TimeoutSet(&iqs_pwm_timeout2, 1000);
                /** set pwm to off based on timeout */
                is_pwm_change2 = 1;
                
            }
        }

        /** TODO: check current mode, if standby --> set LED to off */
        if (IsTimeout(&iqs_standby_timeout) && 0)
        {
            LED_CAPS_OFF();
        }
        break;
    
    /** proximity event */
    case 0x01:
        if (0 /**TODO: standby*/ && iqs333_get_state() == IQS_STATE_IDLE)
        {
            LED_CAPS_ON();
            TimeoutSet(&iqs_standby_timeout, 5000);
        }
        break;

    case CENTERPIECE_STANDBY_SOURCE_VAL:
        set_pwm_index_on(6);
        is_pwm_change = 1;
        ret = 1;
        break;

    case CENTERPIECE_PLAY_PAUSE_VAL:
        set_pwm_index_on(2);
        is_pwm_change = 1;
        ret = 2;
        break;

    case CENTERPIECE_VOL_MIN_VAL:
        set_pwm_index_on(3);
        is_pwm_change = 1;
        ret = 3;
        break;

    case CENTERPIECE_VOL_PLUS_VAL:
        set_pwm_index_on(4);
        is_pwm_change = 1;
        ret = 4;
        break;
    
    case CENTERPIECE_BROADCAST_SPK_MODE:
        set_pwm_index_on(5);
        is_pwm_change = 1;
        ret = 5;
        break;

    default: ret = 0; break;
    }

    if (ret != 0)
    { 
        return ret;
    }

    return CAP_KEY_EMPTY;
}

/**
 * @brief   capsense scan
 * @param   none
 * @return  capsense key msg
*/
CapKeyMsg capsense_scan(void)
{
    uint8_t key_index = CAP_KEY_EMPTY;
    CapKeyMsg Msg = {CAP_KEY_EMPTY, CAP_KEY_UNKOWN_TYPE};

    if (!IsTimeout(&tmr_cap_key_scan))
    {
        return Msg;
    }
    TimeoutSet(&tmr_cap_key_scan, CAP_KEY_SCAN_TIME);

    key_index = capsense_get_touch_channel(0);

    switch(cap_key_state)
    {
        case CAP_KEY_STATE_IDLE:
            if (key_index == CAP_KEY_EMPTY) return Msg;
            pre_key_index = key_index;
            TimeoutSet(&tmr_cap_key_wait, CAP_KEY_JITTER_TIME);
            cap_key_state = CAP_KEY_STATE_JITTER;

        case CAP_KEY_STATE_JITTER:
            if (pre_key_index != key_index)
            {
                pre_key_index = CAP_KEY_EMPTY;
                cap_key_state = CAP_KEY_STATE_IDLE;
            }
            else if (IsTimeout(&tmr_cap_key_wait))
            {
                TimeoutSet(&tmr_cap_key_wait, CAP_KEY_CP_TIME);
                cap_key_state = CAP_KEY_STATE_PRESS_DOWN;
                Msg.index = pre_key_index;
                Msg.type = CAP_KEY_PRESSED;
            }
            break;

        case CAP_KEY_STATE_PRESS_DOWN:
            if (pre_key_index != key_index)
            {
                cap_key_state = CAP_KEY_STATE_IDLE;
                Msg.index = pre_key_index;
                Msg.type = CAP_KEY_RELEASED;
                pre_key_index = CAP_KEY_EMPTY;
            }
            else if (IsTimeout(&tmr_cap_key_wait))
            {
                TimeoutSet(&tmr_cap_key_wait, CAP_KEY_CPH_TIME);
                cap_key_state = CAP_KEY_STATE_CP;
                Msg.index = pre_key_index;
                Msg.type = CAP_KEY_LONG_PRESSED;
            }
            break;

        case CAP_KEY_STATE_CP:
            if (pre_key_index != key_index)
            {
                cap_key_state = CAP_KEY_STATE_IDLE;
                Msg.index = pre_key_index;
                Msg.type = CAP_KEY_LONG_RELEASED;
                pre_key_index = CAP_KEY_EMPTY;
            }
            else if (IsTimeout(&tmr_cap_key_wait))
            {
                TimeoutSet(&tmr_cap_key_wait, CAP_KEY_CPH_TIME);
                Msg.index = pre_key_index;
                Msg.type = CAP_KEY_LONG_PRESS_HOLD;
            }
            break;

        default:
            cap_key_state = CAP_KEY_STATE_IDLE;
            pre_key_index = CAP_KEY_EMPTY;
            break;
    }

    return Msg;
}

