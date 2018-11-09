/*******************************************************************************
* File Name: VOICE_DATA.h  
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

#if !defined(CY_PINS_VOICE_DATA_H) /* Pins VOICE_DATA_H */
#define CY_PINS_VOICE_DATA_H

#include "cytypes.h"
#include "cyfitter.h"
#include "VOICE_DATA_aliases.h"


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
} VOICE_DATA_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   VOICE_DATA_Read(void);
void    VOICE_DATA_Write(uint8 value);
uint8   VOICE_DATA_ReadDataReg(void);
#if defined(VOICE_DATA__PC) || (CY_PSOC4_4200L) 
    void    VOICE_DATA_SetDriveMode(uint8 mode);
#endif
void    VOICE_DATA_SetInterruptMode(uint16 position, uint16 mode);
uint8   VOICE_DATA_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void VOICE_DATA_Sleep(void); 
void VOICE_DATA_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(VOICE_DATA__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define VOICE_DATA_DRIVE_MODE_BITS        (3)
    #define VOICE_DATA_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - VOICE_DATA_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the VOICE_DATA_SetDriveMode() function.
         *  @{
         */
        #define VOICE_DATA_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define VOICE_DATA_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define VOICE_DATA_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define VOICE_DATA_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define VOICE_DATA_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define VOICE_DATA_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define VOICE_DATA_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define VOICE_DATA_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define VOICE_DATA_MASK               VOICE_DATA__MASK
#define VOICE_DATA_SHIFT              VOICE_DATA__SHIFT
#define VOICE_DATA_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VOICE_DATA_SetInterruptMode() function.
     *  @{
     */
        #define VOICE_DATA_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define VOICE_DATA_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define VOICE_DATA_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define VOICE_DATA_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(VOICE_DATA__SIO)
    #define VOICE_DATA_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(VOICE_DATA__PC) && (CY_PSOC4_4200L)
    #define VOICE_DATA_USBIO_ENABLE               ((uint32)0x80000000u)
    #define VOICE_DATA_USBIO_DISABLE              ((uint32)(~VOICE_DATA_USBIO_ENABLE))
    #define VOICE_DATA_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define VOICE_DATA_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define VOICE_DATA_USBIO_ENTER_SLEEP          ((uint32)((1u << VOICE_DATA_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << VOICE_DATA_USBIO_SUSPEND_DEL_SHIFT)))
    #define VOICE_DATA_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << VOICE_DATA_USBIO_SUSPEND_SHIFT)))
    #define VOICE_DATA_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << VOICE_DATA_USBIO_SUSPEND_DEL_SHIFT)))
    #define VOICE_DATA_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(VOICE_DATA__PC)
    /* Port Configuration */
    #define VOICE_DATA_PC                 (* (reg32 *) VOICE_DATA__PC)
#endif
/* Pin State */
#define VOICE_DATA_PS                     (* (reg32 *) VOICE_DATA__PS)
/* Data Register */
#define VOICE_DATA_DR                     (* (reg32 *) VOICE_DATA__DR)
/* Input Buffer Disable Override */
#define VOICE_DATA_INP_DIS                (* (reg32 *) VOICE_DATA__PC2)

/* Interrupt configuration Registers */
#define VOICE_DATA_INTCFG                 (* (reg32 *) VOICE_DATA__INTCFG)
#define VOICE_DATA_INTSTAT                (* (reg32 *) VOICE_DATA__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define VOICE_DATA_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(VOICE_DATA__SIO)
    #define VOICE_DATA_SIO_REG            (* (reg32 *) VOICE_DATA__SIO)
#endif /* (VOICE_DATA__SIO_CFG) */

/* USBIO registers */
#if !defined(VOICE_DATA__PC) && (CY_PSOC4_4200L)
    #define VOICE_DATA_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define VOICE_DATA_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define VOICE_DATA_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define VOICE_DATA_DRIVE_MODE_SHIFT       (0x00u)
#define VOICE_DATA_DRIVE_MODE_MASK        (0x07u << VOICE_DATA_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins VOICE_DATA_H */


/* [] END OF FILE */
