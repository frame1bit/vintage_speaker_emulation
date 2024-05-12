#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdio.h>

#define SOFTWARE_VERSION                (13)

/**this param used to determine used respons
 * 1: experiment respons, uses for big speaker (PAS)
 * 0: use autrans respons
*/
#define CONFIG_RESPONS_EXPERIMENT       (0)
/***
 * !!!! note !!!!
 * invert some key because diffuser from mechanic not suited / inverted
 * this param just used in audio fair , 9 november
 * after that this param will be removed 
*/
#define CONFIG_INVERTED_KEY_POSITION    (1)


#define delay_ms(x)                     HAL_Delay(x)

/**
 * number of WS2812 used 
*/
#define CONFIG_LED_NUMBER               (5)

/* debug option */
//#define DEBUG_UART

#ifdef DEBUG_UART
    /* printf definetion */
    #define DEBUG_MSG( fmt, ...)        \                                                                     
        printf( "[%08d] " fmt ,         \
        HAL_GetTick(), ##__VA_ARGS__)

	#define DBG( fmt, ...)        \
			printf(fmt , ##__VA_ARGS__)
#else
    #define DEBUG_MSG( FLAG, fmt, ... )
    #define DBG( fmt, ...)
#endif

/** use non volatile memory (external eeprom)
 * to save some parameter
 * 1: enable NVM
 * 0: disable NVM
 * if this option not enabled, so NVM use from VX module
 * after factory, default value are:
 * - Volume = 8
 * - 
*/
#define CONFIG_ENABLE_NVM                   (0)


/**
 * set function to STANDBY after power on
 * 0: disable
 * 1: enable
*/
#define CONFIG_ENABLE_STANDBY               (0)
#if (CONFIG_ENABLE_STANDBY)
/**
 * this config used to determine is stanby is depend on 
 * spotify status
 * if spotify status already logged, so after set booting,
 * it will direct to spotify function 
 * standby function just used after factory reset
*/
#define CONFIG_STANDBY_DEPEND_NETWORK_CONNECTION    (1)
#endif

#define CONFIG_DEFAULT_FUNCTION                     (SYS_MODE_SPOTIFY_CONNECT)

/***
 * config to determine error indicator when speaker cannot connect 
 * to wifi access point
 * 0: disable
 * 1: enable 
 * 
 * failed connect to access point caused by:
 * 1. wrong wifi passphrase
 * 2. access point out of range
*/
#define CONFIG_NETWORK_ERROR_INDICATOR_ENABLE           (1)

/**
 *  set max system volume
*/
#define	SYS_VOL_MAX	                        (30)

/***
 * default volume after factory reset 
*/
#define DEFAULT_SYS_VOL                     (15)

/***
 * use fix volume on Power Amp
 * volume control by venice-x on spotify connect app and advanced avrcp 
 * bluetooth volume-sync
 * 0: disable (power amp volume adjust)
 * 1: enable (use fix volume)
 * 
 * @note: venice-x have volume control, so consider of remove power amp volume control usage.
 * this will caused double volume control
*/
#define CONFIG_PA_FIX_VOLUME                (0)


/**
 * this value valid if CONFIG_PA_FIX_VOLUME is enabled
*/
#define POWER_AMP_VOLUME_LEVEL              (30)


/** config to use waiting timer to send key command bluetooth and
 * key command spotify
 * 1: enable
 * 0: disable
*/
#define CONFIG_WAIT_CHANGE_MODE_VENICEX     (1)
#define CONFIG_ROLLING_MODE                 (1)
/**
 * rotary encoder use
 * 1: enable
 * 0: disable
*/
#define CONFIG_ROTARY_ENCODER_ENABLE        (1)
#if (CONFIG_ROTARY_ENCODER_ENABLE)
#define CONFIG_ROTARY_SWITCH                (1)
#endif

/***
 * use capsense touch button
*/
#define CONFIG_CAPSENSE_TOUCH_ENABLE        (0)


/***
 * bluetooth broadcast behaviour
 * speaker with slave role, controll will be blocking
*/
#define CONFIG_BT_ROLE_SLAVE_BLOCKING       (1)


/**
 * config to determine behaviour of CSB 
 * if using no ack, state will force to specific role (master/slave)
 * 1: enable
 * 0: disable
*/
#define CONFIG_BT_CSB_NO_DEVICE_ACK         (1)

/* Power Amp Selection */
#define TAS5822M    0x00
#define TAS3251     0x01

#define CONFIG_POWER_AMP_SELECT             (TAS5822M)

/***
 * this parameter used to auto recover power amp (TAS5822)
 * when power amp in protect condition.
 * this protect condition indicated by pin fault pulled low
*/
#define CONFIG_POWERAMP_AUTORECOVERY       (1)


/***
 * this config determine CSB volume behaviour
 * if this config enable so slave system volume will overrided with master 
 * system volume
*/
#define CONFIG_CSB_VOLUME_OVERRIDED         (0)

/** 
 * volume master selection
*/
enum
{
    /** volume master using bluetooth broadcaster
     * using action MCU ATS2835P
    */
    MASTER_VOLUME_BT_BROADCASTER        = 0,
    /**
     * volume master using digital power amplifier
     * TAS5822M
    */
    MASTER_VOLUME_POWER_AMP,
};

#define CONFIG_MASTER_VOLUME            (MASTER_VOLUME_POWER_AMP)


#endif /* APP_CONFIG_H */
