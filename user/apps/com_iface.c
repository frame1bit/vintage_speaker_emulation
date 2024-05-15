/**
 * @file com_iface.c
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "com_iface.h"

inline void com_iface_init(USART_TypeDef *usart) 
{
    uart_com_init(usart);
}

inline void com_iface_send_data(uint8_t functionCode, uint16_t addReg, uint8_t *data, uint8_t len)
{
    uart_com_send_data(functionCode, addReg, data, len);
}

static void parsing_read_function_code(uint16_t reg)
{
    uint8_t tx_data[8];

    switch (reg) {
        case ADDR_REG_CHECK_COM:
        tx_data[0] = 0;
        tx_data[1] = 0;
        com_iface_send_data(FUNCTION_CODE_READ, ADDR_REG_CHECK_COM, tx_data, 2);
        break;

        default: break;
    }
}

/**
 * @brief communication polling receive data
 * 
 */
void com_iface_polling_data(void)
{
    uint8_t fcode = 0;
    uint16_t addr_register = 0;

    if (!uart_com_poll_data()) {
        return;
    }

    fcode = uartcom.rx_buffer[4];
    switch (fcode) {
        case FUNCTION_CODE_READ:
        addr_register = uartcom.rx_buffer[7] << 8 | uartcom.rx_buffer[8];
        parsing_read_function_code(addr_register);
        break;

        case FUNCTION_CODE_WRITE:

        break;
        default: break;
    }
    

}