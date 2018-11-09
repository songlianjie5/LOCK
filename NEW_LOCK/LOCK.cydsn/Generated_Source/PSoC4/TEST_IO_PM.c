/*******************************************************************************
* File Name: TEST_IO.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "TEST_IO.h"

static TEST_IO_BACKUP_STRUCT  TEST_IO_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: TEST_IO_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet TEST_IO_SUT.c usage_TEST_IO_Sleep_Wakeup
*******************************************************************************/
void TEST_IO_Sleep(void)
{
    #if defined(TEST_IO__PC)
        TEST_IO_backup.pcState = TEST_IO_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            TEST_IO_backup.usbState = TEST_IO_CR1_REG;
            TEST_IO_USB_POWER_REG |= TEST_IO_USBIO_ENTER_SLEEP;
            TEST_IO_CR1_REG &= TEST_IO_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(TEST_IO__SIO)
        TEST_IO_backup.sioState = TEST_IO_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        TEST_IO_SIO_REG &= (uint32)(~TEST_IO_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: TEST_IO_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to TEST_IO_Sleep() for an example usage.
*******************************************************************************/
void TEST_IO_Wakeup(void)
{
    #if defined(TEST_IO__PC)
        TEST_IO_PC = TEST_IO_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            TEST_IO_USB_POWER_REG &= TEST_IO_USBIO_EXIT_SLEEP_PH1;
            TEST_IO_CR1_REG = TEST_IO_backup.usbState;
            TEST_IO_USB_POWER_REG &= TEST_IO_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(TEST_IO__SIO)
        TEST_IO_SIO_REG = TEST_IO_backup.sioState;
    #endif
}


/* [] END OF FILE */
