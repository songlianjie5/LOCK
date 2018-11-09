/*******************************************************************************
* File Name: COVER_CHK_INT.h  
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

#if !defined(CY_PINS_COVER_CHK_INT_H) /* Pins COVER_CHK_INT_H */
#define CY_PINS_COVER_CHK_INT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "COVER_CHK_INT_aliases.h"


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
} COVER_CHK_INT_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   COVER_CHK_INT_Read(void);
void    COVER_CHK_INT_Write(uint8 value);
uint8   COVER_CHK_INT_ReadDataReg(void);
#if defined(COVER_CHK_INT__PC) || (CY_PSOC4_4200L) 
    void    COVER_CHK_INT_SetDriveMode(uint8 mode);
#endif
void    COVER_CHK_INT_SetInterruptMode(uint16 position, uint16 mode);
uint8   COVER_CHK_INT_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void COVER_CHK_INT_Sleep(void); 
void COVER_CHK_INT_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(COVER_CHK_INT__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define COVER_CHK_INT_DRIVE_MODE_BITS        (3)
    #define COVER_CHK_INT_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - COVER_CHK_INT_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the COVER_CHK_INT_SetDriveMode() function.
         *  @{
         */
        #define COVER_CHK_INT_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define COVER_CHK_INT_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define COVER_CHK_INT_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define COVER_CHK_INT_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define COVER_CHK_INT_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define COVER_CHK_INT_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define COVER_CHK_INT_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define COVER_CHK_INT_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define COVER_CHK_INT_MASK               COVER_CHK_INT__MASK
#define COVER_CHK_INT_SHIFT              COVER_CHK_INT__SHIFT
#define COVER_CHK_INT_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in COVER_CHK_INT_SetInterruptMode() function.
     *  @{
     */
        #define COVER_CHK_INT_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define COVER_CHK_INT_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define COVER_CHK_INT_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define COVER_CHK_INT_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(COVER_CHK_INT__SIO)
    #define COVER_CHK_INT_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(COVER_CHK_INT__PC) && (CY_PSOC4_4200L)
    #define COVER_CHK_INT_USBIO_ENABLE               ((uint32)0x80000000u)
    #define COVER_CHK_INT_USBIO_DISABLE              ((uint32)(~COVER_CHK_INT_USBIO_ENABLE))
    #define COVER_CHK_INT_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define COVER_CHK_INT_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define COVER_CHK_INT_USBIO_ENTER_SLEEP          ((uint32)((1u << COVER_CHK_INT_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << COVER_CHK_INT_USBIO_SUSPEND_DEL_SHIFT)))
    #define COVER_CHK_INT_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << COVER_CHK_INT_USBIO_SUSPEND_SHIFT)))
    #define COVER_CHK_INT_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << COVER_CHK_INT_USBIO_SUSPEND_DEL_SHIFT)))
    #define COVER_CHK_INT_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(COVER_CHK_INT__PC)
    /* Port Configuration */
    #define COVER_CHK_INT_PC                 (* (reg32 *) COVER_CHK_INT__PC)
#endif
/* Pin State */
#define COVER_CHK_INT_PS                     (* (reg32 *) COVER_CHK_INT__PS)
/* Data Register */
#define COVER_CHK_INT_DR                     (* (reg32 *) COVER_CHK_INT__DR)
/* Input Buffer Disable Override */
#define COVER_CHK_INT_INP_DIS                (* (reg32 *) COVER_CHK_INT__PC2)

/* Interrupt configuration Registers */
#define COVER_CHK_INT_INTCFG                 (* (reg32 *) COVER_CHK_INT__INTCFG)
#define COVER_CHK_INT_INTSTAT                (* (reg32 *) COVER_CHK_INT__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define COVER_CHK_INT_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(COVER_CHK_INT__SIO)
    #define COVER_CHK_INT_SIO_REG            (* (reg32 *) COVER_CHK_INT__SIO)
#endif /* (COVER_CHK_INT__SIO_CFG) */

/* USBIO registers */
#if !defined(COVER_CHK_INT__PC) && (CY_PSOC4_4200L)
    #define COVER_CHK_INT_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define COVER_CHK_INT_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define COVER_CHK_INT_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define COVER_CHK_INT_DRIVE_MODE_SHIFT       (0x00u)
#define COVER_CHK_INT_DRIVE_MODE_MASK        (0x07u << COVER_CHK_INT_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins COVER_CHK_INT_H */


/* [] END OF FILE */
