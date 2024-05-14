/**
 * @file com_iface.h
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef COM_IFACE_H
#define COM_IFACE_H

#include "comm/uart_com.h"

/** function code */
#define FUNCTION_CODE_WRITE 0x06
#define FUNCTION_CODE_READ  0x03

/** address register */
// read
#define ADDR_REG_CHECK_COM              0x0000
#define ADDR_REG_CHECK_ENROLL_STATUS    0x0001
#define ADDR_REG_CHECK_ID_FINGER        0x0002


// write
#define ADDR_REG_ENROLL_LOOP    0x0001
#define ADDR_REG_DELETE_FINGER  0x0002
#define ADDR_REG_CANCEL_ENROLL  0x0003
#define ADDR_REG_RESET_SYSTEM   0x0004


/** function prototype */
void com_iface_init(USART_TypeDef *usart);
void com_iface_polling_data(void);
/* end of function prototype */

#endif /** end of COM_IFACE_H */