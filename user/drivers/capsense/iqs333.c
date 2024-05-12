#include "iqs333.h"
#include "timeout.h"

#define IQS_DATA_BUFFER_SIZE    30
uint8_t data_buffer[IQS_DATA_BUFFER_SIZE]; // JH 17 01 18
uint8_t IQS_State;       // JH 19 01 18

#define IQS_PWM_BUFFER_SIZE 8
uint8_t pwm_buffer[IQS_PWM_BUFFER_SIZE] = {0};

#define PWM_MODE PWM_CONST


#ifdef CFG_CAP_REINIT_TIMER // JH 21 02 22
extern uint32_t CAP_REINIT_Timeout;
#endif

#ifdef CFG_CAP_KEY_EN

////////////////////////////////////////////////////////
/**
 * note:
 * 0: calibrated for centerpiece finishing mockup
 * 1: not calibrated yet
*/
#if 0
////////////////////////////////////////////////////////

/* Used to switch to Projected mode */
#define SYSTEM_FLAGS_VAL					0x00

/*
* Change the Multipliers (BASE value) of each channel
* Please note that these values are affected by the Auto ATI routine and should
* only be written in the case of a specific setup.  Alternatively Auto ATI
* should be manually called after writing these settings.
* This is applicable for both Multipliers and Compensation.
*/
// modif all value dari CT210 Tool JH 23 06 21
// re calibrate for tempered glass JH 28 06 21 
#define MULTIPLIERS_CH0						0x37
#define MULTIPLIERS_CH1						0x04
#define MULTIPLIERS_CH2						0x04
#define MULTIPLIERS_CH3						0x1E
#define MULTIPLIERS_CH4						0x1F
#define MULTIPLIERS_CH5						0x1E
#define MULTIPLIERS_CH6						0x1D
#define MULTIPLIERS_CH7						0x1B
#define MULTIPLIERS_CH8						0x00
#define MULTIPLIERS_CH9						0x00
/*
* Change the Compensation for each channel
* Please note that these values are affected by the Auto ATI routine and should
* only be written in the case of a specific setup.  Alternatively Auto ATI
* should be manually called after writing these settings.
* This is applicable for both Multipliers and Compensation.
*/
#define COMPENSATION_CH0					0x39
#define COMPENSATION_CH1					0x2A
#define COMPENSATION_CH2					0x2A
#define COMPENSATION_CH3					0x29
#define COMPENSATION_CH4					0x25
#define COMPENSATION_CH5					0x29
#define COMPENSATION_CH6					0x29
#define COMPENSATION_CH7					0x28
#define COMPENSATION_CH8					0x00
#define COMPENSATION_CH9					0x00

/* Change the Prox Settings or setup of the IQS333 */
#define PROXSETTINGS0_VAL					0x06//0x06 FULL ATI MODE, 0x46 PARTIAL ATI JH 17 06 21
#define PROXSETTINGS1_VAL					0x00
#define PROXSETTINGS2_VAL					0x00
#define PROXSETTINGS3_VAL					0x00
#define PROXSETTINGS4_VAL					0x07
#define PROXSETTINGS5_VAL					0x7F

////////////////////////////////////////////////
#define TOUCH_THRESHOLD_VAL                 0x05
////////////////////////////////////////////////

#define PROX_THRESHOLD						0x0A
#define TOUCH_THRESHOLD_CH1					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH2					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH3					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH4					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH5					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH6					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH7					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH8					TOUCH_THRESHOLD_VAL
#define TOUCH_THRESHOLD_CH9					TOUCH_THRESHOLD_VAL

/* Change the Timing settings */
#define FILTER_HALT							0x4F
#define POWER_MODE							0x00
#define TIMEOUT_PERIOD						0x10
#define CH0_ACF_BETA						0x02
#define CH0_9_ACF_BET1						0x02

/* Change ATI Target values  */
#define ATI_TARGET_CH0						0x80
#define ATI_TARGET_CH0_9					0x40

/* Change PWM settings  */
#define PWM_0								0x7F //jadiin 3F semua JH 24 05 21 
#define PWM_1								0x7F
#define PWM_2								0x7F
#define PWM_3								0x7F
#define PWM_4								0x7F
#define PWM_5								0x7F
#define PWM_6								0x7F
#define PWM_7								0x7F

#define PWM_0_OFF							0x00
#define PWM_1_OFF							0x00
#define PWM_2_OFF							0x00
#define PWM_3_OFF							0x00
#define PWM_4_OFF							0x00
#define PWM_5_OFF							0x00
#define PWM_6_OFF							0x00
#define PWM_7_OFF							0x00


/* Change PWM limits and Speed */
#define PWM_LIMITS							0x00
#define PWM_SPEED							0x00

/* Set Active Channels */
#define ACTIVE_CH0							0xF8
#define ACTIVE_CH1							0x00

/* Set Buzzer Output */
#define BUZZER_VAL							0x00
#else
/* Used to switch to Projected mode */
#define SYSTEM_FLAGS_VAL					0x00

/*
* Change the Multipliers (BASE value) of each channel
* Please note that these values are affected by the Auto ATI routine and should
* only be written in the case of a specific setup.  Alternatively Auto ATI
* should be manually called after writing these settings.
* This is applicable for both Multipliers and Compensation.
*/
#define MULTIPLIERS_CH0						0x38
#define MULTIPLIERS_CH1						0x04
#define MULTIPLIERS_CH2						0x04
#define MULTIPLIERS_CH3						0x1A
#define MULTIPLIERS_CH4						0x1C
#define MULTIPLIERS_CH5						0x1E
#define MULTIPLIERS_CH6						0x1A
#define MULTIPLIERS_CH7						0x1B
#define MULTIPLIERS_CH8						0xFF
#define MULTIPLIERS_CH9						0xFF

/*
* Change the Compensation for each channel
* Please note that these values are affected by the Auto ATI routine and should
* only be written in the case of a specific setup.  Alternatively Auto ATI
* should be manually called after writing these settings.
* This is applicable for both Multipliers and Compensation.
*/
#define COMPENSATION_CH0					0x39
#define COMPENSATION_CH1					0x29
#define COMPENSATION_CH2					0x29
#define COMPENSATION_CH3					0x2A
#define COMPENSATION_CH4					0x2C
#define COMPENSATION_CH5					0x29
#define COMPENSATION_CH6					0x30
#define COMPENSATION_CH7					0x30
#define COMPENSATION_CH8					0xFF
#define COMPENSATION_CH9					0xFF

/* Change the Prox Settings or setup of the IQS333 */
#define PROXSETTINGS0_VAL					0x06
#define PROXSETTINGS1_VAL					0x00
#define PROXSETTINGS2_VAL					0x00
#define PROXSETTINGS3_VAL					0x00
#define PROXSETTINGS4_VAL					0x07
#define PROXSETTINGS5_VAL					0x7F

#define CAPS_OFFSET                         4//2

/* Change the Thresholds for each channel */
#define PROX_THRESHOLD						0x04
#define TOUCH_THRESHOLD_CH1					(0x06 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH2					(0x06 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH3					(0x04 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH4					(0x02 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH5					(0x02 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH6					(0x03 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH7					(0x03 + CAPS_OFFSET)
#define TOUCH_THRESHOLD_CH8					0x64
#define TOUCH_THRESHOLD_CH9					0x64

/* Change the Timing settings */
#define FILTER_HALT							0x4F
#define POWER_MODE							0x00
#define TIMEOUT_PERIOD						0x10
#define CH0_ACF_BETA						0x02
#define CH0_9_ACF_BET1						0x02

/* Change ATI Target values  */
#define ATI_TARGET_CH0						0x80
#define ATI_TARGET_CH0_9					0x40

/* Change PWM settings  */
#define PWM_0								0x00
#define PWM_1								0x00
#define PWM_2								0x00
#define PWM_3								0x00
#define PWM_4								0x00
#define PWM_5								0x00
#define PWM_6								0x00
#define PWM_7								0x00

/* Change PWM limits and Speed */
#define PWM_LIMITS							0x00
#define PWM_SPEED							0x00

/* Set Active Channels */
#define ACTIVE_CH0							0xFF
#define ACTIVE_CH1							0x03

/* Set Buzzer Output */
#define BUZZER_VAL							0x00

#endif


/** instance for iqs333 I2C */
static I2C_HandleTypeDef *i2c_instance;


IQS_STATE_t iqs333_write_pwm(void);


#ifdef CAP_KEY_DUAL_CHANNEL
void *IQS333_I2cHandler2 = NULL;
IQS_STATE_t iqs333_write_pwm2(void);
#endif

/**
 * @brief get ready pin
 * @note    The MCU can simply wait for the ready
 *          line to go low know when a
 *          communication window is open. A
 *          communication window can be invoked by
 *          a handshake during event mode. The
 *          handshake is done by setting the ready
 *          line as an output, pulling it low for 10ms
 *          and then setting it to a floating input again.
 *          The IC will respond by pulling ready low
 *          from its side if the handshake was successful
*/
uint8_t iqs333_get_ready(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = IQS333_RDY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(IQS333_RDY_PORT, &GPIO_InitStruct);

    /** set to low on rdy pin */
    HAL_GPIO_WritePin(IQS333_RDY_PORT, IQS333_RDY_PIN, GPIO_PIN_RESET);

    /** set as input again */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(IQS333_RDY_PORT, &GPIO_InitStruct);
    HAL_Delay(10);

    return ((uint8_t) HAL_GPIO_ReadPin(IQS333_RDY_PORT, IQS333_RDY_PIN));
}


int iqs333_init(I2C_HandleTypeDef *i2c)
{
    i2c_instance = i2c;
    if (!i2c_instance)
    {
        return -1;
    }

    /** init state */
    IQS_State = IQS_STATE_FLAGS;

    // CLR_CAPS_LO();
    // delay_ms(2);
    // CLR_CAPS_HI();

    return 0;
}

/** @brief IQS333 I2C write function 
 * 
*/
static int iqs333_write(uint8_t reg, uint8_t *pData, uint8_t len)
{
    uint32_t timeout = 1000;

    while(timeout > 0 && iqs333_get_ready() !=0 )
    {
        timeout--;
    }

    return (HAL_I2C_Mem_Write(i2c_instance,
                                IQS333_ADDR,
                                reg,
                                I2C_MEMADD_SIZE_8BIT,
                                pData,
                                len,
                                0xffffffff));

}  

/**
 * @brief   IQS333 I2C read function
*/
static int iqs333_read(uint8_t reg, uint8_t *pData, uint8_t len)
{
    uint32_t timeout = 1000;

    
    while(timeout > 0 && iqs333_get_ready() !=0 )
    {
        timeout--;
    }

    return (HAL_I2C_Mem_Read(i2c_instance, 
                                IQS333_ADDR,
                                reg,
                                I2C_MEMADD_SIZE_8BIT,
                                pData,
                                len,
                                0xffffffff));

}


/**
 * @brief on running state
*/
void iqs333_init_state(void)
{
    /** check ready pin first */
    // if ( iqs333_get_ready() != 0)
    // {
    //     return;
    // }

    switch (IQS_State)
    {
    case IQS_STATE_FLAGS:
        /* Switch the IQS333 to self sensing mode */
        data_buffer[0] = SYSTEM_FLAGS_VAL;

        if (iqs333_write(FLAGS, data_buffer, 1) == 0)
        {
            IQS_State = IQS_STATE_CHANNELS;
        }
        else
        {
            IQS_State = IQS_STATE_FLAGS;
        }
        break;

    case IQS_STATE_CHANNELS:
        /* Set active channels  */
        data_buffer[0] = ACTIVE_CH0;
        data_buffer[1] = ACTIVE_CH1;

        if (iqs333_write(ACTIVE_CHANNELS, data_buffer, 2) == 0)
        {
            IQS_State = IQS_STATE_TRESHOLD;
        }
        else
        {
            IQS_State = IQS_STATE_CHANNELS;
        }
        break;

    case IQS_STATE_TRESHOLD:
        /* Setup Touch and Prox thresholds for each channel */
        data_buffer[0] = PROX_THRESHOLD;
        data_buffer[1] = TOUCH_THRESHOLD_CH1;
        data_buffer[2] = TOUCH_THRESHOLD_CH2;
        data_buffer[3] = TOUCH_THRESHOLD_CH3;
        data_buffer[4] = TOUCH_THRESHOLD_CH4;
        data_buffer[5] = TOUCH_THRESHOLD_CH5;
        data_buffer[6] = TOUCH_THRESHOLD_CH6;
        data_buffer[7] = TOUCH_THRESHOLD_CH7;
        data_buffer[8] = TOUCH_THRESHOLD_CH8;
        data_buffer[9] = TOUCH_THRESHOLD_CH9;

        if (iqs333_write(THRESHOLDS, data_buffer, 10) == 0)
        {
            IQS_State = IQS_STATE_ATI;
        }
        else
        {
            IQS_State = IQS_STATE_TRESHOLD;
        }
        break;

    case IQS_STATE_ATI:
        /* Set the  ATI Targets (Target Counts) */
        data_buffer[0] = ATI_TARGET_CH0;
        data_buffer[1] = ATI_TARGET_CH0_9;

        if (iqs333_write(ATI_TARGETS, data_buffer, 2) == 0)
        {
            IQS_State = IQS_STATE_MULTIPLIER;
        }
        else
        {
            IQS_State = IQS_STATE_ATI;
        }
        break;

    case IQS_STATE_MULTIPLIER:
        /* Set the BASE value for each channel */
        data_buffer[0] = MULTIPLIERS_CH0;
        data_buffer[1] = MULTIPLIERS_CH1;
        data_buffer[2] = MULTIPLIERS_CH2;
        data_buffer[3] = MULTIPLIERS_CH3;
        data_buffer[4] = MULTIPLIERS_CH4;
        data_buffer[5] = MULTIPLIERS_CH5;
        data_buffer[6] = MULTIPLIERS_CH6;
        data_buffer[7] = MULTIPLIERS_CH7;
        data_buffer[8] = MULTIPLIERS_CH8;
        data_buffer[9] = MULTIPLIERS_CH9;

        if (iqs333_write(MULTIPLIERS, data_buffer, 10) == 0)
        {
            IQS_State = IQS_STATE_PROXSETTINGS;
        }
        else
        {
            IQS_State = IQS_STATE_MULTIPLIER;
        }
        break;

    case IQS_STATE_PROXSETTINGS:

        /* Setup Prox Settings*/
        data_buffer[0] = PROXSETTINGS0_VAL;
        data_buffer[1] = PROXSETTINGS1_VAL;
        data_buffer[2] = PROXSETTINGS2_VAL;
        data_buffer[3] = PROXSETTINGS3_VAL;
        data_buffer[4] = PROXSETTINGS4_VAL;
        data_buffer[5] = PROXSETTINGS5_VAL;

        if (iqs333_write(PROXSETTINGS, data_buffer, 6) == 0)
        {
            IQS_State = IQS_STATE_COMPENSATION;
        }
        else
        {
            IQS_State = IQS_STATE_PROXSETTINGS;
        }
        break;

    case IQS_STATE_COMPENSATION:
        /* Setup Compensation (PCC) */
        data_buffer[0] = COMPENSATION_CH0;
        data_buffer[1] = COMPENSATION_CH1;
        data_buffer[2] = COMPENSATION_CH2;
        data_buffer[3] = COMPENSATION_CH3;
        data_buffer[4] = COMPENSATION_CH4;
        data_buffer[5] = COMPENSATION_CH5;
        data_buffer[6] = COMPENSATION_CH6;
        data_buffer[7] = COMPENSATION_CH7;
        data_buffer[8] = COMPENSATION_CH8;
        data_buffer[9] = COMPENSATION_CH9;

        if (iqs333_write(COMPENSATION, data_buffer, 10) == 0)
        {
            IQS_State = IQS_STATE_TIMINGS;
        }
        else
        {
            IQS_State = IQS_STATE_COMPENSATION;
        }
        break;
    case IQS_STATE_TIMINGS:
        /* Set Timings on the IQS333 */
        data_buffer[0] = FILTER_HALT;
        data_buffer[1] = POWER_MODE;
        data_buffer[2] = TIMEOUT_PERIOD;
        data_buffer[3] = CH0_ACF_BETA;
        data_buffer[4] = CH0_9_ACF_BET1;

        if (iqs333_write(TIMINGS, data_buffer, 5) == 0)
        {
            IQS_State = IQS_STATE_PWM_SETING;
        }
        else
        {
            IQS_State = IQS_STATE_TIMINGS;
        }
        break;
    case IQS_STATE_PWM_SETING:
        /* Setup PWM  */
        // if (mainAppCt.appCurrentMode == AppModeStandByAudioPlay) // use BP format JH 23 02 21 if(gSys.CurModuleID == MODULE_ID_STANDBY)
        if (0) /** TODO: check mode !!!*/
        {
//            data_buffer[0] = PWM_0_OFF;
//            data_buffer[1] = PWM_1_OFF;
//            data_buffer[2] = PWM_2_OFF;
//            data_buffer[3] = PWM_3_OFF;
//            data_buffer[4] = PWM_4_OFF;
//            data_buffer[5] = PWM_5_OFF;
//            data_buffer[6] = PWM_6_OFF;
//            data_buffer[7] = PWM_7_OFF;
        }
        else
        {
            data_buffer[0] = 0; // sementara off kan dulu untuk eksperiment JH 09 06 21
            data_buffer[1] = 0; // nyalain 1 wae buat eksperimen JH 21 06 21
            data_buffer[2] = 0; // mode
            data_buffer[3] = 0; // BT
            data_buffer[4] = 0; // PREV
            data_buffer[5] = 0; // PLAY PAUSE
            data_buffer[6] = 0; // PWM_1; //NEXT
            data_buffer[7] = 0; // EMPTY
        }

        if (iqs333_write(PWM, data_buffer, 8) == 0)
        {
            IQS_State = IQS_STATE_PWM_SPEED;
        }
        else
        {
            IQS_State = IQS_STATE_PWM_SETING;
        }
        break;

    case IQS_STATE_PWM_SPEED:
        /*	Change PWM limits and Speed */
        data_buffer[0] = PWM_LIMITS;
        data_buffer[1] = PWM_SPEED;
        if (iqs333_write(PWM_LIMIT, data_buffer, 2) == 0)
        {
            IQS_State = IQS_STATE_REDO_ATI;
        }
        else
        {
            IQS_State = IQS_STATE_PWM_SPEED;
        }
        break;

    case IQS_STATE_REDO_ATI:   // JH 22 01 18
        data_buffer[0] = 0x10; // REDO ATI command
        if (iqs333_write(PROXSETTINGS, data_buffer, 1) == 0)
        {
            IQS_State = IQS_STATE_WAIT_ATI;
        }
        else
        {
            IQS_State = IQS_STATE_REDO_ATI;
        }
        break;

    case IQS_STATE_WAIT_ATI:
        if (iqs333_read(FLAGS, data_buffer, 1) == 0)
        {
            if ((data_buffer[0] & 0x04) != 0x04)
            {
                IQS_State = IQS_STATE_IDLE;

#ifdef CFG_CAP_REINIT_TIMER // JH 21 02 22
                CAP_REINIT_Timeout = 15000;
#endif
            }
        }
        else
        {
            IQS_State = IQS_STATE_WAIT_ATI;
        }
        break;

    case IQS_STATE_IDLE:
        // nothing
        break;
    }
}



/**
 * @brief   IQS333 read tourch byte 
 * 
*/
uint8_t iqs333_read_touch(void)
{
    uint8_t data[2] = {0};

    if (IQS_State == IQS_STATE_IDLE)
    {
        iqs333_read(TOUCH_BYTES, &data, 2);
        if (data[0] != 0) 
        {
            return (data[0]);
        }
    }

    return 0x00;
}

uint8_t iqs333_get_state(void)
{
    return (IQS_State);
}
uint8_t rdata[30];
void iqs333_test_pwm(void)
{
    static uint8_t index = 3;
    int ret = 0;

    if (IQS_State != IQS_STATE_IDLE) 
        return ;

    ret = iqs333_read(TOUCH_BYTES, &rdata[5], 2);
    delay_ms(10);

#if 1
    pwm_buffer[3] = 0x00;
    pwm_buffer[4] = 0x00;
    pwm_buffer[5] = 0x00;
    pwm_buffer[6] = 0x00;
    //pwm_buffer[index] = 0x7f;
     if (rdata[5] == 0x11)
	 {
     	pwm_buffer[6] = 0x7f;
        iqs333_write(PWM, pwm_buffer, 8);
	 }
	 else if (rdata[5] == 0x41)
	 {
	 	pwm_buffer[5] = 0x7f;
        iqs333_write(PWM, pwm_buffer, 8);
	 }
	 else if (rdata[5] == 0x81)
	 {
	 	pwm_buffer[3] = 0x7f;
        iqs333_write(PWM, pwm_buffer, 8);
	 }
	 else if (rdata[5] == 0x21){
	 	pwm_buffer[4] = 0x7f;
        iqs333_write(PWM, pwm_buffer, 8);
	 }
    
    //if (index++ > 6) index = 3;
#else
    //iqs333_read(TOUCH_BYTES, rdata, 2);
    if (rdata[0] != 0) {
        if (rdata[0] == 0x11)
        {
            pwm_buffer[0] = PWM_OFF;
			pwm_buffer[1] = PWM_OFF;
			pwm_buffer[2] = PWM_OFF;
			pwm_buffer[3] = PWM_3;
			pwm_buffer[4] = PWM_OFF;
			pwm_buffer[5] = PWM_OFF;
			pwm_buffer[6] = PWM_OFF;
			pwm_buffer[7] = PWM_OFF;
            iqs333_write(PWM, pwm_buffer, 8);
        }
        else if (rdata[0] == 0x41)
        {
            pwm_buffer[0] = PWM_OFF;
			pwm_buffer[1] = PWM_OFF;
			pwm_buffer[2] = PWM_OFF;
			pwm_buffer[3] = PWM_OFF;
			pwm_buffer[4] = PWM_4;
			pwm_buffer[5] = PWM_OFF;
			pwm_buffer[6] = PWM_OFF;
			pwm_buffer[7] = PWM_OFF;
            iqs333_write(PWM, pwm_buffer, 8);
        }
        else if (rdata[0] == 0x81)
        {
            pwm_buffer[0] = PWM_OFF;
			pwm_buffer[1] = PWM_OFF;
			pwm_buffer[2] = PWM_OFF;
			pwm_buffer[3] = PWM_OFF;
			pwm_buffer[4] = PWM_OFF;
			pwm_buffer[5] = PWM_5;
			pwm_buffer[6] = PWM_OFF;
			pwm_buffer[7] = PWM_OFF;
            iqs333_write(PWM, pwm_buffer, 8);
        }
        else if (rdata[0] == 0x21){
            pwm_buffer[0] = PWM_OFF;
			pwm_buffer[1] = PWM_OFF;
			pwm_buffer[2] = PWM_OFF;
			pwm_buffer[3] = PWM_OFF;
			pwm_buffer[4] = PWM_OFF;
			pwm_buffer[5] = PWM_OFF;
			pwm_buffer[6] = PWM_6;
			pwm_buffer[7] = PWM_OFF;
            iqs333_write(PWM, pwm_buffer, 8);
        }

    }
#endif
}


/**
 * @brief   get current pwm buffer
*/
uint8_t iqs333_get_pwmbuff(uint8_t index)
{
    //if (index > IQS_PWM_BUFFER_SIZE) return;
    return (pwm_buffer[index]);
}

/**
 * @brief set pwm buffer
*/
void iqs333_set_pwmbuff(uint8_t index, uint8_t value)
{
    pwm_buffer[index] = value;
}

/**
 * @brief write IQS333 pwm buffer
*/
void iqs333_write_pwmbuff(void)
{
    iqs333_write(PWM, &pwm_buffer, 8);
}


#endif /* #if 0*/
