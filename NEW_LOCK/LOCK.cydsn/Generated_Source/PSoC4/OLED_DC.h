/*******************************************************************************
* File Name: OLED_DC.h  
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

#if !defined(CY_PINS_OLED_DC_H) /* Pins OLED_DC_H */
#define CY_PINS_OLED_DC_H

#include "cytypes.h"
#include "cyfitter.h"
#include "OLED_DC_aliases.h"


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
} OLED_DC_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   OLED_DC_Read(void);
void    OLED_DC_Write(uint8 value);
uint8   OLED_DC_ReadDataReg(void);
#if defined(OLED_DC__PC) || (CY_PSOC4_4200L) 
    void    OLED_DC_SetDriveMode(uint8 mode);
#endif
void    OLED_DC_SetInterruptMode(uint16 position, uint16 mode);
uint8   OLED_DC_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void OLED_DC_Sleep(void); 
void OLED_DC_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(OLED_DC__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define OLED_DC_DRIVE_MODE_BITS        (3)
    #define OLED_DC_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - OLED_DC_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the OLED_DC_SetDriveMode() function.
         *  @{
         */
        #define OLED_DC_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define OLED_DC_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define OLED_DC_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define OLED_DC_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define OLED_DC_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define OLED_DC_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define OLED_DC_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define OLED_DC_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define OLED_DC_MASK               OLED_DC__MASK
#define OLED_DC_SHIFT              OLED_DC__SHIFT
#define OLED_DC_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in OLED_DC_SetInterruptMode() function.
     *  @{
     */
        #define OLED_DC_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define OLED_DC_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define OLED_DC_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define OLED_DC_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(OLED_DC__SIO)
    #define OLED_DC_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(OLED_DC__PC) && (CY_PSOC4_4200L)
    #define OLED_DC_USBIO_ENABLE               ((uint32)0x80000000u)
    #define OLED_DC_USBIO_DISABLE              ((uint32)(~OLED_DC_USBIO_ENABLE))
    #define OLED_DC_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define OLED_DC_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define OLED_DC_USBIO_ENTER_SLEEP          ((uint32)((1u << OLED_DC_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << OLED_DC_USBIO_SUSPEND_DEL_SHIFT)))
    #define OLED_DC_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << OLED_DC_USBIO_SUSPEND_SHIFT)))
    #define OLED_DC_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << OLED_DC_USBIO_SUSPEND_DEL_SHIFT)))
    #define OLED_DC_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(OLED_DC__PC)
    /* Port Configuration */
    #define OLED_DC_PC                 (* (reg32 *) OLED_DC__PC)
#endif
/* Pin State */
#define OLED_DC_PS                     (* (reg32 *) OLED_DC__PS)
/* Data Register */
#define OLED_DC_DR                     (* (reg32 *) OLED_DC__DR)
/* Input Buffer Disable Override */
#define OLED_DC_INP_DIS                (* (reg32 *) OLED_DC__PC2)

/* Interrupt configuration Registers */
#define OLED_DC_INTCFG                 (* (reg32 *) OLED_DC__INTCFG)
#define OLED_DC_INTSTAT                (* (reg32 *) OLED_DC__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define OLED_DC_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(OLED_DC__SIO)
    #define OLED_DC_SIO_REG            (* (reg32 *) OLED_DC__SIO)
#endif /* (OLED_DC__SIO_CFG) */

/* USBIO registers */
#if !defined(OLED_DC__PC) && (CY_PSOC4_4200L)
    #define OLED_DC_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define OLED_DC_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define OLED_DC_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define OLED_DC_DRIVE_MODE_SHIFT       (0x00u)
#define OLED_DC_DRIVE_MODE_MASK        (0x07u << OLED_DC_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins OLED_DC_H */


/* [] END OF FILE */
