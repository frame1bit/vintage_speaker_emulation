#ifndef CAPSENSE_H
#define CAPSENSE_H

#include <stdint.h>

#define 	CAP_KEY_EMPTY               0xFF

typedef enum _AdcKeyType
{
	CAP_KEY_UNKOWN_TYPE = 0,
	CAP_KEY_PRESSED,
	CAP_KEY_RELEASED,
	CAP_KEY_LONG_PRESSED,
	CAP_KEY_LONG_PRESS_HOLD,
	CAP_KEY_LONG_RELEASED,
}AdcKeyType;

typedef struct _CapKeyMsg
{
    uint16_t index;
    uint16_t type;
}CapKeyMsg;


/** capsense mapping value */
#define	CENTERPIECE_STANDBY_SOURCE_VAL	0x09
#define	CENTERPIECE_VOL_MIN_VAL			0x11
#define	CENTERPIECE_PLAY_PAUSE_VAL		0x21
#define	CENTERPIECE_VOL_PLUS_VAL		0x81
#define	CENTERPIECE_BROADCAST_SPK_MODE	0x41

//waktu cap key menyala dalam ms JH 04 04 22 
#define CFG_CAP_OFF_TIMEOUT             2900 

/** prototype function */
int32_t capsense_init(void);
void capsense_running_init_state(void);
CapKeyMsg capsense_scan(void);
void capsense_process_pwm(void);
void capsense_pwm_off(void);

#endif  /*CAPSENSE_H*/
