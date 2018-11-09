/***************************************************************************//**
* \file UART_0_FPC_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_0_FPC_PVT.h"
#include "UART_0_FPC_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (UART_0_FPC_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: UART_0_FPC_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(UART_0_FPC_SPI_UART_ISR)
{
#if (UART_0_FPC_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (UART_0_FPC_INTERNAL_RX_SW_BUFFER_CONST) */

#if (UART_0_FPC_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (UART_0_FPC_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef UART_0_FPC_SPI_UART_ISR_ENTRY_CALLBACK
    UART_0_FPC_SPI_UART_ISR_EntryCallback();
#endif /* UART_0_FPC_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != UART_0_FPC_customIntrHandler)
    {
        UART_0_FPC_customIntrHandler();
    }

    #if(UART_0_FPC_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        UART_0_FPC_ClearSpiExtClkInterruptSource(UART_0_FPC_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (UART_0_FPC_CHECK_RX_SW_BUFFER)
    {
        if (UART_0_FPC_CHECK_INTR_RX_MASKED(UART_0_FPC_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (UART_0_FPC_rxBufferHead + 1u);

                /* Adjust local head index */
                if (UART_0_FPC_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == UART_0_FPC_rxBufferTail)
                {
                    #if (UART_0_FPC_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        UART_0_FPC_INTR_RX_MASK_REG &= ~UART_0_FPC_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) UART_0_FPC_RX_FIFO_RD_REG;
                        UART_0_FPC_rxBufferOverflow = (uint8) UART_0_FPC_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    UART_0_FPC_PutWordInRxBuffer(locHead, UART_0_FPC_RX_FIFO_RD_REG);

                    /* Move head index */
                    UART_0_FPC_rxBufferHead = locHead;
                }
            }
            while(0u != UART_0_FPC_GET_RX_FIFO_ENTRIES);

            UART_0_FPC_ClearRxInterruptSource(UART_0_FPC_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (UART_0_FPC_CHECK_TX_SW_BUFFER)
    {
        if (UART_0_FPC_CHECK_INTR_TX_MASKED(UART_0_FPC_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (UART_0_FPC_txBufferHead != UART_0_FPC_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (UART_0_FPC_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (UART_0_FPC_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    UART_0_FPC_TX_FIFO_WR_REG = UART_0_FPC_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    UART_0_FPC_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    UART_0_FPC_DISABLE_INTR_TX(UART_0_FPC_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (UART_0_FPC_SPI_UART_FIFO_SIZE != UART_0_FPC_GET_TX_FIFO_ENTRIES);

            UART_0_FPC_ClearTxInterruptSource(UART_0_FPC_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef UART_0_FPC_SPI_UART_ISR_EXIT_CALLBACK
    UART_0_FPC_SPI_UART_ISR_ExitCallback();
#endif /* UART_0_FPC_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (UART_0_FPC_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */
