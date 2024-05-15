#include <stdio.h>
#include <stdint.h>
#include "main_task.h"
#include "app_config.h"
#include "key/key.h"
#include "timeout.h"
#include "app_event_message.h"
#include "sys_app.h"
#include "led_indicator/led_animation.h"
#include "led_indicator.h"
#include "potentiometer/potentiometer.h"
#include "com_iface.h"

extern void user_led_init(void);
extern void user_led_run(void);

void adc_irq_callback(__uint32_t value);

EventContext msgSend;
uint8_t mainTaskState = TASK_STATE_IDLE;
uint8_t subTaskState[2] = {TASK_STATE_IDLE};
enum 
{
    SUBT_BTBROADCAST = 0,
    /**reserved*/
};

uint8_t subTaskNetworkState = TASK_STATE_IDLE;

/**
 * timer used to validity check connection
 * is it actually disconnected or just temporary
 * lost connection 
*/
TIMER tmrWaitConn;

TIMER tmrIdle;
TIMER tmrFsVolumeChange;
TIMER tmrFactoryTimeout;    // timeout if factory reset fail, it will reset flag
                            // so user can press factory reset again
TIMER tmrCheckStandbySetupMode;

/** this timer used to tricky volume feedback from VX module
 * after increase or decrease volume from local key, VX module wiil reply pre step 
 * so it will caused repetition effect
*/
TIMER tmrVolumeSync;

/*** time used to waiting FS module stable when change function
 * if user press button mode too fast its caused the FS module not be able processed
 * key command bluetoothe and key command spotify
 * FS module take sometime to ready in switch mode
*/
TIMER tmrWaitChangeMode;
/***this timeout timer used for blocking current  function reply
 * from VX module until current function reply from VX stable (latest)
*/
TIMER tmrBlockingFunctionReplyVx;


/** timeout used for auto broadcast mode */
TIMER tmrAutoBroadcast;
TIMER tmrSimMaster, tmrSimSlave;
#define TIMEOUT_BROADCAST_MASTER    6000
#define TIMEOUT_BROADCAST_SLAVE     6000

/** this timer used to set timeout in broadcast slave mode 
 * and not trigger the volume sync
*/
TIMER tmrVolSyncTimeout;


TIMER tmrBooting;
TIMER tmrSimBt;
TIMER tmrSimWifi;
TIMER tmrTestingUart;

uint8_t txBuff[20];

Potentiometer_t potensio = {
    .adc_handler = &hadc,
    .poll_read = potentiometer_adc_poll_read,
    .irq_callback = adc_irq_callback,
};

#define SET_ANIMATION_PROPERTY(s, _blink_speed, _bright_static, _color, _center_color, _flag_blink) \
		s.blink_speed = _blink_speed,       \
		s.bright_static = _bright_static,   \
        s.color = _color,                   \
        s.center_color = (_center_color!=NULL) ? _center_color : _color,     \
        s.flag_blink = _flag_blink

/** makro to whitelist message in broadacast mode slave 
 * (ev->eventId != MSG_BT_BROADCAST) && (system_config.bt_broadcast.role == BT_BROADCAST_ON_ROLE_SLAVE)
*/
#define IS_MSG_BLOCKED_ON_BT_BROADCAST_SLAVE(msg) \
        ( (system_config.bt_broadcast.role == BT_BROADCAST_ON_ROLE_SLAVE) && \
            (msg != MSG_BT_BROADCAST && \
             msg != MSG_BT_BROADCAST_SYNC_VOL && \
             msg != MSG_SPEAKER_MODE))


static void task_state_control(uint8_t *state_id, uint8_t state)
{
    *state_id = state;
}

static uint8_t get_next_mode(uint8_t cur_mode)
{

}

static uint8_t convert_fs_mode(uint8_t fsmode)
{

}

/**
 * @brief do mode change, if task state is paused (speaker role = slave) --> ignore
 * 
 * @param state_id taskState
 * @param ev event ID
 */
static void do_change_task(uint8_t *state_id, EventContext *ev)
{
    
}


/***
 * @brief   used to check is current mode is standby
 *          if current mode is standby and in setup mode so it must
 *          be wake up to trigger frontier to show SSID
 *          this function may executed in RUN_STATE so it use timer protection so 
 *          it will not trigger key command standby repeatedly
*/
static void do_check_standby_and_setup_mode(void)
{

}

enum
{
    /** state from internal system*/
    BT_NO_ACK = 0,
    /** state from bt broadcaster, role wait for reply */
    BT_ACK,
};

/***
 * @brief get broadcast role
 * @param   ack: is ack (wait reply from bt broadcaster)
 * @return  bt role
*/
static uint8_t get_broadcast_role(/*void*/ uint8_t ack)
{

}

/***
 * @brief get CSB device connection status
 * 
*/
static uint8_t get_broadcast_device_connection_status(void)
{
#if (CONFIG_BT_CSB_NO_DEVICE_ACK)
    return 0;
#else
    return (uartcom.device_connect_status);
#endif
}
/**
 * @brief   reset device connection status variable before 
 *          send command bt broadcast role
*/
static void reset_broadcast_device_connection_status(void)
{

}


void adc_irq_callback(__uint32_t value)
{
    static __uint16_t i = 0;

    if (++i > 1000) {
        i = 0;
        potensio.value[0] = value;
    }
}

/**
 * subtask for bluetooth broadcast process handle
 * flow:
 * user press buttton (short pressed) to determine role of bt broadcast
 * rolling state from 0: off, 1: master, 2: slave
 * 
*/
static void subtask_bt_broadcast(SystemConfig_t *sc)
{
    static uint8_t tx_data = 0;
    static uint8_t pre_broadcast_role = 0;  /** this used to saved previous bt broadcast role */

    switch(subTaskState[SUBT_BTBROADCAST])
    {
        case TASK_STATE_IDLE:
            /***
             * wait if any change on broadcast status ON/OFF
            */
            if (sc->bt_broadcast.update_state)
            {
                set_visual_mode(LED_EVENT_DYNAMIC_COLOR);
                sc->bt_broadcast.role = (sc->bt_broadcast.role + 1) % BT_BROADCAST_ROLE_SUM;
                dispProp.flag_blink = 1;
                subTaskState[SUBT_BTBROADCAST] = TASK_STATE_INIT;

                SET_FLAG( FLAG_CSB_SCANNING_STATUS );

                switch(sc->bt_broadcast.role)
                {
                    default: 
                        sc->bt_broadcast.update_state = 0;
                        tx_data = BT_BROADCAST_OFF_ROLE_STANDALONE;
                        subTaskState[SUBT_BTBROADCAST] = TASK_STATE_STOP;
                        CLR_FLAG( FLAG_CSB_SCANNING_STATUS );
                        break;

                    case BT_BROADCAST_ON_ROLE_MASTER:
                        TimeoutSet(&tmrAutoBroadcast, TIMEOUT_BROADCAST_MASTER);
                        TimeoutSet(&tmrSimMaster, 2000); // simulation
                        tx_data = BT_BROADCAST_ON_ROLE_MASTER;
                        dispProp.center_color = LED_COLOR_RED;


                        break;
                    case BT_BROADCAST_ON_ROLE_SLAVE:
                        TimeoutSet(&tmrAutoBroadcast, TIMEOUT_BROADCAST_SLAVE);
                        TimeoutSet(&tmrSimSlave, 2000); // simulation
                        tx_data = BT_BROADCAST_ON_ROLE_SLAVE;
                        dispProp.center_color = LED_COLOR_YELLOW;

                        /***
                         * @note: this used to trick if before set to slave mode and volume higher 
                         * it need to minimize volume first
                        */
                        sc->pre_system_volume = sc->system_volume;
                        sc->system_volume = 0;
                        TimeoutSet(&tmrVolSyncTimeout, 2000);
                        //CLR_FLAG( FLAG_CSB_SYNC_VOL_TIMEOUT );
                        break;
                }

                /** reset data connection status before read from uart comm BT broadcaster*/
                //uartcom.device_connect_status = 0;      // reset to 0
                reset_broadcast_device_connection_status();


            }
            break;

        case TASK_STATE_INIT:

            break;

        case TASK_STATE_RUN:

            break;

        case TASK_STATE_PAUSE:

            break;

        case TASK_STATE_STOP:

            break;

        default: 
            break;
    }

}



/**
 * @brief common task using on several task handler
 * @note    
*/
static void task_common(EventContext *ev)
{
    static uint8_t tx_data = 0;
    static uint16_t temp = 0;
    static uint8_t __data[2];

    if (ev->eventId != MSG_NONE)
    {

#if (CONFIG_BT_ROLE_SLAVE_BLOCKING)
    if ( IS_MSG_BLOCKED_ON_BT_BROADCAST_SLAVE(ev->eventId) )
        return;
#endif /*CONFIG_BT_ROLE_SLAVE_BLOCKING*/

    if ( (system_config.current_function == SYS_MODE_BOOTING && ev->eventId != MSG_FACTORY_RESET) && 
            system_config.venicex_state != VENICEX_STATE_READY)
        return;

        switch(ev->eventId)
        {
            /** volume up event id */
            case MSG_VOL_UP:
                if (system_config.system_volume < SYS_VOL_MAX)
                {
                    system_config.system_volume++;
                }

                system_app_nvm_save_trig();
                set_vol_display(system_config.system_volume);

                txBuff[0] = system_config.system_volume<<8;
                txBuff[1] = system_config.system_volume & 0xff;

                /** send command */
                com_iface_send_data(FUNCTION_CODE_READ, ADDR_REG_VOLUME, txBuff, 2);


                if (system_config.blink_periode < 1000) {
                    system_config.blink_periode += 50;
                }
                break;

            /** volume down event id */
            case MSG_VOL_DW:
                if (system_config.system_volume > 0)
                {
                    system_config.system_volume--;
                }

                txBuff[0] = system_config.system_volume<<8;
                txBuff[1] = system_config.system_volume & 0xff;

                /** send command */
                com_iface_send_data(FUNCTION_CODE_READ, ADDR_REG_VOLUME, txBuff, 2);

                if (system_config.blink_periode > 0) {
                    system_config.blink_periode -= 50;
                }

                system_app_nvm_save_trig();
                set_vol_display(system_config.system_volume);
                
                break;
            
            /** set to direct value */
            case MSG_BT_BROADCAST_SYNC_VOL: /**  message from bt broadcast sync */
            case MSG_FS_VOL_SET: /** message source from FS uart event */
                set_vol_display(system_config.system_volume);
                
                break;

            case MSG_PLAY_PAUSE:

                break;

            case MSG_BT_BROADCAST:

                break;

            case MSG_PRE:

                break;

            case MSG_NEXT:

                break;

            case MSG_SPEAKER_MODE:
                system_config.speaker_mode = (system_config.speaker_mode + 1) % SPEAKER_MODE_SUM;
                set_speaker_mode_display(&system_config);

                break;

            case MSG_MODE:
                CLR_FLAG( FLAG_IS_MODE_FROM_FS );
                if (mainTaskState == TASK_STATE_RUN) {
                    mainTaskState = TASK_STATE_STOP;
                }
                break;

#if (CONFIG_ENABLE_STANDBY)
            case MSG_POWER:

                break;
#endif
            /** factory reset event id */
            case MSG_FACTORY_RESET:
                /** ignore if any re trigger from factory reset */
                // if (system_config.venicex_state == VENICEX_STATE_FACTORY_START)
                //     return;

                // ignore if there are any factory reset retrigger
                if ( (/*GET_FLAG( FLAG_FS_FACTORY_RESET_START ) && */!IsTimeout(&tmrFactoryTimeout)) 
                     || system_config.current_function == SYS_MODE_FACTORY_RESET ) 
                    return;

                SET_FLAG( FLAG_FS_FACTORY_RESET_START );
                TimeoutSet(&tmrFactoryTimeout, 5000);
                break;

            default: break;
        }
    }

    /** saving to nvm memory handler (eeprom) */
    system_app_nvm_saving_process();

    subtask_bt_broadcast(&system_config);

    if (potensio.value[0] != potensio.value[1]) {
        __data[0] = (uint8_t)(potensio.value[0] >> 8);
        __data[1] = (uint8_t)(potensio.value[0] & 0xff);
        com_iface_send_data(FUNCTION_CODE_READ, ADDR_REG_ADC1_VALUE, (uint8_t*)__data, 2);
        potensio.value[1] = potensio.value[0];
    }


    if ( GET_FLAG( FLAG_FS_FACTORY_RESET_START ) )
    {

    }

#if (CONFIG_POWERAMP_AUTORECOVERY)

#endif

}

void task_network_configuration(void *arg)
{
    /** used for save previous wifi state
     * if there is change from venice-x command
     * so this variable used for indicator (led) update
    */
    static uint8_t pre_wifi_state = 0xf;

    EventContext *ev = (EventContext *)(arg);

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:

            break;

        case TASK_STATE_RUN:

            break;

        case TASK_STATE_WAIT:
            /** reserved */
            break;

        case TASK_STATE_PAUSE:

            break;

        case TASK_STATE_STOP:
            mainTaskState = TASK_STATE_IDLE;
            break;

        default: 
            break;
    }

    task_common(ev);

}

void task_idle(void *arg)
{
    
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            TimeoutSet(&tmrIdle, 1000);
            set_visual_mode(LED_EVENT_ERROR);
            SET_ANIMATION_PROPERTY(dispProp, 
                                    NULL, 
                                    NULL,
                                    LED_COLOR_RED,
                                    NULL, 
                                    0); /** ignore flag, event error force to blink */
            break;

        case TASK_STATE_RUN:
            if (IsTimeout(&tmrIdle))
            {
                mainTaskState = TASK_STATE_STOP;
                /** after idle, go to default function */
                system_config.current_function = CONFIG_DEFAULT_FUNCTION;
            }
            break;

        case TASK_STATE_PAUSE:
            /** reserved */
            break;

        case TASK_STATE_STOP:
            mainTaskState = TASK_STATE_IDLE;
            break;

        default: 
            break;
    }
}


/**
 * @brief task for factory reset
 * 
 * reset some parameter
 * - set default function to CONFIG_DEFAULT_FUNCTION
 * - set default system volume to DEFAULT_SYS_VOL
 *
 * send command factory reset to other module (Venice-X)     
 * note:
 * - if  CONFIG_ENABLE_NVM not enabled, so NVM use from VX
 *   VX value after factory:
 *   - volume = 8
*/
static void task_factory(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            set_visual_mode(LED_EVENT_FACTORY_RESET);

#if (CONFIG_ENABLE_NVM)
            /** erase eeprom data */
            if (nvm_handler.erase(sizeof(nvmParam)) < 0)
            {
                /** do error handler */
            } 
#endif
            mainTaskState = TASK_STATE_RUN;
            break;

        case TASK_STATE_RUN:

            break;

        case TASK_STATE_PAUSE:
            /* reserved for temporary*/
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
            break;
        default: break;
    }

}

/**
 * @brief   task standby
*/
void task_standby(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    /** ignore all message except POWER */
    if (ev->eventId != MSG_POWER && ev->eventId != MSG_NONE)
    {
        return;
    }

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;

            break;

        case TASK_STATE_INIT:
            if (system_config.venicex_state == VENICEX_STATE_READY)
            {
                mainTaskState = TASK_STATE_RUN;
                set_visual_mode(LED_EVENT_DYNAMIC_COLOR);
                SET_ANIMATION_PROPERTY(dispProp, 
                                    15, 
                                    60,
                                    LED_COLOR_WHITE,
                                    NULL, 0);
            }

            break;

        case TASK_STATE_RUN:
            if (ev->eventId == MSG_POWER)
            {
                mainTaskState = TASK_STATE_STOP;
            }
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
    


            break;

        default: break;
    }
    
}

/***
 * @brief   task for booting process
 *          waiting for other module (Venice X) in ready state 
 * @param   ev: event message 
*/
static void task_booting(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            led_indicator_set_in_mode(SYS_MODE_BOOTING, NULL);
            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            /** set indicator for booting process */
            set_visual_mode(LED_EVENT_BOOTING);
            system_config.venicex_state = VENICEX_STATE_BOOTING_PROCESS;
            TimeoutSet(&tmrBooting, 2000);
            break;

        case TASK_STATE_RUN:
            if ( IsTimeout(&tmrBooting) )
            {
                mainTaskState = TASK_STATE_STOP;
            }
            break;

        case TASK_STATE_PAUSE:
            /** reserved */
            break;

        case TASK_STATE_STOP:
            system_config.current_function = CONFIG_DEFAULT_FUNCTION;
            mainTaskState = TASK_STATE_IDLE;
            break;

        default: break;
    }

    task_common(ev);
}


void task_bluetooth_a2dp(void *arg)
{
    /** use for save previous bt state
     * if there is change from venice-x command
     * so this variable used for indicator (led) update
    */
    static uint8_t pre_bt_state = 0xf;

    EventContext *ev = (EventContext *)(arg);

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            mainTaskState = TASK_STATE_INIT;  
            TimeoutSet(&tmrSimBt, 4000);
            system_config.bt_evt = BT_EVENT_PAIRING;  
            led_indicator_set_in_mode(SYS_MODE_BT_A2DP, &system_config.bt_evt);
            /** set current state to nvm */
            system_app_nvm_save_trig();
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            break;

        case TASK_STATE_RUN:
            if (IsTimeout(&tmrSimBt)) {
                system_config.bt_evt = BT_EVENT_CONNECTED;
                led_indicator_set_in_mode(SYS_MODE_BT_A2DP, &system_config.bt_evt);
            }
            break;

        case TASK_STATE_PAUSE:
            /** resume task*/
            break;

        case TASK_STATE_STOP:
            /** do deinit task */
            mainTaskState = TASK_STATE_IDLE;
            system_config.current_function = SYS_MODE_SPOTIFY_CONNECT;
            break;
        default: break;
    }

    task_common(ev);
}


/**             
 * @brief do spotify task 
*/
void task_spotify_connect(void *arg)
{
    EventContext *ev = (EventContext *)(arg);
    uint8_t tx_data[10];

    do_change_task(&mainTaskState, ev);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:
            TimeoutSet(&tmrSimWifi, 3000);
            system_config.wifi_evt = WIFI_EVENT_CONNECTING;
            led_indicator_set_in_mode(SYS_MODE_SPOTIFY_CONNECT, &system_config.wifi_evt);

            mainTaskState = TASK_STATE_INIT;
            break;

        case TASK_STATE_INIT:
            mainTaskState = TASK_STATE_RUN;
            break;

        case TASK_STATE_RUN:
            if ( IsTimeout(&tmrSimWifi))
            {
                system_config.wifi_evt = WIFI_EVENT_CONNECTED;
                led_indicator_set_in_mode(SYS_MODE_SPOTIFY_CONNECT, &system_config.wifi_evt);
            }

            /*** adc using polling */
            if ( IsTimeout(&tmrTestingUart) ) {
                TimeoutSet(&tmrTestingUart, 250);

                //potensio.adc_value;
                tx_data[0] = 0x00;
                tx_data[1] += 0x01;
                //uart_com_send_data(0x03, 0x0000, tx_data, 2);
                //com_iface_send_data(FUNCTION_CODE_READ, ADDR_REG_VOLUME, txBuff, 2);
                
            }
            break;

        case TASK_STATE_PAUSE:
            /** pause task */
            break;

        case TASK_STATE_STOP:
            mainTaskState = TASK_STATE_IDLE;
            system_config.current_function = SYS_MODE_BT_A2DP;
            break;
        default: break;
    }

    task_common(ev);

}

/***
 * @brief   task for power amp recovery 
 * @param   
*/
void task_power_amp_autorecover(void *arg)
{
    EventContext *ev = (EventContext *)(arg);

    switch(mainTaskState)
    {
        case TASK_STATE_IDLE:

            break;

        case TASK_STATE_INIT:

            break;

        case TASK_STATE_RUN:

            break;

        case TASK_STATE_PAUSE:
            /** reserved */
            break;

        case TASK_STATE_STOP:

            break;

        default: 
            break;
    }

    task_common(ev);
}

/** State Machine 
 *  @note run in main_looping
*/
void (*MainTaskRunState[SYS_MODE_NUM])(void *arg) = {
    task_idle,
    task_standby,
    task_booting,
    task_bluetooth_a2dp,
    task_spotify_connect,
    task_factory,
    task_network_configuration,
    task_power_amp_autorecover,
};
/**********************************************************/


/**
 * @brief init main task 
*/
void main_task_init(void)
{
    
    system_app_init();

    key_init();

    led_animation_init();

    user_led_init();

    led_indicator_init();

    com_iface_init(USART1);

}

void main_task_run(void *arg)
{
    
    /** key scanning process */
    msgSend.eventId = key_scan();
    

    /** led user to indicate activity */
    user_led_run();

    led_indicator_run(NULL);

    com_iface_polling_data();

    /** handler led animation */
    //led_animation_handler(); 

    /** running state **/
    (*MainTaskRunState[system_config.current_function])((void*)&msgSend);
}


