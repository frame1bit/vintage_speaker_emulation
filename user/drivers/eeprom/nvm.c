#include "nvm.h"

/**
 * function interface for eeprom init, write, read and erase
 * currently just function aliasing 
*/
NVM_Handler_t nvm_handler = 
{
    /** init eeprom before operation,
     * called on init process
    */
    .init = eeprom_init,

    /**
     * non volatile memory write with data buffer to write
     * and length of the data
    */
    .write = eeprom_write,

    /**
     * non volatile memory read with data buffer to read
     * and length of the data to be read
     * return
     * 0: sucesss
     * non zero return indicate failed 
    */
    .read = eeprom_read,

    /**
     * non volatile memory erased with size of byte to be erased
     * depend of the length of data
     * 
     * return:
     * 0: sucess
     * non zero: failed
    */
    .erase = eeprom_erase,
};