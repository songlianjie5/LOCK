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

#if !defined(CY_SCB_PVT_SPI_1_CARD_H)
#define CY_SCB_PVT_SPI_1_CARD_H

#include "SPI_1_CARD.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define SPI_1_CARD_SetI2CExtClkInterruptMode(interruptMask) SPI_1_CARD_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define SPI_1_CARD_ClearI2CExtClkInterruptSource(interruptMask) SPI_1_CARD_CLEAR_INTR_I2C_EC(interruptMask)
#define SPI_1_CARD_GetI2CExtClkInterruptSource()                (SPI_1_CARD_INTR_I2C_EC_REG)
#define SPI_1_CARD_GetI2CExtClkInterruptMode()                  (SPI_1_CARD_INTR_I2C_EC_MASK_REG)
#define SPI_1_CARD_GetI2CExtClkInterruptSourceMasked()          (SPI_1_CARD_INTR_I2C_EC_MASKED_REG)

#if (!SPI_1_CARD_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define SPI_1_CARD_SetSpiExtClkInterruptMode(interruptMask) \
                                                                SPI_1_CARD_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define SPI_1_CARD_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                SPI_1_CARD_CLEAR_INTR_SPI_EC(interruptMask)
    #define SPI_1_CARD_GetExtSpiClkInterruptSource()                 (SPI_1_CARD_INTR_SPI_EC_REG)
    #define SPI_1_CARD_GetExtSpiClkInterruptMode()                   (SPI_1_CARD_INTR_SPI_EC_MASK_REG)
    #define SPI_1_CARD_GetExtSpiClkInterruptSourceMasked()           (SPI_1_CARD_INTR_SPI_EC_MASKED_REG)
#endif /* (!SPI_1_CARD_CY_SCBIP_V1) */

#if(SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void SPI_1_CARD_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (SPI_1_CARD_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SPI_1_CARD_CUSTOM_INTR_HANDLER)
    extern cyisraddress SPI_1_CARD_customIntrHandler;
#endif /* !defined (CY_REMOVE_SPI_1_CARD_CUSTOM_INTR_HANDLER) */
#endif /* (SPI_1_CARD_SCB_IRQ_INTERNAL) */

extern SPI_1_CARD_BACKUP_STRUCT SPI_1_CARD_backup;

#if(SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 SPI_1_CARD_scbMode;
    extern uint8 SPI_1_CARD_scbEnableWake;
    extern uint8 SPI_1_CARD_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 SPI_1_CARD_mode;
    extern uint8 SPI_1_CARD_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * SPI_1_CARD_rxBuffer;
    extern uint8   SPI_1_CARD_rxDataBits;
    extern uint32  SPI_1_CARD_rxBufferSize;

    extern volatile uint8 * SPI_1_CARD_txBuffer;
    extern uint8   SPI_1_CARD_txDataBits;
    extern uint32  SPI_1_CARD_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 SPI_1_CARD_numberOfAddr;
    extern uint8 SPI_1_CARD_subAddrSize;
#endif /* (SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (SPI_1_CARD_SCB_MODE_I2C_CONST_CFG || \
        SPI_1_CARD_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 SPI_1_CARD_IntrTxMask;
#endif /* (! (SPI_1_CARD_SCB_MODE_I2C_CONST_CFG || \
              SPI_1_CARD_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define SPI_1_CARD_SCB_MODE_I2C_RUNTM_CFG     (SPI_1_CARD_SCB_MODE_I2C      == SPI_1_CARD_scbMode)
    #define SPI_1_CARD_SCB_MODE_SPI_RUNTM_CFG     (SPI_1_CARD_SCB_MODE_SPI      == SPI_1_CARD_scbMode)
    #define SPI_1_CARD_SCB_MODE_UART_RUNTM_CFG    (SPI_1_CARD_SCB_MODE_UART     == SPI_1_CARD_scbMode)
    #define SPI_1_CARD_SCB_MODE_EZI2C_RUNTM_CFG   (SPI_1_CARD_SCB_MODE_EZI2C    == SPI_1_CARD_scbMode)
    #define SPI_1_CARD_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (SPI_1_CARD_SCB_MODE_UNCONFIG == SPI_1_CARD_scbMode)

    /* Defines wakeup enable */
    #define SPI_1_CARD_SCB_WAKE_ENABLE_CHECK       (0u != SPI_1_CARD_scbEnableWake)
#endif /* (SPI_1_CARD_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!SPI_1_CARD_CY_SCBIP_V1)
    #define SPI_1_CARD_SCB_PINS_NUMBER    (7u)
#else
    #define SPI_1_CARD_SCB_PINS_NUMBER    (2u)
#endif /* (!SPI_1_CARD_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_SPI_1_CARD_H) */


/* [] END OF FILE */
