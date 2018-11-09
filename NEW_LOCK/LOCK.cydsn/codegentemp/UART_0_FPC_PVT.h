/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_UART_0_FPC_H)
#define CY_SCB_PVT_UART_0_FPC_H

#include "UART_0_FPC.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define UART_0_FPC_SetI2CExtClkInterruptMode(interruptMask) UART_0_FPC_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define UART_0_FPC_ClearI2CExtClkInterruptSource(interruptMask) UART_0_FPC_CLEAR_INTR_I2C_EC(interruptMask)
#define UART_0_FPC_GetI2CExtClkInterruptSource()                (UART_0_FPC_INTR_I2C_EC_REG)
#define UART_0_FPC_GetI2CExtClkInterruptMode()                  (UART_0_FPC_INTR_I2C_EC_MASK_REG)
#define UART_0_FPC_GetI2CExtClkInterruptSourceMasked()          (UART_0_FPC_INTR_I2C_EC_MASKED_REG)

#if (!UART_0_FPC_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define UART_0_FPC_SetSpiExtClkInterruptMode(interruptMask) \
                                                                UART_0_FPC_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define UART_0_FPC_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                UART_0_FPC_CLEAR_INTR_SPI_EC(interruptMask)
    #define UART_0_FPC_GetExtSpiClkInterruptSource()                 (UART_0_FPC_INTR_SPI_EC_REG)
    #define UART_0_FPC_GetExtSpiClkInterruptMode()                   (UART_0_FPC_INTR_SPI_EC_MASK_REG)
    #define UART_0_FPC_GetExtSpiClkInterruptSourceMasked()           (UART_0_FPC_INTR_SPI_EC_MASKED_REG)
#endif /* (!UART_0_FPC_CY_SCBIP_V1) */

#if(UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void UART_0_FPC_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (UART_0_FPC_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_UART_0_FPC_CUSTOM_INTR_HANDLER)
    extern cyisraddress UART_0_FPC_customIntrHandler;
#endif /* !defined (CY_REMOVE_UART_0_FPC_CUSTOM_INTR_HANDLER) */
#endif /* (UART_0_FPC_SCB_IRQ_INTERNAL) */

extern UART_0_FPC_BACKUP_STRUCT UART_0_FPC_backup;

#if(UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 UART_0_FPC_scbMode;
    extern uint8 UART_0_FPC_scbEnableWake;
    extern uint8 UART_0_FPC_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 UART_0_FPC_mode;
    extern uint8 UART_0_FPC_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * UART_0_FPC_rxBuffer;
    extern uint8   UART_0_FPC_rxDataBits;
    extern uint32  UART_0_FPC_rxBufferSize;

    extern volatile uint8 * UART_0_FPC_txBuffer;
    extern uint8   UART_0_FPC_txDataBits;
    extern uint32  UART_0_FPC_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 UART_0_FPC_numberOfAddr;
    extern uint8 UART_0_FPC_subAddrSize;
#endif /* (UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (UART_0_FPC_SCB_MODE_I2C_CONST_CFG || \
        UART_0_FPC_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 UART_0_FPC_IntrTxMask;
#endif /* (! (UART_0_FPC_SCB_MODE_I2C_CONST_CFG || \
              UART_0_FPC_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define UART_0_FPC_SCB_MODE_I2C_RUNTM_CFG     (UART_0_FPC_SCB_MODE_I2C      == UART_0_FPC_scbMode)
    #define UART_0_FPC_SCB_MODE_SPI_RUNTM_CFG     (UART_0_FPC_SCB_MODE_SPI      == UART_0_FPC_scbMode)
    #define UART_0_FPC_SCB_MODE_UART_RUNTM_CFG    (UART_0_FPC_SCB_MODE_UART     == UART_0_FPC_scbMode)
    #define UART_0_FPC_SCB_MODE_EZI2C_RUNTM_CFG   (UART_0_FPC_SCB_MODE_EZI2C    == UART_0_FPC_scbMode)
    #define UART_0_FPC_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (UART_0_FPC_SCB_MODE_UNCONFIG == UART_0_FPC_scbMode)

    /* Defines wakeup enable */
    #define UART_0_FPC_SCB_WAKE_ENABLE_CHECK       (0u != UART_0_FPC_scbEnableWake)
#endif /* (UART_0_FPC_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!UART_0_FPC_CY_SCBIP_V1)
    #define UART_0_FPC_SCB_PINS_NUMBER    (7u)
#else
    #define UART_0_FPC_SCB_PINS_NUMBER    (2u)
#endif /* (!UART_0_FPC_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_UART_0_FPC_H) */


/* [] END OF FILE */
