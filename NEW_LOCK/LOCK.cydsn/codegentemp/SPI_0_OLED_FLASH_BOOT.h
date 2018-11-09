/***************************************************************************//**
* \file SPI_0_OLED_FLASH_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_SPI_0_OLED_FLASH_H)
#define CY_SCB_BOOT_SPI_0_OLED_FLASH_H

#include "SPI_0_OLED_FLASH_PVT.h"

#if (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC)
    #include "SPI_0_OLED_FLASH_I2C.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC) */

#if (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_INC)
    #include "SPI_0_OLED_FLASH_EZI2C.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_INC) */

#if (SPI_0_OLED_FLASH_SCB_MODE_SPI_INC || SPI_0_OLED_FLASH_SCB_MODE_UART_INC)
    #include "SPI_0_OLED_FLASH_SPI_UART.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_SPI_INC || SPI_0_OLED_FLASH_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SPI_0_OLED_FLASH) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC)
    #define SPI_0_OLED_FLASH_I2C_BTLDR_COMM_ENABLED     (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED && \
                                                            (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SPI_0_OLED_FLASH_I2C_SLAVE_CONST))
#else
     #define SPI_0_OLED_FLASH_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_INC)
    #define SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED   (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED && \
                                                         SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (SPI_0_OLED_FLASH_SCB_MODE_SPI_INC)
    #define SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED     (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED && \
                                                            (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SPI_0_OLED_FLASH_SPI_SLAVE_CONST))
#else
        #define SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (SPI_0_OLED_FLASH_SCB_MODE_UART_INC)
       #define SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED    (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED && \
                                                            (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (SPI_0_OLED_FLASH_UART_RX_DIRECTION && \
                                                              SPI_0_OLED_FLASH_UART_TX_DIRECTION)))
#else
     #define SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define SPI_0_OLED_FLASH_BTLDR_COMM_MODE_ENABLED    (SPI_0_OLED_FLASH_I2C_BTLDR_COMM_ENABLED   || \
                                                     SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED   || \
                                                     SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED || \
                                                     SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void SPI_0_OLED_FLASH_I2CCyBtldrCommStart(void);
    void SPI_0_OLED_FLASH_I2CCyBtldrCommStop (void);
    void SPI_0_OLED_FLASH_I2CCyBtldrCommReset(void);
    cystatus SPI_0_OLED_FLASH_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SPI_0_OLED_FLASH_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG)
        #define SPI_0_OLED_FLASH_CyBtldrCommStart   SPI_0_OLED_FLASH_I2CCyBtldrCommStart
        #define SPI_0_OLED_FLASH_CyBtldrCommStop    SPI_0_OLED_FLASH_I2CCyBtldrCommStop
        #define SPI_0_OLED_FLASH_CyBtldrCommReset   SPI_0_OLED_FLASH_I2CCyBtldrCommReset
        #define SPI_0_OLED_FLASH_CyBtldrCommRead    SPI_0_OLED_FLASH_I2CCyBtldrCommRead
        #define SPI_0_OLED_FLASH_CyBtldrCommWrite   SPI_0_OLED_FLASH_I2CCyBtldrCommWrite
    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void SPI_0_OLED_FLASH_EzI2CCyBtldrCommStart(void);
    void SPI_0_OLED_FLASH_EzI2CCyBtldrCommStop (void);
    void SPI_0_OLED_FLASH_EzI2CCyBtldrCommReset(void);
    cystatus SPI_0_OLED_FLASH_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SPI_0_OLED_FLASH_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG)
        #define SPI_0_OLED_FLASH_CyBtldrCommStart   SPI_0_OLED_FLASH_EzI2CCyBtldrCommStart
        #define SPI_0_OLED_FLASH_CyBtldrCommStop    SPI_0_OLED_FLASH_EzI2CCyBtldrCommStop
        #define SPI_0_OLED_FLASH_CyBtldrCommReset   SPI_0_OLED_FLASH_EzI2CCyBtldrCommReset
        #define SPI_0_OLED_FLASH_CyBtldrCommRead    SPI_0_OLED_FLASH_EzI2CCyBtldrCommRead
        #define SPI_0_OLED_FLASH_CyBtldrCommWrite   SPI_0_OLED_FLASH_EzI2CCyBtldrCommWrite
    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void SPI_0_OLED_FLASH_SpiCyBtldrCommStart(void);
    void SPI_0_OLED_FLASH_SpiCyBtldrCommStop (void);
    void SPI_0_OLED_FLASH_SpiCyBtldrCommReset(void);
    cystatus SPI_0_OLED_FLASH_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SPI_0_OLED_FLASH_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (SPI_0_OLED_FLASH_SCB_MODE_SPI_CONST_CFG)
        #define SPI_0_OLED_FLASH_CyBtldrCommStart   SPI_0_OLED_FLASH_SpiCyBtldrCommStart
        #define SPI_0_OLED_FLASH_CyBtldrCommStop    SPI_0_OLED_FLASH_SpiCyBtldrCommStop
        #define SPI_0_OLED_FLASH_CyBtldrCommReset   SPI_0_OLED_FLASH_SpiCyBtldrCommReset
        #define SPI_0_OLED_FLASH_CyBtldrCommRead    SPI_0_OLED_FLASH_SpiCyBtldrCommRead
        #define SPI_0_OLED_FLASH_CyBtldrCommWrite   SPI_0_OLED_FLASH_SpiCyBtldrCommWrite
    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void SPI_0_OLED_FLASH_UartCyBtldrCommStart(void);
    void SPI_0_OLED_FLASH_UartCyBtldrCommStop (void);
    void SPI_0_OLED_FLASH_UartCyBtldrCommReset(void);
    cystatus SPI_0_OLED_FLASH_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SPI_0_OLED_FLASH_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (SPI_0_OLED_FLASH_SCB_MODE_UART_CONST_CFG)
        #define SPI_0_OLED_FLASH_CyBtldrCommStart   SPI_0_OLED_FLASH_UartCyBtldrCommStart
        #define SPI_0_OLED_FLASH_CyBtldrCommStop    SPI_0_OLED_FLASH_UartCyBtldrCommStop
        #define SPI_0_OLED_FLASH_CyBtldrCommReset   SPI_0_OLED_FLASH_UartCyBtldrCommReset
        #define SPI_0_OLED_FLASH_CyBtldrCommRead    SPI_0_OLED_FLASH_UartCyBtldrCommRead
        #define SPI_0_OLED_FLASH_CyBtldrCommWrite   SPI_0_OLED_FLASH_UartCyBtldrCommWrite
    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED)
    #if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void SPI_0_OLED_FLASH_CyBtldrCommStart(void);
        void SPI_0_OLED_FLASH_CyBtldrCommStop (void);
        void SPI_0_OLED_FLASH_CyBtldrCommReset(void);
        cystatus SPI_0_OLED_FLASH_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus SPI_0_OLED_FLASH_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SPI_0_OLED_FLASH)
        #define CyBtldrCommStart    SPI_0_OLED_FLASH_CyBtldrCommStart
        #define CyBtldrCommStop     SPI_0_OLED_FLASH_CyBtldrCommStop
        #define CyBtldrCommReset    SPI_0_OLED_FLASH_CyBtldrCommReset
        #define CyBtldrCommWrite    SPI_0_OLED_FLASH_CyBtldrCommWrite
        #define CyBtldrCommRead     SPI_0_OLED_FLASH_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SPI_0_OLED_FLASH) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SPI_0_OLED_FLASH_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define SPI_0_OLED_FLASH_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define SPI_0_OLED_FLASH_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define SPI_0_OLED_FLASH_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define SPI_0_OLED_FLASH_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef SPI_0_OLED_FLASH_SPI_BYTE_TO_BYTE
    #define SPI_0_OLED_FLASH_SPI_BYTE_TO_BYTE   (2u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef SPI_0_OLED_FLASH_UART_BYTE_TO_BYTE
    #define SPI_0_OLED_FLASH_UART_BYTE_TO_BYTE  (2500u)
#endif /* SPI_0_OLED_FLASH_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_SPI_0_OLED_FLASH_H) */


/* [] END OF FILE */
