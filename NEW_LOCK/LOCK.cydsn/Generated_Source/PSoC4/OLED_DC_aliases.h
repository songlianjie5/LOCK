/*******************************************************************************
* File Name: OLED_DC.h  
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

#if !defined(CY_PINS_OLED_DC_ALIASES_H) /* Pins OLED_DC_ALIASES_H */
#define CY_PINS_OLED_DC_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define OLED_DC_0			(OLED_DC__0__PC)
#define OLED_DC_0_PS		(OLED_DC__0__PS)
#define OLED_DC_0_PC		(OLED_DC__0__PC)
#define OLED_DC_0_DR		(OLED_DC__0__DR)
#define OLED_DC_0_SHIFT	(OLED_DC__0__SHIFT)
#define OLED_DC_0_INTR	((uint16)((uint16)0x0003u << (OLED_DC__0__SHIFT*2u)))

#define OLED_DC_INTR_ALL	 ((uint16)(OLED_DC_0_INTR))


#endif /* End Pins OLED_DC_ALIASES_H */


/* [] END OF FILE */
