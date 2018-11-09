/*******************************************************************************
* File Name: OP2_INT.c  
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
#include "OP2_INT.h"

static OP2_INT_BACKUP_STRUCT  OP2_INT_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: OP2_INT_Sleep
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
*  \snippet OP2_INT_SUT.c usage_OP2_INT_Sleep_Wakeup
*******************************************************************************/
void OP2_INT_Sleep(void)
{
    #if defined(OP2_INT__PC)
        OP2_INT_backup.pcState = OP2_INT_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            OP2_INT_backup.usbState = OP2_INT_CR1_REG;
            OP2_INT_USB_POWER_REG |= OP2_INT_USBIO_ENTER_SLEEP;
            OP2_INT_CR1_REG &= OP2_INT_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(OP2_INT__SIO)
        OP2_INT_backup.sioState = OP2_INT_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        OP2_INT_SIO_REG &= (uint32)(~OP2_INT_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: OP2_INT_Wakeup
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
*  Refer to OP2_INT_Sleep() for an example usage.
*******************************************************************************/
void OP2_INT_Wakeup(void)
{
    #if defined(OP2_INT__PC)
        OP2_INT_PC = OP2_INT_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            OP2_INT_USB_POWER_REG &= OP2_INT_USBIO_EXIT_SLEEP_PH1;
            OP2_INT_CR1_REG = OP2_INT_backup.usbState;
            OP2_INT_USB_POWER_REG &= OP2_INT_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(OP2_INT__SIO)
        OP2_INT_SIO_REG = OP2_INT_backup.sioState;
    #endif
}


/* [] END OF FILE */
