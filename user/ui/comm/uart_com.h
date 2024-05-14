/**
 * @file uart_com.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UAR_COM_H
#define UAR_COM_H

#include "main.h"
#include "circular_buffer.h"
#include "timeout.h"

/** communication header */
#define COM_HEADER_1ST  0x12
#define COM_HEADER_2ND  0x21
#define COM_HEADER_3RD  0x34
#define COM_HEADER_4TH  0x76


#define UART_COM_BUFF_LEN       20
#define CIRCULAR_BUFFER_LEN     12*2
#define BUFFER_TX_LEN           40

/** handler for uart communication */
typedef struct
{
    uint32_t *uart_handler;
    uint8_t circular_buffer[CIRCULAR_BUFFER_LEN];
    MCU_CIRCULAR_CONTEXT cbCtx;
    uint8_t rx_buffer[UART_COM_BUFF_LEN];
    TIMER timeout;
}UartCom_t;

/** function prototype */
void USART1_IRQ_Handler(void);
void uart_com_init(USART_TypeDef *usart);
uint8_t uart_com_poll_data(void);
void uart_com_send_data(uint8_t function_code, uint16_t addr_reg, uint8_t *buff, uint16_t len);
/** end of function prototype */

/** extern variable */
extern UartCom_t uartcom;
/* end of extern variable */

#endif /*UAR_COM_H*/
