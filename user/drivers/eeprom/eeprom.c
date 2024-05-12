#include "eeprom.h"
#include <stdint.h>


/** eeprom using I2C Bus 1*/
extern I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef *eeI2CInstance;

/**
 * @brief init eeprom 
*/
int eeprom_init(I2C_HandleTypeDef *i2c)
{
    eeI2CInstance = i2c;
    /* init I2C - already initted @main */
    /* check instance */
    if (!eeI2CInstance) {
        /** handler error I2C */
        return -1;
    }
    
    return 0;
}

/**
 * @brief write data to eeprom
 * @return none
*/
int eeprom_write(uint8_t *data, uint8_t len)
{

	int ret = 0;

    if ( (len + EEPROM_WRITE_OFFSET) > EEPROM_MAX_WORD_ADDRESS)
    {
        /** overflow , max word address */
        ret = -1;
    }

    ret = HAL_I2C_Mem_Write(eeI2CInstance,
                        EEPROM_I2C_ADDRESS, 
                        EEPROM_WRITE_OFFSET, 
                        I2C_MEMADD_SIZE_16BIT, 
                        data, 
                        len, 
                        HAL_MAX_DELAY);
    if (ret != HAL_OK)
    {
        /** do error handler */
        ret = -2;
    }
    HAL_Delay(10);

    return (ret);
}

/**
 * @brief read data from eeprom from specific 
*/
int eeprom_read(uint8_t *data, uint8_t len)
{
	int ret = 0;

    if ( (len + EEPROM_WRITE_OFFSET) > EEPROM_MAX_WORD_ADDRESS)
    {
        /** overflow , max word address */
        return -1;
    }
    ret = HAL_I2C_Mem_Read(eeI2CInstance,
                        EEPROM_I2C_ADDRESS, 
                        EEPROM_WRITE_OFFSET, 
                        I2C_MEMADD_SIZE_16BIT, 
                        data, 
                        len, 
                        HAL_MAX_DELAY);
    if (ret != HAL_OK)
    {
        /** do error handler */
        ret = -2;
    }
    HAL_Delay(10);
    
    return (ret);
}

/**
 * @brief erase eeprom
 * @param   erase_size: area to be erased
*/
int eeprom_erase(uint8_t erase_size)
{
    uint8_t *buff = NULL;
    int i = 0;

    if ( (erase_size + EEPROM_WRITE_OFFSET) > EEPROM_MAX_WORD_ADDRESS)
    {
        /** max allowed address */
        return -1;
    }

    buff = malloc( 1 * erase_size);
    if (!buff)
        return -2;
    for(i = 0; i < erase_size; i+=1)
    {
        buff[i] = 0xFF;
    }

    if (eeprom_write(buff, erase_size) < 0)
    {
        /* write failed */
        return -3;
    }

    return 0;

}

/**
 * @brief test write some data and read back data from eeprom
 * @param   none
 * @return none
*/
void eeprom_test(void)
{
    uint8_t data_to_write[5];
    uint8_t data_to_read[5];
    int ret = 0;

    data_to_write[0] = 'a';
    data_to_write[1] = 's';
    data_to_write[2] = 'u';
    
    /** test write data */
    ret = eeprom_write(&data_to_write[0], 3);
    if (ret != 0)
    {
        /* do error handler */
        return ret;
    }

    HAL_Delay(10);  // wait with some delay

    ret = eeprom_read(&data_to_read, 3);
    if (ret != 0)
    {
        /* do error handler */
        return ret;
    } 

}


