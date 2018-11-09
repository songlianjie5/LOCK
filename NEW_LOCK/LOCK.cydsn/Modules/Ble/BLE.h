/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef __BLE_H
#define __BLE_H

#include "main.h"

extern void BLE_Uart_Init(void);
extern _Bool ReadUartFifo(uint8_t *pData);
extern _Bool BLE_Decode(char *str,uint8_t *dat);
extern void ISR_UART1_BLE(void);

extern void Ble_Init(void);
//#define BLE_INT_LOW     GPIO_ISR1_Write(GPIO_ISR1_Read()&(~0x04))
//#define BLE_INT_HIG     GPIO_ISR1_Write(GPIO_ISR1_Read()|0x04)

#endif

/* [] END OF FILE */
