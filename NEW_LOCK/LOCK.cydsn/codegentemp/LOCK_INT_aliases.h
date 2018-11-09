/*******************************************************************************
* File Name: LOCK_INT.h  
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

#if !defined(CY_PINS_LOCK_INT_ALIASES_H) /* Pins LOCK_INT_ALIASES_H */
#define CY_PINS_LOCK_INT_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define LOCK_INT_0			(LOCK_INT__0__PC)
#define LOCK_INT_0_PS		(LOCK_INT__0__PS)
#define LOCK_INT_0_PC		(LOCK_INT__0__PC)
#define LOCK_INT_0_DR		(LOCK_INT__0__DR)
#define LOCK_INT_0_SHIFT	(LOCK_INT__0__SHIFT)
#define LOCK_INT_0_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__0__SHIFT*2u)))

#define LOCK_INT_1			(LOCK_INT__1__PC)
#define LOCK_INT_1_PS		(LOCK_INT__1__PS)
#define LOCK_INT_1_PC		(LOCK_INT__1__PC)
#define LOCK_INT_1_DR		(LOCK_INT__1__DR)
#define LOCK_INT_1_SHIFT	(LOCK_INT__1__SHIFT)
#define LOCK_INT_1_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__1__SHIFT*2u)))

#define LOCK_INT_2			(LOCK_INT__2__PC)
#define LOCK_INT_2_PS		(LOCK_INT__2__PS)
#define LOCK_INT_2_PC		(LOCK_INT__2__PC)
#define LOCK_INT_2_DR		(LOCK_INT__2__DR)
#define LOCK_INT_2_SHIFT	(LOCK_INT__2__SHIFT)
#define LOCK_INT_2_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__2__SHIFT*2u)))

#define LOCK_INT_3			(LOCK_INT__3__PC)
#define LOCK_INT_3_PS		(LOCK_INT__3__PS)
#define LOCK_INT_3_PC		(LOCK_INT__3__PC)
#define LOCK_INT_3_DR		(LOCK_INT__3__DR)
#define LOCK_INT_3_SHIFT	(LOCK_INT__3__SHIFT)
#define LOCK_INT_3_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__3__SHIFT*2u)))

#define LOCK_INT_INTR_ALL	 ((uint16)(LOCK_INT_0_INTR| LOCK_INT_1_INTR| LOCK_INT_2_INTR| LOCK_INT_3_INTR))
#define LOCK_INT_OP2INT			(LOCK_INT__OP2INT__PC)
#define LOCK_INT_OP2INT_PS		(LOCK_INT__OP2INT__PS)
#define LOCK_INT_OP2INT_PC		(LOCK_INT__OP2INT__PC)
#define LOCK_INT_OP2INT_DR		(LOCK_INT__OP2INT__DR)
#define LOCK_INT_OP2INT_SHIFT	(LOCK_INT__OP2INT__SHIFT)
#define LOCK_INT_OP2INT_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__0__SHIFT*2u)))

#define LOCK_INT_LOCK_BNT_INT			(LOCK_INT__LOCK_BNT_INT__PC)
#define LOCK_INT_LOCK_BNT_INT_PS		(LOCK_INT__LOCK_BNT_INT__PS)
#define LOCK_INT_LOCK_BNT_INT_PC		(LOCK_INT__LOCK_BNT_INT__PC)
#define LOCK_INT_LOCK_BNT_INT_DR		(LOCK_INT__LOCK_BNT_INT__DR)
#define LOCK_INT_LOCK_BNT_INT_SHIFT	(LOCK_INT__LOCK_BNT_INT__SHIFT)
#define LOCK_INT_LOCK_BNT_INT_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__1__SHIFT*2u)))

#define LOCK_INT_LOCK_MODE_INT			(LOCK_INT__LOCK_MODE_INT__PC)
#define LOCK_INT_LOCK_MODE_INT_PS		(LOCK_INT__LOCK_MODE_INT__PS)
#define LOCK_INT_LOCK_MODE_INT_PC		(LOCK_INT__LOCK_MODE_INT__PC)
#define LOCK_INT_LOCK_MODE_INT_DR		(LOCK_INT__LOCK_MODE_INT__DR)
#define LOCK_INT_LOCK_MODE_INT_SHIFT	(LOCK_INT__LOCK_MODE_INT__SHIFT)
#define LOCK_INT_LOCK_MODE_INT_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__2__SHIFT*2u)))

#define LOCK_INT_LOCK_SLOT_INT			(LOCK_INT__LOCK_SLOT_INT__PC)
#define LOCK_INT_LOCK_SLOT_INT_PS		(LOCK_INT__LOCK_SLOT_INT__PS)
#define LOCK_INT_LOCK_SLOT_INT_PC		(LOCK_INT__LOCK_SLOT_INT__PC)
#define LOCK_INT_LOCK_SLOT_INT_DR		(LOCK_INT__LOCK_SLOT_INT__DR)
#define LOCK_INT_LOCK_SLOT_INT_SHIFT	(LOCK_INT__LOCK_SLOT_INT__SHIFT)
#define LOCK_INT_LOCK_SLOT_INT_INTR	((uint16)((uint16)0x0003u << (LOCK_INT__3__SHIFT*2u)))


#endif /* End Pins LOCK_INT_ALIASES_H */


/* [] END OF FILE */
