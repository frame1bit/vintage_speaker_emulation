#ifndef NVM_H
#define NVM_H

#include "eeprom.h"
#include "timeout.h"

/** 
 * struct for non volatile memory interface */
typedef struct
{
    int (*init)(I2C_HandleTypeDef *i2c);
    int (*write)(uint8_t *dataIn, uint8_t len);
    int (*read)(uint8_t *dataOut, uint8_t len);
    int (*erase)(uint8_t erase_size);

    TIMER tmrWrite;
}NVM_Handler_t;

/** function prototype */

/* end of function prototype */

/** extern variable */
extern NVM_Handler_t nvm_handler;
/** end of extern variable */

#endif /* NVM_H */
