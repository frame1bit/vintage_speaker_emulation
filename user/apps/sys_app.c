#include "main.h"
#include "app_config.h"
#include "sys_app.h"
#include "timeout.h"
#include "eeprom/nvm.h"

/** set global flag */
gFlag_t gflag_sys;

/**
 * global system information 
*/
SystemConfig_t system_config = 
{
    .system_volume = DEFAULT_SYS_VOL,
    .current_function = SYS_MODE_BOOTING,
    .blink_periode = 500
};

/** non volatile memory param 
*/
SysNvmParam_t nvmParam;


/**
 * @brief write parameter to non volatile memory
 * @param   pWrite: system config parameter to be write 
 *                  in non volatile memory
*/
static void system_app_save_nvm(SystemConfig_t *pWrite)
{

    nvmParam.system_volume = pWrite->system_volume;

    if (/*pWrite->current_function != SYS_MODE_STANDBY && 
        pWrite->current_function != SYS_MODE_BOOTING &&
        pWrite->current_function != SYS_MODE_FACTORY_RESET*/
        (MAIN_MODE(pWrite->current_function)))
    {
        nvmParam.current_function = pWrite->current_function;
    }

    if (nvm_handler.write(&nvmParam, sizeof(nvmParam)) < 0)
    {
        /** nvm error handler */
    }
}


/***
 * @brief get saved param on non volatile memory (eeprom)
 * @param   pRead: system config parameter to be read
*/
static void system_app_read_nvm(SystemConfig_t *pRead)
{
    nvm_handler.read(&nvmParam, sizeof(nvmParam));

    /** read function */
    if (nvmParam.current_function > SYS_MODE_NUM-1)
    {
        pRead->current_function = CONFIG_DEFAULT_FUNCTION;
    }
    else
    {
        if (MAIN_MODE(nvmParam.current_function))
        {
            pRead->current_function = nvmParam.current_function;
        }
        else
        {
            pRead->current_function = CONFIG_DEFAULT_FUNCTION;
        }
    }

    pRead->pre_function = pRead->current_function;
    pRead->current_function = SYS_MODE_BOOTING;

    /** read volume */
    if (nvmParam.system_volume > SYS_VOL_MAX)
    {
        pRead->system_volume = DEFAULT_SYS_VOL;
    }
}


/***
 * @brief   set default parameter after factory reset
 *          - system function
 *          - system volume
*/
void system_app_set_default(void)
{

	// removed
}

/**
 * @brief   system application init 
*/
void system_app_init(void)
{
	// removed

}

/***
 * @brief   trigger to save to non volatile memory
 * @note    see NVM_SAVE_PERIODE to adjust save timing
*/
void system_app_nvm_save_trig(void)
{
    SET_FLAG(FLAG_SAVE_NVM);
    TimeoutSet(&nvm_handler.tmrWrite, NVM_SAVE_PERIODE);
}

/**
 * @brief handler for saving data to nv memory
 * @note    place at continues loop
*/
void system_app_nvm_saving_process(void)
{

#if (CONFIG_ENABLE_NVM)
    if (GET_FLAG(FLAG_SAVE_NVM))
    {
        if (IsTimeout(&nvm_handler.tmrWrite))
        {
            system_app_save_nvm(&system_config);
            CLR_FLAG(FLAG_SAVE_NVM);
        }
    }

#endif /**CONFIG_ENABLE_NVM*/
}
