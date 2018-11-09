/*******************************************************************************
* File Name: LOCK_INT.h  
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

#if !defined(CY_PINS_LOCK_INT_H) /* Pins LOCK_INT_H */
#define CY_PINS_LOCK_INT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "LOCK_INT_aliases.h"


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
} LOCK_INT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   LOCK_INT_Read(void);
void    LOCK_INT_Write(uint8 value);
uint8   LOCK_INT_ReadDataReg(void);
#if defined(LOCK_INT__PC) || (CY_PSOC4_4200L) 
    void    LOCK_INT_SetDriveMode(uint8 mode);
#endif
void    LOCK_INT_SetInterruptMode(uint16 position, uint16 mode);
uint8   LOCK_INT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void LOCK_INT_Sleep(void); 
void LOCK_INT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(LOCK_INT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define LOCK_INT_DRIVE_MODE_BITS        (3)
    #define LOCK_INT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - LOCK_INT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the LOCK_INT_SetDriveMode() function.
         *  @{
         */
        #define LOCK_INT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define LOCK_INT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define LOCK_INT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define LOCK_INT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define LOCK_INT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define LOCK_INT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define LOCK_INT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define LOCK_INT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define LOCK_INT_MASK               LOCK_INT__MASK
#define LOCK_INT_SHIFT              LOCK_INT__SHIFT
#define LOCK_INT_WIDTH              4u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in LOCK_INT_SetInterruptMode() function.
     *  @{
     */
        #define LOCK_INT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define LOCK_INT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define LOCK_INT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define LOCK_INT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(LOCK_INT__SIO)
    #define LOCK_INT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(LOCK_INT__PC) && (CY_PSOC4_4200L)
    #define LOCK_INT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define LOCK_INT_USBIO_DISABLE              ((uint32)(~LOCK_INT_USBIO_ENABLE))
    #define LOCK_INT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define LOCK_INT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define LOCK_INT_USBIO_ENTER_SLEEP          ((uint32)((1u << LOCK_INT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << LOCK_INT_USBIO_SUSPEND_DEL_SHIFT)))
    #define LOCK_INT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << LOCK_INT_USBIO_SUSPEND_SHIFT)))
    #define LOCK_INT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << LOCK_INT_USBIO_SUSPEND_DEL_SHIFT)))
    #define LOCK_INT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(LOCK_INT__PC)
    /* Port Configuration */
    #define LOCK_INT_PC                 (* (reg32 *) LOCK_INT__PC)
#endif
/* Pin State */
#define LOCK_INT_PS                     (* (reg32 *) LOCK_INT__PS)
/* Data Register */
#define LOCK_INT_DR                     (* (reg32 *) LOCK_INT__DR)
/* Input Buffer Disable Override */
#define LOCK_INT_INP_DIS                (* (reg32 *) LOCK_INT__PC2)

/* Interrupt configuration Registers */
#define LOCK_INT_INTCFG                 (* (reg32 *) LOCK_INT__INTCFG)
#define LOCK_INT_INTSTAT                (* (reg32 *) LOCK_INT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define LOCK_INT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(LOCK_INT__SIO)
    #define LOCK_INT_SIO_REG            (* (reg32 *) LOCK_INT__SIO)
#endif /* (LOCK_INT__SIO_CFG) */

/* USBIO registers */
#if !defined(LOCK_INT__PC) && (CY_PSOC4_4200L)
    #define LOCK_INT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define LOCK_INT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define LOCK_INT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define LOCK_INT_DRIVE_MODE_SHIFT       (0x00u)
#define LOCK_INT_DRIVE_MODE_MASK        (0x07u << LOCK_INT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins LOCK_INT_H */


/* [] END OF FILE */
