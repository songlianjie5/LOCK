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

#if !defined(CY_SCB_PVT_SPI_0_OLED_FLASH_H)
#define CY_SCB_PVT_SPI_0_OLED_FLASH_H

#include "SPI_0_OLED_FLASH.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define SPI_0_OLED_FLASH_SetI2CExtClkInterruptMode(interruptMask) SPI_0_OLED_FLASH_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define SPI_0_OLED_FLASH_ClearI2CExtClkInterruptSource(interruptMask) SPI_0_OLED_FLASH_CLEAR_INTR_I2C_EC(interruptMask)
#define SPI_0_OLED_FLASH_GetI2CExtClkInterruptSource()                (SPI_0_OLED_FLASH_INTR_I2C_EC_REG)
#define SPI_0_OLED_FLASH_GetI2CExtClkInterruptMode()                  (SPI_0_OLED_FLASH_INTR_I2C_EC_MASK_REG)
#define SPI_0_OLED_FLASH_GetI2CExtClkInterruptSourceMasked()          (SPI_0_OLED_FLASH_INTR_I2C_EC_MASKED_REG)

#if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define SPI_0_OLED_FLASH_SetSpiExtClkInterruptMode(interruptMask) \
                                                                SPI_0_OLED_FLASH_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define SPI_0_OLED_FLASH_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                SPI_0_OLED_FLASH_CLEAR_INTR_SPI_EC(interruptMask)
    #define SPI_0_OLED_FLASH_GetExtSpiClkInterruptSource()                 (SPI_0_OLED_FLASH_INTR_SPI_EC_REG)
    #define SPI_0_OLED_FLASH_GetExtSpiClkInterruptMode()                   (SPI_0_OLED_FLASH_INTR_SPI_EC_MASK_REG)
    #define SPI_0_OLED_FLASH_GetExtSpiClkInterruptSourceMasked()           (SPI_0_OLED_FLASH_INTR_SPI_EC_MASKED_REG)
#endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */

#if(SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void SPI_0_OLED_FLASH_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER)
    extern cyisraddress SPI_0_OLED_FLASH_customIntrHandler;
#endif /* !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER) */
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */

extern SPI_0_OLED_FLASH_BACKUP_STRUCT SPI_0_OLED_FLASH_backup;

#if(SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 SPI_0_OLED_FLASH_scbMode;
    extern uint8 SPI_0_OLED_FLASH_scbEnableWake;
    extern uint8 SPI_0_OLED_FLASH_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 SPI_0_OLED_FLASH_mode;
    extern uint8 SPI_0_OLED_FLASH_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * SPI_0_OLED_FLASH_rxBuffer;
    extern uint8   SPI_0_OLED_FLASH_rxDataBits;
    extern uint32  SPI_0_OLED_FLASH_rxBufferSize;

    extern volatile uint8 * SPI_0_OLED_FLASH_txBuffer;
    extern uint8   SPI_0_OLED_FLASH_txDataBits;
    extern uint32  SPI_0_OLED_FLASH_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 SPI_0_OLED_FLASH_numberOfAddr;
    extern uint8 SPI_0_OLED_FLASH_subAddrSize;
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG || \
        SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 SPI_0_OLED_FLASH_IntrTxMask;
#endif /* (! (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG || \
              SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define SPI_0_OLED_FLASH_SCB_MODE_I2C_RUNTM_CFG     (SPI_0_OLED_FLASH_SCB_MODE_I2C      == SPI_0_OLED_FLASH_scbMode)
    #define SPI_0_OLED_FLASH_SCB_MODE_SPI_RUNTM_CFG     (SPI_0_OLED_FLASH_SCB_MODE_SPI      == SPI_0_OLED_FLASH_scbMode)
    #define SPI_0_OLED_FLASH_SCB_MODE_UART_RUNTM_CFG    (SPI_0_OLED_FLASH_SCB_MODE_UART     == SPI_0_OLED_FLASH_scbMode)
    #define SPI_0_OLED_FLASH_SCB_MODE_EZI2C_RUNTM_CFG   (SPI_0_OLED_FLASH_SCB_MODE_EZI2C    == SPI_0_OLED_FLASH_scbMode)
    #define SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG == SPI_0_OLED_FLASH_scbMode)

    /* Defines wakeup enable */
    #define SPI_0_OLED_FLASH_SCB_WAKE_ENABLE_CHECK       (0u != SPI_0_OLED_FLASH_scbEnableWake)
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
    #define SPI_0_OLED_FLASH_SCB_PINS_NUMBER    (7u)
#else
    #define SPI_0_OLED_FLASH_SCB_PINS_NUMBER    (2u)
#endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_SPI_0_OLED_FLASH_H) */


/* [] END OF FILE */
