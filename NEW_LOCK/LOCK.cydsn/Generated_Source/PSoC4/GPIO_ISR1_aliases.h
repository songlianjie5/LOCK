/*******************************************************************************
* File Name: GPIO_ISR1.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_GPIO_ISR1_ALIASES_H) /* Pins GPIO_ISR1_ALIASES_H */
#define CY_PINS_GPIO_ISR1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define GPIO_ISR1_0			(GPIO_ISR1__0__PC)
#define GPIO_ISR1_0_PS		(GPIO_ISR1__0__PS)
#define GPIO_ISR1_0_PC		(GPIO_ISR1__0__PC)
#define GPIO_ISR1_0_DR		(GPIO_ISR1__0__DR)
#define GPIO_ISR1_0_SHIFT	(GPIO_ISR1__0__SHIFT)
#define GPIO_ISR1_0_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__0__SHIFT*2u)))

#define GPIO_ISR1_1			(GPIO_ISR1__1__PC)
#define GPIO_ISR1_1_PS		(GPIO_ISR1__1__PS)
#define GPIO_ISR1_1_PC		(GPIO_ISR1__1__PC)
#define GPIO_ISR1_1_DR		(GPIO_ISR1__1__DR)
#define GPIO_ISR1_1_SHIFT	(GPIO_ISR1__1__SHIFT)
#define GPIO_ISR1_1_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__1__SHIFT*2u)))

#define GPIO_ISR1_2			(GPIO_ISR1__2__PC)
#define GPIO_ISR1_2_PS		(GPIO_ISR1__2__PS)
#define GPIO_ISR1_2_PC		(GPIO_ISR1__2__PC)
#define GPIO_ISR1_2_DR		(GPIO_ISR1__2__DR)
#define GPIO_ISR1_2_SHIFT	(GPIO_ISR1__2__SHIFT)
#define GPIO_ISR1_2_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__2__SHIFT*2u)))

#define GPIO_ISR1_3			(GPIO_ISR1__3__PC)
#define GPIO_ISR1_3_PS		(GPIO_ISR1__3__PS)
#define GPIO_ISR1_3_PC		(GPIO_ISR1__3__PC)
#define GPIO_ISR1_3_DR		(GPIO_ISR1__3__DR)
#define GPIO_ISR1_3_SHIFT	(GPIO_ISR1__3__SHIFT)
#define GPIO_ISR1_3_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__3__SHIFT*2u)))

#define GPIO_ISR1_INTR_ALL	 ((uint16)(GPIO_ISR1_0_INTR| GPIO_ISR1_1_INTR| GPIO_ISR1_2_INTR| GPIO_ISR1_3_INTR))
#define GPIO_ISR1_KEY_INT			(GPIO_ISR1__KEY_INT__PC)
#define GPIO_ISR1_KEY_INT_PS		(GPIO_ISR1__KEY_INT__PS)
#define GPIO_ISR1_KEY_INT_PC		(GPIO_ISR1__KEY_INT__PC)
#define GPIO_ISR1_KEY_INT_DR		(GPIO_ISR1__KEY_INT__DR)
#define GPIO_ISR1_KEY_INT_SHIFT	(GPIO_ISR1__KEY_INT__SHIFT)
#define GPIO_ISR1_KEY_INT_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__0__SHIFT*2u)))

#define GPIO_ISR1_EXT_UART_2_INT			(GPIO_ISR1__EXT_UART_2_INT__PC)
#define GPIO_ISR1_EXT_UART_2_INT_PS		(GPIO_ISR1__EXT_UART_2_INT__PS)
#define GPIO_ISR1_EXT_UART_2_INT_PC		(GPIO_ISR1__EXT_UART_2_INT__PC)
#define GPIO_ISR1_EXT_UART_2_INT_DR		(GPIO_ISR1__EXT_UART_2_INT__DR)
#define GPIO_ISR1_EXT_UART_2_INT_SHIFT	(GPIO_ISR1__EXT_UART_2_INT__SHIFT)
#define GPIO_ISR1_EXT_UART_2_INT_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__1__SHIFT*2u)))

#define GPIO_ISR1_BLE_INT			(GPIO_ISR1__BLE_INT__PC)
#define GPIO_ISR1_BLE_INT_PS		(GPIO_ISR1__BLE_INT__PS)
#define GPIO_ISR1_BLE_INT_PC		(GPIO_ISR1__BLE_INT__PC)
#define GPIO_ISR1_BLE_INT_DR		(GPIO_ISR1__BLE_INT__DR)
#define GPIO_ISR1_BLE_INT_SHIFT	(GPIO_ISR1__BLE_INT__SHIFT)
#define GPIO_ISR1_BLE_INT_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__2__SHIFT*2u)))

#define GPIO_ISR1_FPC_UART_0_INT			(GPIO_ISR1__FPC_UART_0_INT__PC)
#define GPIO_ISR1_FPC_UART_0_INT_PS		(GPIO_ISR1__FPC_UART_0_INT__PS)
#define GPIO_ISR1_FPC_UART_0_INT_PC		(GPIO_ISR1__FPC_UART_0_INT__PC)
#define GPIO_ISR1_FPC_UART_0_INT_DR		(GPIO_ISR1__FPC_UART_0_INT__DR)
#define GPIO_ISR1_FPC_UART_0_INT_SHIFT	(GPIO_ISR1__FPC_UART_0_INT__SHIFT)
#define GPIO_ISR1_FPC_UART_0_INT_INTR	((uint16)((uint16)0x0003u << (GPIO_ISR1__3__SHIFT*2u)))


#endif /* End Pins GPIO_ISR1_ALIASES_H */


/* [] END OF FILE */
