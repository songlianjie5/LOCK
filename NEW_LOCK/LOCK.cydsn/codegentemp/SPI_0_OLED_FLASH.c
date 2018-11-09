/***************************************************************************//**
* \file SPI_0_OLED_FLASH.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPI_0_OLED_FLASH_PVT.h"

#if (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC)
    #include "SPI_0_OLED_FLASH_I2C_PVT.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_I2C_INC) */

#if (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_INC)
    #include "SPI_0_OLED_FLASH_EZI2C_PVT.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_INC) */

#if (SPI_0_OLED_FLASH_SCB_MODE_SPI_INC || SPI_0_OLED_FLASH_SCB_MODE_UART_INC)
    #include "SPI_0_OLED_FLASH_SPI_UART_PVT.h"
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_SPI_INC || SPI_0_OLED_FLASH_SCB_MODE_UART_INC) */


/***************************************
*    Run Time Configuration Vars
***************************************/

/* Stores internal component configuration for Unconfigured mode */
#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    uint8 SPI_0_OLED_FLASH_scbMode = SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG;
    uint8 SPI_0_OLED_FLASH_scbEnableWake;
    uint8 SPI_0_OLED_FLASH_scbEnableIntr;

    /* I2C configuration variables */
    uint8 SPI_0_OLED_FLASH_mode;
    uint8 SPI_0_OLED_FLASH_acceptAddr;

    /* SPI/UART configuration variables */
    volatile uint8 * SPI_0_OLED_FLASH_rxBuffer;
    uint8  SPI_0_OLED_FLASH_rxDataBits;
    uint32 SPI_0_OLED_FLASH_rxBufferSize;

    volatile uint8 * SPI_0_OLED_FLASH_txBuffer;
    uint8  SPI_0_OLED_FLASH_txDataBits;
    uint32 SPI_0_OLED_FLASH_txBufferSize;

    /* EZI2C configuration variables */
    uint8 SPI_0_OLED_FLASH_numberOfAddr;
    uint8 SPI_0_OLED_FLASH_subAddrSize;
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Common SCB Vars
***************************************/
/**
* \addtogroup group_general
* \{
*/

/** SPI_0_OLED_FLASH_initVar indicates whether the SPI_0_OLED_FLASH 
*  component has been initialized. The variable is initialized to 0 
*  and set to 1 the first time SCB_Start() is called. This allows 
*  the component to restart without reinitialization after the first 
*  call to the SPI_0_OLED_FLASH_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  SPI_0_OLED_FLASH_Init() function can be called before the 
*  SPI_0_OLED_FLASH_Start() or SPI_0_OLED_FLASH_Enable() function.
*/
uint8 SPI_0_OLED_FLASH_initVar = 0u;


#if (! (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG || \
        SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG))
    /** This global variable stores TX interrupt sources after 
    * SPI_0_OLED_FLASH_Stop() is called. Only these TX interrupt sources 
    * will be restored on a subsequent SPI_0_OLED_FLASH_Enable() call.
    */
    uint16 SPI_0_OLED_FLASH_IntrTxMask = 0u;
#endif /* (! (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG || \
              SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG)) */
/** \} globals */

#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER)
    void (*SPI_0_OLED_FLASH_customIntrHandler)(void) = NULL;
#endif /* !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER) */
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */


/***************************************
*    Private Function Prototypes
***************************************/

static void SPI_0_OLED_FLASH_ScbEnableIntr(void);
static void SPI_0_OLED_FLASH_ScbModeStop(void);
static void SPI_0_OLED_FLASH_ScbModePostEnable(void);


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_Init
****************************************************************************//**
*
*  Initializes the SPI_0_OLED_FLASH component to operate in one of the selected
*  configurations: I2C, SPI, UART or EZI2C.
*  When the configuration is set to "Unconfigured SCB", this function does
*  not do any initialization. Use mode-specific initialization APIs instead:
*  SPI_0_OLED_FLASH_I2CInit, SPI_0_OLED_FLASH_SpiInit, 
*  SPI_0_OLED_FLASH_UartInit or SPI_0_OLED_FLASH_EzI2CInit.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_Init(void)
{
#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_initVar = 0u;
    }
    else
    {
        /* Initialization was done before this function call */
    }

#elif (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG)
    SPI_0_OLED_FLASH_I2CInit();

#elif (SPI_0_OLED_FLASH_SCB_MODE_SPI_CONST_CFG)
    SPI_0_OLED_FLASH_SpiInit();

#elif (SPI_0_OLED_FLASH_SCB_MODE_UART_CONST_CFG)
    SPI_0_OLED_FLASH_UartInit();

#elif (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG)
    SPI_0_OLED_FLASH_EzI2CInit();

#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_Enable
****************************************************************************//**
*
*  Enables SPI_0_OLED_FLASH component operation: activates the hardware and 
*  internal interrupt. It also restores TX interrupt sources disabled after the 
*  SPI_0_OLED_FLASH_Stop() function was called (note that level-triggered TX 
*  interrupt sources remain disabled to not cause code lock-up).
*  For I2C and EZI2C modes the interrupt is internal and mandatory for 
*  operation. For SPI and UART modes the interrupt can be configured as none, 
*  internal or external.
*  The SPI_0_OLED_FLASH configuration should be not changed when the component
*  is enabled. Any configuration changes should be made after disabling the 
*  component.
*  When configuration is set to “Unconfigured SPI_0_OLED_FLASH”, the component 
*  must first be initialized to operate in one of the following configurations: 
*  I2C, SPI, UART or EZ I2C, using the mode-specific initialization API. 
*  Otherwise this function does not enable the component.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_Enable(void)
{
#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Enable SCB block, only if it is already configured */
    if (!SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_CTRL_REG |= SPI_0_OLED_FLASH_CTRL_ENABLED;

        SPI_0_OLED_FLASH_ScbEnableIntr();

        /* Call PostEnable function specific to current operation mode */
        SPI_0_OLED_FLASH_ScbModePostEnable();
    }
#else
    SPI_0_OLED_FLASH_CTRL_REG |= SPI_0_OLED_FLASH_CTRL_ENABLED;

    SPI_0_OLED_FLASH_ScbEnableIntr();

    /* Call PostEnable function specific to current operation mode */
    SPI_0_OLED_FLASH_ScbModePostEnable();
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_Start
****************************************************************************//**
*
*  Invokes SPI_0_OLED_FLASH_Init() and SPI_0_OLED_FLASH_Enable().
*  After this function call, the component is enabled and ready for operation.
*  When configuration is set to "Unconfigured SCB", the component must first be
*  initialized to operate in one of the following configurations: I2C, SPI, UART
*  or EZI2C. Otherwise this function does not enable the component.
*
* \globalvars
*  SPI_0_OLED_FLASH_initVar - used to check initial configuration, modified
*  on first function call.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_Start(void)
{
    if (0u == SPI_0_OLED_FLASH_initVar)
    {
        SPI_0_OLED_FLASH_Init();
        SPI_0_OLED_FLASH_initVar = 1u; /* Component was initialized */
    }

    SPI_0_OLED_FLASH_Enable();
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_Stop
****************************************************************************//**
*
*  Disables the SPI_0_OLED_FLASH component: disable the hardware and internal 
*  interrupt. It also disables all TX interrupt sources so as not to cause an 
*  unexpected interrupt trigger because after the component is enabled, the 
*  TX FIFO is empty.
*  Refer to the function SPI_0_OLED_FLASH_Enable() for the interrupt 
*  configuration details.
*  This function disables the SCB component without checking to see if 
*  communication is in progress. Before calling this function it may be 
*  necessary to check the status of communication to make sure communication 
*  is complete. If this is not done then communication could be stopped mid 
*  byte and corrupted data could result.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_Stop(void)
{
#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
    SPI_0_OLED_FLASH_DisableInt();
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */

    /* Call Stop function specific to current operation mode */
    SPI_0_OLED_FLASH_ScbModeStop();

    /* Disable SCB IP */
    SPI_0_OLED_FLASH_CTRL_REG &= (uint32) ~SPI_0_OLED_FLASH_CTRL_ENABLED;

    /* Disable all TX interrupt sources so as not to cause an unexpected
    * interrupt trigger after the component will be enabled because the 
    * TX FIFO is empty.
    * For SCB IP v0, it is critical as it does not mask-out interrupt
    * sources when it is disabled. This can cause a code lock-up in the
    * interrupt handler because TX FIFO cannot be loaded after the block
    * is disabled.
    */
    SPI_0_OLED_FLASH_SetTxInterruptMode(SPI_0_OLED_FLASH_NO_INTR_SOURCES);

#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
    SPI_0_OLED_FLASH_ClearPendingInt();
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_SetRxFifoLevel
****************************************************************************//**
*
*  Sets level in the RX FIFO to generate a RX level interrupt.
*  When the RX FIFO has more entries than the RX FIFO level an RX level
*  interrupt request is generated.
*
*  \param level: Level in the RX FIFO to generate RX level interrupt.
*   The range of valid level values is between 0 and RX FIFO depth - 1.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_SetRxFifoLevel(uint32 level)
{
    uint32 rxFifoCtrl;

    rxFifoCtrl = SPI_0_OLED_FLASH_RX_FIFO_CTRL_REG;

    rxFifoCtrl &= ((uint32) ~SPI_0_OLED_FLASH_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    rxFifoCtrl |= ((uint32) (SPI_0_OLED_FLASH_RX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SPI_0_OLED_FLASH_RX_FIFO_CTRL_REG = rxFifoCtrl;
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_SetTxFifoLevel
****************************************************************************//**
*
*  Sets level in the TX FIFO to generate a TX level interrupt.
*  When the TX FIFO has less entries than the TX FIFO level an TX level
*  interrupt request is generated.
*
*  \param level: Level in the TX FIFO to generate TX level interrupt.
*   The range of valid level values is between 0 and TX FIFO depth - 1.
*
*******************************************************************************/
void SPI_0_OLED_FLASH_SetTxFifoLevel(uint32 level)
{
    uint32 txFifoCtrl;

    txFifoCtrl = SPI_0_OLED_FLASH_TX_FIFO_CTRL_REG;

    txFifoCtrl &= ((uint32) ~SPI_0_OLED_FLASH_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK); /* Clear level mask bits */
    txFifoCtrl |= ((uint32) (SPI_0_OLED_FLASH_TX_FIFO_CTRL_TRIGGER_LEVEL_MASK & level));

    SPI_0_OLED_FLASH_TX_FIFO_CTRL_REG = txFifoCtrl;
}


#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
    /*******************************************************************************
    * Function Name: SPI_0_OLED_FLASH_SetCustomInterruptHandler
    ****************************************************************************//**
    *
    *  Registers a function to be called by the internal interrupt handler.
    *  First the function that is registered is called, then the internal interrupt
    *  handler performs any operation such as software buffer management functions
    *  before the interrupt returns.  It is the user's responsibility not to break
    *  the software buffer operations. Only one custom handler is supported, which
    *  is the function provided by the most recent call.
    *  At the initialization time no custom handler is registered.
    *
    *  \param func: Pointer to the function to register.
    *        The value NULL indicates to remove the current custom interrupt
    *        handler.
    *
    *******************************************************************************/
    void SPI_0_OLED_FLASH_SetCustomInterruptHandler(void (*func)(void))
    {
    #if !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER)
        SPI_0_OLED_FLASH_customIntrHandler = func; /* Register interrupt handler */
    #else
        if (NULL != func)
        {
            /* Suppress compiler warning */
        }
    #endif /* !defined (CY_REMOVE_SPI_0_OLED_FLASH_CUSTOM_INTR_HANDLER) */
    }
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_ScbModeEnableIntr
****************************************************************************//**
*
*  Enables an interrupt for a specific mode.
*
*******************************************************************************/
static void SPI_0_OLED_FLASH_ScbEnableIntr(void)
{
#if (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL)
    /* Enable interrupt in NVIC */
    #if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
        if (0u != SPI_0_OLED_FLASH_scbEnableIntr)
        {
            SPI_0_OLED_FLASH_EnableInt();
        }

    #else
        SPI_0_OLED_FLASH_EnableInt();

    #endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */
#endif /* (SPI_0_OLED_FLASH_SCB_IRQ_INTERNAL) */
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_ScbModePostEnable
****************************************************************************//**
*
*  Calls the PostEnable function for a specific operation mode.
*
*******************************************************************************/
static void SPI_0_OLED_FLASH_ScbModePostEnable(void)
{
#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
#if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
    if (SPI_0_OLED_FLASH_SCB_MODE_SPI_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_SpiPostEnable();
    }
    else if (SPI_0_OLED_FLASH_SCB_MODE_UART_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_UartPostEnable();
    }
    else
    {
        /* Unknown mode: do nothing */
    }
#endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */

#elif (SPI_0_OLED_FLASH_SCB_MODE_SPI_CONST_CFG)
    SPI_0_OLED_FLASH_SpiPostEnable();

#elif (SPI_0_OLED_FLASH_SCB_MODE_UART_CONST_CFG)
    SPI_0_OLED_FLASH_UartPostEnable();

#else
    /* Unknown mode: do nothing */
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */
}


/*******************************************************************************
* Function Name: SPI_0_OLED_FLASH_ScbModeStop
****************************************************************************//**
*
*  Calls the Stop function for a specific operation mode.
*
*******************************************************************************/
static void SPI_0_OLED_FLASH_ScbModeStop(void)
{
#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    if (SPI_0_OLED_FLASH_SCB_MODE_I2C_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_I2CStop();
    }
    else if (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_EzI2CStop();
    }
#if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
    else if (SPI_0_OLED_FLASH_SCB_MODE_SPI_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_SpiStop();
    }
    else if (SPI_0_OLED_FLASH_SCB_MODE_UART_RUNTM_CFG)
    {
        SPI_0_OLED_FLASH_UartStop();
    }
#endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */
    else
    {
        /* Unknown mode: do nothing */
    }
#elif (SPI_0_OLED_FLASH_SCB_MODE_I2C_CONST_CFG)
    SPI_0_OLED_FLASH_I2CStop();

#elif (SPI_0_OLED_FLASH_SCB_MODE_EZI2C_CONST_CFG)
    SPI_0_OLED_FLASH_EzI2CStop();

#elif (SPI_0_OLED_FLASH_SCB_MODE_SPI_CONST_CFG)
    SPI_0_OLED_FLASH_SpiStop();

#elif (SPI_0_OLED_FLASH_SCB_MODE_UART_CONST_CFG)
    SPI_0_OLED_FLASH_UartStop();

#else
    /* Unknown mode: do nothing */
#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */
}


#if (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: SPI_0_OLED_FLASH_SetPins
    ****************************************************************************//**
    *
    *  Sets the pins settings accordingly to the selected operation mode.
    *  Only available in the Unconfigured operation mode. The mode specific
    *  initialization function calls it.
    *  Pins configuration is set by PSoC Creator when a specific mode of operation
    *  is selected in design time.
    *
    *  \param mode:      Mode of SCB operation.
    *  \param subMode:   Sub-mode of SCB operation. It is only required for SPI and UART
    *             modes.
    *  \param uartEnableMask: enables TX or RX direction and RTS and CTS signals.
    *
    *******************************************************************************/
    void SPI_0_OLED_FLASH_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask)
    {
        uint32 pinsDm[SPI_0_OLED_FLASH_SCB_PINS_NUMBER];
        uint32 i;
        
    #if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
        uint32 pinsInBuf = 0u;
    #endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */
        
        uint32 hsiomSel[SPI_0_OLED_FLASH_SCB_PINS_NUMBER] = 
        {
            SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_SEL_GPIO,
            SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_SEL_GPIO,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

    #if (SPI_0_OLED_FLASH_CY_SCBIP_V1)
        /* Supress compiler warning. */
        if ((0u == subMode) || (0u == uartEnableMask))
        {
        }
    #endif /* (SPI_0_OLED_FLASH_CY_SCBIP_V1) */

        /* Set default HSIOM to GPIO and Drive Mode to Analog Hi-Z */
        for (i = 0u; i < SPI_0_OLED_FLASH_SCB_PINS_NUMBER; i++)
        {
            pinsDm[i] = SPI_0_OLED_FLASH_PIN_DM_ALG_HIZ;
        }

        if ((SPI_0_OLED_FLASH_SCB_MODE_I2C   == mode) ||
            (SPI_0_OLED_FLASH_SCB_MODE_EZI2C == mode))
        {
        #if (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN)
            hsiomSel[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_OD_LO;
        #elif (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_SEL_I2C;
            pinsDm  [SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_OD_LO;
        #else
        #endif /* (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN) */
        
        #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
            hsiomSel[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_SEL_I2C;
            pinsDm  [SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_OD_LO;
        #endif /* (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN) */
        }
    #if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
        else if (SPI_0_OLED_FLASH_SCB_MODE_SPI == mode)
        {
        #if (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN)
            hsiomSel[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_SEL_SPI;
        #elif (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN)
            hsiomSel[SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_SEL_SPI;
        #else
        #endif /* (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN) */
        
        #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
            hsiomSel[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_SEL_SPI;
        #endif /* (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN) */
        
        #if (SPI_0_OLED_FLASH_CTS_SCLK_PIN)
            hsiomSel[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX] = SPI_0_OLED_FLASH_CTS_SCLK_HSIOM_SEL_SPI;
        #endif /* (SPI_0_OLED_FLASH_CTS_SCLK_PIN) */

            if (SPI_0_OLED_FLASH_SPI_SLAVE == subMode)
            {
                /* Slave */
                pinsDm[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
                pinsDm[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsDm[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;

            #if (SPI_0_OLED_FLASH_RTS_SS0_PIN)
                /* Only SS0 is valid choice for Slave */
                hsiomSel[SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm  [SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
            #endif /* (SPI_0_OLED_FLASH_RTS_SS0_PIN) */

            #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
                /* Disable input buffer */
                 pinsInBuf |= SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN) */
            }
            else 
            {
                /* (Master) */
                pinsDm[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsDm[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
                pinsDm[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;

            #if (SPI_0_OLED_FLASH_RTS_SS0_PIN)
                hsiomSel [SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_RTS_SS0_HSIOM_SEL_SPI;
                pinsDm   [SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsInBuf |= SPI_0_OLED_FLASH_RTS_SS0_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_RTS_SS0_PIN) */

            #if (SPI_0_OLED_FLASH_SS1_PIN)
                hsiomSel [SPI_0_OLED_FLASH_SS1_PIN_INDEX] = SPI_0_OLED_FLASH_SS1_HSIOM_SEL_SPI;
                pinsDm   [SPI_0_OLED_FLASH_SS1_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsInBuf |= SPI_0_OLED_FLASH_SS1_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_SS1_PIN) */

            #if (SPI_0_OLED_FLASH_SS2_PIN)
                hsiomSel [SPI_0_OLED_FLASH_SS2_PIN_INDEX] = SPI_0_OLED_FLASH_SS2_HSIOM_SEL_SPI;
                pinsDm   [SPI_0_OLED_FLASH_SS2_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsInBuf |= SPI_0_OLED_FLASH_SS2_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_SS2_PIN) */

            #if (SPI_0_OLED_FLASH_SS3_PIN)
                hsiomSel [SPI_0_OLED_FLASH_SS3_PIN_INDEX] = SPI_0_OLED_FLASH_SS3_HSIOM_SEL_SPI;
                pinsDm   [SPI_0_OLED_FLASH_SS3_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                pinsInBuf |= SPI_0_OLED_FLASH_SS3_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_SS3_PIN) */

                /* Disable input buffers */
            #if (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN)
                pinsInBuf |= SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_MASK;
            #elif (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN)
                pinsInBuf |= SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_MASK;
            #else
            #endif /* (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN) */

            #if (SPI_0_OLED_FLASH_CTS_SCLK_PIN)
                pinsInBuf |= SPI_0_OLED_FLASH_CTS_SCLK_PIN_MASK;
            #endif /* (SPI_0_OLED_FLASH_CTS_SCLK_PIN) */
            }
        }
        else /* UART */
        {
            if (SPI_0_OLED_FLASH_UART_MODE_SMARTCARD == subMode)
            {
                /* SmartCard */
            #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
                hsiomSel[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_SEL_UART;
                pinsDm  [SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_OD_LO;
            #endif /* (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN) */
            }
            else /* Standard or IrDA */
            {
                if (0u != (SPI_0_OLED_FLASH_UART_RX_PIN_ENABLE & uartEnableMask))
                {
                #if (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN)
                    hsiomSel[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
                #elif (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN)
                    hsiomSel[SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_SEL_UART;
                    pinsDm  [SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
                #else
                #endif /* (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN) */
                }

                if (0u != (SPI_0_OLED_FLASH_UART_TX_PIN_ENABLE & uartEnableMask))
                {
                #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
                    hsiomSel[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_SEL_UART;
                    pinsDm  [SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                    
                    /* Disable input buffer */
                    pinsInBuf |= SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_MASK;
                #endif /* (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN) */
                }

            #if !(SPI_0_OLED_FLASH_CY_SCBIP_V0 || SPI_0_OLED_FLASH_CY_SCBIP_V1)
                if (SPI_0_OLED_FLASH_UART_MODE_STD == subMode)
                {
                    if (0u != (SPI_0_OLED_FLASH_UART_CTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* CTS input is multiplexed with SCLK */
                    #if (SPI_0_OLED_FLASH_CTS_SCLK_PIN)
                        hsiomSel[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX] = SPI_0_OLED_FLASH_CTS_SCLK_HSIOM_SEL_UART;
                        pinsDm  [SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_DIG_HIZ;
                    #endif /* (SPI_0_OLED_FLASH_CTS_SCLK_PIN) */
                    }

                    if (0u != (SPI_0_OLED_FLASH_UART_RTS_PIN_ENABLE & uartEnableMask))
                    {
                        /* RTS output is multiplexed with SS0 */
                    #if (SPI_0_OLED_FLASH_RTS_SS0_PIN)
                        hsiomSel[SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_RTS_SS0_HSIOM_SEL_UART;
                        pinsDm  [SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX] = SPI_0_OLED_FLASH_PIN_DM_STRONG;
                        
                        /* Disable input buffer */
                        pinsInBuf |= SPI_0_OLED_FLASH_RTS_SS0_PIN_MASK;
                    #endif /* (SPI_0_OLED_FLASH_RTS_SS0_PIN) */
                    }
                }
            #endif /* !(SPI_0_OLED_FLASH_CY_SCBIP_V0 || SPI_0_OLED_FLASH_CY_SCBIP_V1) */
            }
        }
    #endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */

    /* Configure pins: set HSIOM, DM and InputBufEnable */
    /* Note: the DR register settings do not effect the pin output if HSIOM is other than GPIO */

    #if (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_REG,
                                       SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_RX_SCL_MOSI_HSIOM_POS,
                                        hsiomSel[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX]);

        SPI_0_OLED_FLASH_uart_rx_i2c_scl_spi_mosi_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_INDEX]);

        #if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
            SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_uart_rx_i2c_scl_spi_mosi_INP_DIS,
                                         SPI_0_OLED_FLASH_uart_rx_i2c_scl_spi_mosi_MASK,
                                         (0u != (pinsInBuf & SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN_MASK)));
        #endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */
    
    #elif (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_REG,
                                       SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SPI_0_OLED_FLASH_uart_rx_wake_i2c_scl_spi_mosi_SetDriveMode((uint8)
                                                               pinsDm[SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_uart_rx_wake_i2c_scl_spi_mosi_INP_DIS,
                                     SPI_0_OLED_FLASH_uart_rx_wake_i2c_scl_spi_mosi_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN_MASK)));

         /* Set interrupt on falling edge */
        SPI_0_OLED_FLASH_SET_INCFG_TYPE(SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_INTCFG_REG,
                                        SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_INTCFG_TYPE_MASK,
                                        SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_INTCFG_TYPE_POS,
                                        SPI_0_OLED_FLASH_INTCFG_TYPE_FALLING_EDGE);
    #else
    #endif /* (SPI_0_OLED_FLASH_RX_WAKE_SCL_MOSI_PIN) */

    #if (SPI_0_OLED_FLASH_TX_SDA_MISO_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_REG,
                                       SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_TX_SDA_MISO_HSIOM_POS,
                                        hsiomSel[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX]);

        SPI_0_OLED_FLASH_uart_tx_i2c_sda_spi_miso_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_INDEX]);

    #if (!SPI_0_OLED_FLASH_CY_SCBIP_V1)
        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_uart_tx_i2c_sda_spi_miso_INP_DIS,
                                     SPI_0_OLED_FLASH_uart_tx_i2c_sda_spi_miso_MASK,
                                    (0u != (pinsInBuf & SPI_0_OLED_FLASH_TX_SDA_MISO_PIN_MASK)));
    #endif /* (!SPI_0_OLED_FLASH_CY_SCBIP_V1) */
    #endif /* (SPI_0_OLED_FLASH_RX_SCL_MOSI_PIN) */

    #if (SPI_0_OLED_FLASH_CTS_SCLK_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_CTS_SCLK_HSIOM_REG,
                                       SPI_0_OLED_FLASH_CTS_SCLK_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_CTS_SCLK_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX]);

        SPI_0_OLED_FLASH_uart_cts_spi_sclk_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_CTS_SCLK_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_uart_cts_spi_sclk_INP_DIS,
                                     SPI_0_OLED_FLASH_uart_cts_spi_sclk_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_CTS_SCLK_PIN_MASK)));
    #endif /* (SPI_0_OLED_FLASH_CTS_SCLK_PIN) */

    #if (SPI_0_OLED_FLASH_RTS_SS0_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_RTS_SS0_HSIOM_REG,
                                       SPI_0_OLED_FLASH_RTS_SS0_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_RTS_SS0_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX]);

        SPI_0_OLED_FLASH_uart_rts_spi_ss0_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_RTS_SS0_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_uart_rts_spi_ss0_INP_DIS,
                                     SPI_0_OLED_FLASH_uart_rts_spi_ss0_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_RTS_SS0_PIN_MASK)));
    #endif /* (SPI_0_OLED_FLASH_RTS_SS0_PIN) */

    #if (SPI_0_OLED_FLASH_SS1_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_SS1_HSIOM_REG,
                                       SPI_0_OLED_FLASH_SS1_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_SS1_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_SS1_PIN_INDEX]);

        SPI_0_OLED_FLASH_spi_ss1_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_SS1_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_spi_ss1_INP_DIS,
                                     SPI_0_OLED_FLASH_spi_ss1_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_SS1_PIN_MASK)));
    #endif /* (SPI_0_OLED_FLASH_SS1_PIN) */

    #if (SPI_0_OLED_FLASH_SS2_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_SS2_HSIOM_REG,
                                       SPI_0_OLED_FLASH_SS2_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_SS2_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_SS2_PIN_INDEX]);

        SPI_0_OLED_FLASH_spi_ss2_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_SS2_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_spi_ss2_INP_DIS,
                                     SPI_0_OLED_FLASH_spi_ss2_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_SS2_PIN_MASK)));
    #endif /* (SPI_0_OLED_FLASH_SS2_PIN) */

    #if (SPI_0_OLED_FLASH_SS3_PIN)
        SPI_0_OLED_FLASH_SET_HSIOM_SEL(SPI_0_OLED_FLASH_SS3_HSIOM_REG,
                                       SPI_0_OLED_FLASH_SS3_HSIOM_MASK,
                                       SPI_0_OLED_FLASH_SS3_HSIOM_POS,
                                       hsiomSel[SPI_0_OLED_FLASH_SS3_PIN_INDEX]);

        SPI_0_OLED_FLASH_spi_ss3_SetDriveMode((uint8) pinsDm[SPI_0_OLED_FLASH_SS3_PIN_INDEX]);

        SPI_0_OLED_FLASH_SET_INP_DIS(SPI_0_OLED_FLASH_spi_ss3_INP_DIS,
                                     SPI_0_OLED_FLASH_spi_ss3_MASK,
                                     (0u != (pinsInBuf & SPI_0_OLED_FLASH_SS3_PIN_MASK)));
    #endif /* (SPI_0_OLED_FLASH_SS3_PIN) */
    }

#endif /* (SPI_0_OLED_FLASH_SCB_MODE_UNCONFIG_CONST_CFG) */


#if (SPI_0_OLED_FLASH_CY_SCBIP_V0 || SPI_0_OLED_FLASH_CY_SCBIP_V1)
    /*******************************************************************************
    * Function Name: SPI_0_OLED_FLASH_I2CSlaveNackGeneration
    ****************************************************************************//**
    *
    *  Sets command to generate NACK to the address or data.
    *
    *******************************************************************************/
    void SPI_0_OLED_FLASH_I2CSlaveNackGeneration(void)
    {
        /* Check for EC_AM toggle condition: EC_AM and clock stretching for address are enabled */
        if ((0u != (SPI_0_OLED_FLASH_CTRL_REG & SPI_0_OLED_FLASH_CTRL_EC_AM_MODE)) &&
            (0u == (SPI_0_OLED_FLASH_I2C_CTRL_REG & SPI_0_OLED_FLASH_I2C_CTRL_S_NOT_READY_ADDR_NACK)))
        {
            /* Toggle EC_AM before NACK generation */
            SPI_0_OLED_FLASH_CTRL_REG &= ~SPI_0_OLED_FLASH_CTRL_EC_AM_MODE;
            SPI_0_OLED_FLASH_CTRL_REG |=  SPI_0_OLED_FLASH_CTRL_EC_AM_MODE;
        }

        SPI_0_OLED_FLASH_I2C_SLAVE_CMD_REG = SPI_0_OLED_FLASH_I2C_SLAVE_CMD_S_NACK;
    }
#endif /* (SPI_0_OLED_FLASH_CY_SCBIP_V0 || SPI_0_OLED_FLASH_CY_SCBIP_V1) */


/* [] END OF FILE */
