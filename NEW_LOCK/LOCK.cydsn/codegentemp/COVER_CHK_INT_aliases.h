/*******************************************************************************
* File Name: COVER_CHK_INT.h  
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

#if !defined(CY_PINS_COVER_CHK_INT_ALIASES_H) /* Pins COVER_CHK_INT_ALIASES_H */
#define CY_PINS_COVER_CHK_INT_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define COVER_CHK_INT_0			(COVER_CHK_INT__0__PC)
#define COVER_CHK_INT_0_PS		(COVER_CHK_INT__0__PS)
#define COVER_CHK_INT_0_PC		(COVER_CHK_INT__0__PC)
#define COVER_CHK_INT_0_DR		(COVER_CHK_INT__0__DR)
#define COVER_CHK_INT_0_SHIFT	(COVER_CHK_INT__0__SHIFT)
#define COVER_CHK_INT_0_INTR	((uint16)((uint16)0x0003u << (COVER_CHK_INT__0__SHIFT*2u)))

#define COVER_CHK_INT_INTR_ALL	 ((uint16)(COVER_CHK_INT_0_INTR))


#endif /* End Pins COVER_CHK_INT_ALIASES_H */


/* [] END OF FILE */
