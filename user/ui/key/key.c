#include "key.h"
#include "button/button.h"
#include "timeout.h"

uint16_t gFuncID = 0;
DBKey_t DbClick;
TIMER tmrDBClick;

/**
 * PDS : Press Start
 * SPR : Short Press
 * CPS : Long Press to Start
 * CPH : Long Press to hold
*/
static const uint16_t CAPKEY_TAB[][5] = 
{
	// PDS (press start) //SPR (short press release)    // CPS (long press to start) // CPH (long press hold) 
	{MSG_NONE,			MSG_NONE,		    MSG_NONE,		MSG_NONE,			MSG_NONE		},

#if (CONFIG_INVERTED_KEY_POSITION)	
    {MSG_NONE,	        MSG_PLAY_PAUSE,		MSG_NONE,       MSG_NONE,		    MSG_NONE	    },
    {MSG_NONE,			MSG_MODE,		    MSG_POWER,		MSG_NONE,			MSG_NONE		},
	{MSG_NONE,	        MSG_VOL_UP,		    MSG_NEXT,	    MSG_NONE,	        MSG_NONE		},
	{MSG_NONE,	        MSG_VOL_DW,		    MSG_PRE,		MSG_NONE,	        MSG_NONE		},
#else
    {MSG_NONE,			MSG_MODE,		    MSG_POWER,		MSG_NONE,			MSG_NONE		},
	{MSG_NONE,	        MSG_PLAY_PAUSE,		MSG_NONE,       MSG_NONE,		MSG_NONE	},
	{MSG_NONE,	        MSG_VOL_DW,		    MSG_PRE,		MSG_NONE,	        MSG_NONE		},
	{MSG_NONE,	        MSG_VOL_UP,		    MSG_NEXT,	    MSG_NONE,	        MSG_NONE		},
#endif
    {MSG_NONE,			MSG_BT_BROADCAST,	MSG_SPEAKER_MODE,		MSG_NONE,	        MSG_NONE		},
	
    {MSG_NONE,			MSG_NONE,		    MSG_NONE,		MSG_NONE,	        MSG_NONE		},

};

static const uint16_t IOKEY_TAB[][5] =
{
    //KEY_PRESS       SHORT_RELEASE     LONG_PRESS              KEY_HOLD      LONG_PRESS_RELEASE
	{MSG_NONE,        MSG_NONE,         MSG_FACTORY_RESET,      MSG_NONE,     MSG_NONE    },
    {MSG_NONE,        MSG_MODE,         MSG_BT_BROADCAST,       MSG_NONE,     MSG_NONE    },
};

static const uint16_t CODEKEY_TAB[][5] =
{
	//KEY_PRESS       SHORT_RELEASE         LONG_PRESS          KEY_HOLD           LONG_PRESS_RELEASE
	{MSG_NONE,        MSG_VOL_DW,        	MSG_VOL_DW,   		MSG_VOL_DW,  		MSG_NONE},
	{MSG_NONE,        MSG_VOL_UP,          	MSG_VOL_UP,     	MSG_VOL_UP,    		MSG_NONE},

};


/***
 * @brief double click key init
 * @note
*/
void key_double_click_init(void)
{
    DbClick.dbclick_en            = 1;
	DbClick.dbclick_counter       = 0;
	DbClick.dbclick_timeout       = 0;

	DbClick.KeyMsg                = CFG_PARA_CLICK_MSG;////Single click msg
	DbClick.dbclick_msg           = CFG_PARA_DBCLICK_MSG;//double  click msg
	TimeoutSet(&tmrDBClick, 0);
}

/***
 * @brief get double click message 
*/
uint8_t key_double_click_get_msg(uint16_t msg)
{
    if (DbClick.dbclick_en == 0)
        return 0;
    /////You can increase the work mode judgment here. Not all modes require double-click function.////////
    // if( WORK_MODE != BT_MODE) return 0
    if ((DbClick.KeyMsg == msg))
    {
        gFuncID = 0;

        if (DbClick.dbclick_timeout == 0)
        {
            DbClick.dbclick_timeout = CFG_PARA_DBCLICK_DLY_TIME;
            DbClick.dbclick_counter += 1;
        }
        else
        {
            gFuncID = DbClick.dbclick_msg;
            DbClick.dbclick_counter = 0;
            DbClick.dbclick_timeout = 0;
        }
        return 1;
    }
    else
    {
        if (msg != MSG_NONE)
        {
            DbClick.dbclick_timeout = 0;
            DbClick.dbclick_counter = 0;
        }
        return 0;
    }
}

/***
 * @brief process double click
 * @note
*/
void key_double_click_process(void)
{
    if(!IsTimeout(&tmrDBClick))
	{
		return;
	}
	TimeoutSet(&tmrDBClick, 4);

	if(DbClick.dbclick_en)
	{
		if(DbClick.dbclick_timeout)
		{
			DbClick.dbclick_timeout--;
			if(DbClick.dbclick_timeout == 0)
			{
				gFuncID = DbClick.KeyMsg;
				DbClick.dbclick_counter = 0;
				DbClick.dbclick_timeout = 0;
			}
		}
	}
}

/**
 * @brief   all key init
 * @param   void
 * @return  none
*/
void key_init(void)
{
    int ret = 0;

    /** init io button*/
    iobtn_key_init();

#if (CONFIG_CAPSENSE_TOUCH_ENABLE)
    /** init capsense key */
    ret = capsense_init();
#endif

#if (CONFIG_ROTARY_ENCODER_ENABLE)
    rotary_key_init();
#endif

#if (KEY_USE_DOUBLE_CLICK)
    key_double_click_init();
#endif /*KEY_USE_DOUBLE_CLICK*/

}

uint16_t flag_double_click = 0;
uint16_t preKey = MSG_NONE;

uint16_t double_key_process(uint16_t message)
{

    if (message == MSG_VOL_UP)
    {
        if (!IsTimeout(&tmrDBClick))
        {
            // if (preKey != message)
            // {
            //     return message;
            // }
            // else
            {
                return MSG_MODE;
            }
        }
        TimeoutSet(&tmrDBClick, 500);
    }
    return message;
}


/**
 * @brief scan all input key
*/
MessageId key_scan(void)
{
    MessageId Msg = MSG_NONE;
    IOKeyMsg ioKeyMsg;

#if (CONFIG_CAPSENSE_TOUCH_ENABLE)
    CapKeyMsg CapKeyMsg;

    /** capsense init state */
    capsense_running_init_state();
    /** Capsense scan */
    CapKeyMsg = capsense_scan();
    if (CapKeyMsg.index != CAP_KEY_EMPTY && CapKeyMsg.type != CAP_KEY_UNKOWN_TYPE)
    {
        gFuncID = CAPKEY_TAB[CapKeyMsg.index][CapKeyMsg.type - 1];
    }
    /** process pwm capsense */
    capsense_process_pwm();
#endif /*CONFIG_CAPSENSE_TOUCH_ENABLE*/

    /* IO key button scan */
    ioKeyMsg = iobtn_key_scan();
    if (ioKeyMsg.index != IO_CHANNEL_EMPTY && ioKeyMsg.type != IO_KEY_UNKOWN_TYPE)
    {
        gFuncID = IOKEY_TAB[ioKeyMsg.index][ioKeyMsg.type - 1];
    }

#if (CONFIG_ROTARY_ENCODER_ENABLE)
    CodeKeyType CodeKey = CODE_KEY_NONE;
    CodeKey = rotary_key_scan();
    if (CodeKey != CODE_KEY_NONE)
    {
        gFuncID = CODEKEY_TAB[CodeKey-1][1];
    }
#endif /*CONFIG_ROTARY_ENCODER_ENABLE*/

#if (KEY_USE_DOUBLE_CLICK)
    key_double_click_get_msg(gFuncID);

    /** process double click */
    key_double_click_process();
    
#endif /*KEY_USE_DOUBLE_CLICK*/
    Msg = gFuncID;
    gFuncID = 0;

    return Msg;
}



