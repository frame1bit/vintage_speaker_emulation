/**
 * @file uart_com.c
 * @author c.e.s
 * @brief 
 * @version 0.1
 * @date 2024-05-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "uart_com.h"
#include <stdint.h>

UartCom_t uartcom;

uint16_t data_length;

static uint16_t crc16_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    uint16_t polynomial = 0xA001; 

    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i];

        for (size_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief process uart data 
 * 
 * @note    handler circular buffer content
 *          call at loop
*/
uint8_t uart_com_poll_data(void)
{
    uint16_t crc16 = 0, _crc16;
	uint8_t len;
    static int rx_index = 0;
    uint8_t data_out;
    static uint8_t tx_data[10];

    TimeoutSet(&uartcom.timeout, 100);

    while( (len = MCUCircular_GetDataLen(&uartcom.cbCtx)) > 0 )
    {
        if (IsTimeout(&uartcom.timeout))
            return 0;

        /** get uart data */
        if (MCUCircular_GetData(&uartcom.cbCtx, &data_out, 1) == 0)
        {
            /* reserved */
        }

        if(rx_index == 0)
        {
            if (data_out == COM_HEADER_1ST) {
                uartcom.rx_buffer[rx_index++] = data_out;
            }
        }
        else if (rx_index == 1)
        {
            if (data_out == COM_HEADER_2ND) {
                uartcom.rx_buffer[rx_index++] = data_out;
            }
        }
        else if (rx_index == 2)
        {
            if (data_out == COM_HEADER_3RD) {
                uartcom.rx_buffer[rx_index++] = data_out;
            }
        }
        else if (rx_index == 3)
        {
            if (data_out == COM_HEADER_4TH) {
                uartcom.rx_buffer[rx_index++] = data_out;
            }
        }
        else if (rx_index == 4) /** function code */
        {
            uartcom.rx_buffer[rx_index++] = data_out;
        }
        else if (rx_index < 7)
        {
            uartcom.rx_buffer[rx_index++] = data_out;
            data_length = (uartcom.rx_buffer[5]<<8) | uartcom.rx_buffer[6];
        }
        else
        {
            uartcom.rx_buffer[rx_index] = data_out;
            if (rx_index > UART_COM_BUFF_LEN)
                rx_index = 0;
            
            if (rx_index >= 8 + data_length + 2) {
                crc16 = crc16_ccitt(uartcom.rx_buffer, rx_index-1);
                _crc16 = uartcom.rx_buffer[rx_index-1] | (uartcom.rx_buffer[rx_index] << 8);
                rx_index = 0;
                if (crc16 == _crc16) HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
                return (crc16 == _crc16) ? 1 : 0;
            }
            rx_index = rx_index + 1;
        }
    }

    return 0;
}


/**
 * @brief uart1 receive handler
 * 
 * @note    called on USART1_IRQHandler() function on file stm32f1xx_it.c 
*/
void USART1_IRQ_Handler(void)
{
	uint8_t temp;

    /** if any data received */
    if (LL_USART_IsActiveFlag_RXNE(uartcom.uart_handler))
    {
    	temp = LL_USART_ReceiveData8(uartcom.uart_handler);
        MCUCircular_PutData(&uartcom.cbCtx, &temp, 1);
    }

    /** if any error occur */
    if (LL_USART_IsActiveFlag_ORE(uartcom.uart_handler))
    {

    }
}


/**
 * @brief   uart comm init
*/
void uart_com_init(USART_TypeDef *usart)
{
    uartcom.uart_handler = usart;
    /** init circular buffer */
    MCUCircular_Config(&uartcom.cbCtx, uartcom.circular_buffer, CIRCULAR_BUFFER_LEN);
    LL_USART_EnableIT_RXNE(uartcom.uart_handler);
}

/**
 * @brief send data uart using LL
 * @note    function LL_USART_TransmitData8 usually corrupt and need
 *          delay 5 ms between transmit process
*/
static void uart_com_send_byte(uint8_t data)
{
    uint32_t timeout = 10;

    while(!LL_USART_IsActiveFlag_TXE(uartcom.uart_handler))
    {
        /* Check Systick counter flag to decrement the time-out value */
        if (LL_SYSTICK_IsActiveCounterFlag()) 
        { 
            if(timeout-- == 0)
            {
                break;
            }
        } 
    }

    LL_USART_ClearFlag_TC(uartcom.uart_handler); 
    LL_USART_TransmitData8(uartcom.uart_handler, data);
}


/**
 * @brief   send formatted command and data
*/
void uart_com_send_data(uint8_t function_code, uint16_t addr_reg, uint8_t *buff, uint16_t len)
{
    uint16_t crc16;
    uint8_t crc16_buff[2];
    uint8_t temp[BUFFER_TX_LEN];
    uint8_t i;

    temp[0] = COM_HEADER_1ST;
    temp[1] = COM_HEADER_2ND;
    temp[2] = COM_HEADER_3RD;
    temp[3] = COM_HEADER_4TH;
    temp[4] = function_code;
    temp[5] = (uint8_t)(len >> 8) & 0xff;
    temp[6] = (uint8_t)(len & 0xff);
    temp[7] = (uint8_t)(addr_reg>>8) & 0xff;
    temp[8] = (uint8_t)(addr_reg & 0xff);

    if(len)
    {
        for(i = 9; i < (len + 9); i += 1)
        {
            temp[i] = buff[i-9];
        }
    }

    crc16 = crc16_ccitt(temp, i);
    temp[i+0] = (uint8_t)(crc16 & 0xff);
    temp[i+1] = (uint8_t)(crc16 >> 8) & 0xff;

    len = i + 2;
    for (i=0; i < len; i+=1)
    {
        uart_com_send_byte(temp[i]);
    }
}
