/*******************************************************************************
* File Name: VOICE_DATA.h  
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

#if !defined(CY_PINS_VOICE_DATA_ALIASES_H) /* Pins VOICE_DATA_ALIASES_H */
#define CY_PINS_VOICE_DATA_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define VOICE_DATA_0			(VOICE_DATA__0__PC)
#define VOICE_DATA_0_PS		(VOICE_DATA__0__PS)
#define VOICE_DATA_0_PC		(VOICE_DATA__0__PC)
#define VOICE_DATA_0_DR		(VOICE_DATA__0__DR)
#define VOICE_DATA_0_SHIFT	(VOICE_DATA__0__SHIFT)
#define VOICE_DATA_0_INTR	((uint16)((uint16)0x0003u << (VOICE_DATA__0__SHIFT*2u)))

#define VOICE_DATA_INTR_ALL	 ((uint16)(VOICE_DATA_0_INTR))


#endif /* End Pins VOICE_DATA_ALIASES_H */


/* [] END OF FILE */
