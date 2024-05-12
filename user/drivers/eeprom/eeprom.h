#ifndef EEPROM_H
#define EEPROM_H

#include "app_config.h"
#include "main.h"

/** test using 8kb GT24c08 eeprom */

#define EEPROM_I2C_ADDRESS  (0xa0)

/** determine eeprom capacity */
#define EEPROM_PAGE_MAX_NUM     (64)    // max pages number    
#define EEPROM_PAGE_SIZE        (16)    // in bytes
#define EEPROM_MAX_WORD_ADDRESS (EEPROM_PAGE_MAX_NUM * EEPROM_PAGE_SIZE)

#define EEPROM_SAVE_ADDRESS_LOCATION    (EEPROM_MAX_WORD_ADDRESS)

#define EEPROM_WRITE_OFFSET     (0x0000)    // offset address of saved data

/** wear leveling feature optional */
#define WEAR_LEVELING   1

#define WEAR_LEVELING_ADDRESS   (EEPROM_MAX_WORD_ADDRESS)

/** function prototype */
int eeprom_init(I2C_HandleTypeDef *i2c);
int eeprom_write( uint8_t *data, uint8_t len);
int eeprom_read( uint8_t *data, uint8_t len);
int eeprom_erase(uint8_t erase_size);
void eeprom_test(void);
// int nvm_write(uint8_t *nvData, uint8_t len);
// int nvm_read(uint8_t *nvDataOut, uint8_t len);

#endif /* EEPROM_H */
