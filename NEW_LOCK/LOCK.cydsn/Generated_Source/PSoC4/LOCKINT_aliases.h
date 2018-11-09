/*******************************************************************************
* File Name: LOCKINT.h  
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

#if !defined(CY_PINS_LOCKINT_ALIASES_H) /* Pins LOCKINT_ALIASES_H */
#define CY_PINS_LOCKINT_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define LOCKINT_0			(LOCKINT__0__PC)
#define LOCKINT_0_PS		(LOCKINT__0__PS)
#define LOCKINT_0_PC		(LOCKINT__0__PC)
#define LOCKINT_0_DR		(LOCKINT__0__DR)
#define LOCKINT_0_SHIFT	(LOCKINT__0__SHIFT)
#define LOCKINT_0_INTR	((uint16)((uint16)0x0003u << (LOCKINT__0__SHIFT*2u)))

#define LOCKINT_1			(LOCKINT__1__PC)
#define LOCKINT_1_PS		(LOCKINT__1__PS)
#define LOCKINT_1_PC		(LOCKINT__1__PC)
#define LOCKINT_1_DR		(LOCKINT__1__DR)
#define LOCKINT_1_SHIFT	(LOCKINT__1__SHIFT)
#define LOCKINT_1_INTR	((uint16)((uint16)0x0003u << (LOCKINT__1__SHIFT*2u)))

#define LOCKINT_2			(LOCKINT__2__PC)
#define LOCKINT_2_PS		(LOCKINT__2__PS)
#define LOCKINT_2_PC		(LOCKINT__2__PC)
#define LOCKINT_2_DR		(LOCKINT__2__DR)
#define LOCKINT_2_SHIFT	(LOCKINT__2__SHIFT)
#define LOCKINT_2_INTR	((uint16)((uint16)0x0003u << (LOCKINT__2__SHIFT*2u)))

#define LOCKINT_3			(LOCKINT__3__PC)
#define LOCKINT_3_PS		(LOCKINT__3__PS)
#define LOCKINT_3_PC		(LOCKINT__3__PC)
#define LOCKINT_3_DR		(LOCKINT__3__DR)
#define LOCKINT_3_SHIFT	(LOCKINT__3__SHIFT)
#define LOCKINT_3_INTR	((uint16)((uint16)0x0003u << (LOCKINT__3__SHIFT*2u)))

#define LOCKINT_INTR_ALL	 ((uint16)(LOCKINT_0_INTR| LOCKINT_1_INTR| LOCKINT_2_INTR| LOCKINT_3_INTR))
#define LOCKINT_OP2INT			(LOCKINT__OP2INT__PC)
#define LOCKINT_OP2INT_PS		(LOCKINT__OP2INT__PS)
#define LOCKINT_OP2INT_PC		(LOCKINT__OP2INT__PC)
#define LOCKINT_OP2INT_DR		(LOCKINT__OP2INT__DR)
#define LOCKINT_OP2INT_SHIFT	(LOCKINT__OP2INT__SHIFT)
#define LOCKINT_OP2INT_INTR	((uint16)((uint16)0x0003u << (LOCKINT__0__SHIFT*2u)))

#define LOCKINT_LOCK_BNT_INT			(LOCKINT__LOCK_BNT_INT__PC)
#define LOCKINT_LOCK_BNT_INT_PS		(LOCKINT__LOCK_BNT_INT__PS)
#define LOCKINT_LOCK_BNT_INT_PC		(LOCKINT__LOCK_BNT_INT__PC)
#define LOCKINT_LOCK_BNT_INT_DR		(LOCKINT__LOCK_BNT_INT__DR)
#define LOCKINT_LOCK_BNT_INT_SHIFT	(LOCKINT__LOCK_BNT_INT__SHIFT)
#define LOCKINT_LOCK_BNT_INT_INTR	((uint16)((uint16)0x0003u << (LOCKINT__1__SHIFT*2u)))

#define LOCKINT_LOCK_MODE_INT			(LOCKINT__LOCK_MODE_INT__PC)
#define LOCKINT_LOCK_MODE_INT_PS		(LOCKINT__LOCK_MODE_INT__PS)
#define LOCKINT_LOCK_MODE_INT_PC		(LOCKINT__LOCK_MODE_INT__PC)
#define LOCKINT_LOCK_MODE_INT_DR		(LOCKINT__LOCK_MODE_INT__DR)
#define LOCKINT_LOCK_MODE_INT_SHIFT	(LOCKINT__LOCK_MODE_INT__SHIFT)
#define LOCKINT_LOCK_MODE_INT_INTR	((uint16)((uint16)0x0003u << (LOCKINT__2__SHIFT*2u)))

#define LOCKINT_LOCK_SLOT_INT			(LOCKINT__LOCK_SLOT_INT__PC)
#define LOCKINT_LOCK_SLOT_INT_PS		(LOCKINT__LOCK_SLOT_INT__PS)
#define LOCKINT_LOCK_SLOT_INT_PC		(LOCKINT__LOCK_SLOT_INT__PC)
#define LOCKINT_LOCK_SLOT_INT_DR		(LOCKINT__LOCK_SLOT_INT__DR)
#define LOCKINT_LOCK_SLOT_INT_SHIFT	(LOCKINT__LOCK_SLOT_INT__SHIFT)
#define LOCKINT_LOCK_SLOT_INT_INTR	((uint16)((uint16)0x0003u << (LOCKINT__3__SHIFT*2u)))


#endif /* End Pins LOCKINT_ALIASES_H */


/* [] END OF FILE */
