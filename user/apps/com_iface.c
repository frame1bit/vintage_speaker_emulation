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

/**
 * @brief communication polling receive data
 * 
 */
void com_iface_polling_data(void)
{
    if (!uart_com_poll_data()) {
        return;
    }
    
}