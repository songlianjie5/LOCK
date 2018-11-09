/*******************************************************************************
* File Name: CARD_RESET.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_CARD_RESET_ALIASES_H) /* Pins CARD_RESET_ALIASES_H */
#define CY_PINS_CARD_RESET_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define CARD_RESET_0			(CARD_RESET__0__PC)
#define CARD_RESET_0_PS		(CARD_RESET__0__PS)
#define CARD_RESET_0_PC		(CARD_RESET__0__PC)
#define CARD_RESET_0_DR		(CARD_RESET__0__DR)
#define CARD_RESET_0_SHIFT	(CARD_RESET__0__SHIFT)
#define CARD_RESET_0_INTR	((uint16)((uint16)0x0003u << (CARD_RESET__0__SHIFT*2u)))

#define CARD_RESET_INTR_ALL	 ((uint16)(CARD_RESET_0_INTR))


#endif /* End Pins CARD_RESET_ALIASES_H */


/* [] END OF FILE */
