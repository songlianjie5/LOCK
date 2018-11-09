/*******************************************************************************
* File Name: VOICE_DATA.c  
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
#include "VOICE_DATA.h"

static VOICE_DATA_BACKUP_STRUCT  VOICE_DATA_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: VOICE_DATA_Sleep
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
*  \snippet VOICE_DATA_SUT.c usage_VOICE_DATA_Sleep_Wakeup
*******************************************************************************/
void VOICE_DATA_Sleep(void)
{
    #if defined(VOICE_DATA__PC)
        VOICE_DATA_backup.pcState = VOICE_DATA_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            VOICE_DATA_backup.usbState = VOICE_DATA_CR1_REG;
            VOICE_DATA_USB_POWER_REG |= VOICE_DATA_USBIO_ENTER_SLEEP;
            VOICE_DATA_CR1_REG &= VOICE_DATA_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(VOICE_DATA__SIO)
        VOICE_DATA_backup.sioState = VOICE_DATA_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        VOICE_DATA_SIO_REG &= (uint32)(~VOICE_DATA_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: VOICE_DATA_Wakeup
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
*  Refer to VOICE_DATA_Sleep() for an example usage.
*******************************************************************************/
void VOICE_DATA_Wakeup(void)
{
    #if defined(VOICE_DATA__PC)
        VOICE_DATA_PC = VOICE_DATA_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            VOICE_DATA_USB_POWER_REG &= VOICE_DATA_USBIO_EXIT_SLEEP_PH1;
            VOICE_DATA_CR1_REG = VOICE_DATA_backup.usbState;
            VOICE_DATA_USB_POWER_REG &= VOICE_DATA_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(VOICE_DATA__SIO)
        VOICE_DATA_SIO_REG = VOICE_DATA_backup.sioState;
    #endif
}


/* [] END OF FILE */
