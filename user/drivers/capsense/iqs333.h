#ifndef IQS333_H
#define IQS333_H

/******************************************************************************
*                                                                             *
*                            Module specification                             *
*                                                                             *
*                                Copyright by                                 *
*                                                                             *
*                              Azoteq (Pty) Ltd                               *
*                          Republic of South Africa                           *
*                                                                             *
*                           Tel: +27(0)21 863 0033                            *
*                          E-mail: info@azoteq.com                            *
*                                                                             *
*******************************************************************************
Name             :  IQS333.h
Description      :  IQS333 Address Structure Map (see datasheet)
                 :  This Header file can be used to set the settings on the
                 :  IQS333 easily
                 :  The parameters are used IQS333_Button_Wheel_Settings(void)
Author           :  Alwino van der Merwe
Date             :  24/07/13
Revision         :  V1.3.1
*******************************************************************************/
#include "main.h"

#define CFG_CAP_KEY_EN

// void init_IQS333_I2C(void);
// void init_IQS333_I2C2(void);
// void IQS333_Init_State(void); 
// void IQS333_Init_State2(void);
// void IQS333_PWM_State(void);
// void IQS333_PWM_State_OFF(void);


/* i2c Slave Address */
#define IQS333_ADDR (0x64<<1) //shift left 1 bit JH 18 01 18 

/** capsense ready pin */
#define IQS333_RDY_PORT 0//RDY_CAPS_GPIO_Port
#define IQS333_RDY_PIN  0//RDY_CAPS_Pin

/* led capsense, used when some proximity event occur */

#define IQS333_LED_PORT LED_CAPS_GPIO_Port
#define IQS333_LED_PIN  LED_CAPS_Pin

#define	IQS333_CLR_PORT	CLR_CAPS_GPIO_Port
#define	IQS333_CLR_PIN	CLR_CAPS_Pin

#define LED_CAPS_ON()       //HAL_GPIO_WritePin(IQS333_LED_PORT, IQS333_LED_PIN, GPIO_PIN_RESET)
#define LED_CAPS_OFF()      //HAL_GPIO_WritePin(IQS333_LED_PORT, IQS333_LED_PIN, GPIO_PIN_SET)

/** MCLR Pin */
#define	CLR_CAPS_HI()		//HAL_GPIO_WritePin(IQS333_CLR_PORT, IQS333_CLR_PIN, GPIO_PIN_SET)
#define	CLR_CAPS_LO()		//HAL_GPIO_WritePin(IQS333_CLR_PORT, IQS333_CLR_PIN, GPIO_PIN_RESET)
/** end of pin definition for capsense */



// Communication Command / Address Structure on IQS333 - ie. Memory Map
#define	VERSION_INFO			0x00	// Product number can be read      : 2 bytes
#define FLAGS 					0x01	// System flags and events         : 1 byte
#define WHEEL_COORDS			0x02	// Wheel coordinates - 2 wheels    : 4 bytes
#define TOUCH_BYTES     		0x03	// Touch channels                  : 2 bytes
#define COUNTS                	0x04	// Count Values                    :20 bytes
#define LTA              		0x05	// LTA Values                      :20 bytes
#define MULTIPLIERS            	0x06	// Multipliers Values              :10 bytes
#define COMPENSATION            0x07	// Compensation Values (PCC)       :10 bytes
#define PROXSETTINGS           	0x08	// Prox Settings - Various         : 6 bytes
#define THRESHOLDS              0x09	// Threshold Values                :10 bytes
#define TIMINGS                 0x0A    // Timings                         : 5 bytes
#define	ATI_TARGETS				0x0B	// Targets for ATI                 : 2 bytes
#define PWM 					0x0C	// PWM Settings                    : 4 bytes
#define PWM_LIMIT				0x0D	// PWM Limits and speed            : 2 bytes
#define ACTIVE_CHANNELS     	0x0E 	// Active channels                 : 2 bytes
#define BUZZER           		0x0F	// Buzzer Output                   : 1 byte

//JH 19 01 18 
#define IQS_STATE_FLAGS 		0x01
#define IQS_STATE_CHANNELS 		0x02
#define IQS_STATE_TRESHOLD 		   3
#define IQS_STATE_ATI 			   4
#define IQS_STATE_MULTIPLIER 	   5
#define IQS_STATE_PROXSETTINGS 	   6
#define IQS_STATE_COMPENSATION 	   7
#define IQS_STATE_TIMINGS 	       8
#define IQS_STATE_PWM_SETING 	   9
#define IQS_STATE_PWM_SPEED	      10
#define IQS_STATE_REDO_ATI 		  11 //new state JH 22 01 18 
#define IQS_STATE_WAIT_ATI 		  12
#define IQS_STATE_IDLE 			  13
#define IQS_STATE_WAIT_READY 	  14

//#define IQS_STATE_TIMINGS 	       8
//#define IQS_STATE_REDO_ATI 		   9 //new state JH 22 01 18 
//#define IQS_STATE_WAIT_ATI 		  10
//#define IQS_STATE_IDLE 			  11
//#define IQS_STATE_WAIT_READY 	  12


#ifdef CFG_CAP_KEY_EN
extern uint8_t IQS_State; //JH 19 01 18 
extern uint8_t IQS_State2; //JH 19 01 18 

extern uint8_t fIQS_ChangePWM; //flag untuk ubah PWM JH 09 06 21 
#endif

/*
* IQS Status
*/
typedef enum _iqs_stat
{
	IQS_OK	= 0x00,
	IQS_COMM_ERROR,
	IQS_TIMEOUT,
	IQS_ARG_ERROR,
	IQS_NOT_READY,
	IQS_RESERVED,
	
}IQS_STATE_t;

enum
{
	PWM_OFF 	= 0x00,
	PWM_CONST	= 0x01,
	PWM_DEC0	= 0x04,
	PWM_DEC_LIM	= 0x05,
	PWM_INC100	= 0x06,
	PWM_INC_LIM	= 0x07,
};

/** function prototype */
int iqs333_init(I2C_HandleTypeDef *i2c);
void iqs333_init_state(void);
uint8_t iqs333_get_pwmbuff(uint8_t index);
void iqs333_set_pwmbuff(uint8_t index, uint8_t value);
uint8_t iqs333_read_touch(void);
uint8_t iqs333_get_state(void);
uint8_t iqs333_get_ready(void);
void iqs333_write_pwmbuff(void);

void iqs333_test_pwm(void);

#endif /*IQS333_H*/
