/*******************************************************************************
* File Name: LOCKINT.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LOCKINT_H) /* Pins LOCKINT_H */
#define CY_PINS_LOCKINT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "LOCKINT_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} LOCKINT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   LOCKINT_Read(void);
void    LOCKINT_Write(uint8 value);
uint8   LOCKINT_ReadDataReg(void);
#if defined(LOCKINT__PC) || (CY_PSOC4_4200L) 
    void    LOCKINT_SetDriveMode(uint8 mode);
#endif
void    LOCKINT_SetInterruptMode(uint16 position, uint16 mode);
uint8   LOCKINT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void LOCKINT_Sleep(void); 
void LOCKINT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(LOCKINT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define LOCKINT_DRIVE_MODE_BITS        (3)
    #define LOCKINT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - LOCKINT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the LOCKINT_SetDriveMode() function.
         *  @{
         */
        #define LOCKINT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define LOCKINT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define LOCKINT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define LOCKINT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define LOCKINT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define LOCKINT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define LOCKINT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define LOCKINT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define LOCKINT_MASK               LOCKINT__MASK
#define LOCKINT_SHIFT              LOCKINT__SHIFT
#define LOCKINT_WIDTH              4u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LOCKINT_SetInterruptMode() function.
     *  @{
     */
        #define LOCKINT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define LOCKINT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define LOCKINT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define LOCKINT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(LOCKINT__SIO)
    #define LOCKINT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(LOCKINT__PC) && (CY_PSOC4_4200L)
    #define LOCKINT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define LOCKINT_USBIO_DISABLE              ((uint32)(~LOCKINT_USBIO_ENABLE))
    #define LOCKINT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define LOCKINT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define LOCKINT_USBIO_ENTER_SLEEP          ((uint32)((1u << LOCKINT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << LOCKINT_USBIO_SUSPEND_DEL_SHIFT)))
    #define LOCKINT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << LOCKINT_USBIO_SUSPEND_SHIFT)))
    #define LOCKINT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << LOCKINT_USBIO_SUSPEND_DEL_SHIFT)))
    #define LOCKINT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(LOCKINT__PC)
    /* Port Configuration */
    #define LOCKINT_PC                 (* (reg32 *) LOCKINT__PC)
#endif
/* Pin State */
#define LOCKINT_PS                     (* (reg32 *) LOCKINT__PS)
/* Data Register */
#define LOCKINT_DR                     (* (reg32 *) LOCKINT__DR)
/* Input Buffer Disable Override */
#define LOCKINT_INP_DIS                (* (reg32 *) LOCKINT__PC2)

/* Interrupt configuration Registers */
#define LOCKINT_INTCFG                 (* (reg32 *) LOCKINT__INTCFG)
#define LOCKINT_INTSTAT                (* (reg32 *) LOCKINT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define LOCKINT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(LOCKINT__SIO)
    #define LOCKINT_SIO_REG            (* (reg32 *) LOCKINT__SIO)
#endif /* (LOCKINT__SIO_CFG) */

/* USBIO registers */
#if !defined(LOCKINT__PC) && (CY_PSOC4_4200L)
    #define LOCKINT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define LOCKINT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define LOCKINT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define LOCKINT_DRIVE_MODE_SHIFT       (0x00u)
#define LOCKINT_DRIVE_MODE_MASK        (0x07u << LOCKINT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins LOCKINT_H */


/* [] END OF FILE */
