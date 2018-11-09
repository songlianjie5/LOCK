/*******************************************************************************
* File Name: TEST.h  
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

#if !defined(CY_PINS_TEST_ALIASES_H) /* Pins TEST_ALIASES_H */
#define CY_PINS_TEST_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define TEST_0			(TEST__0__PC)
#define TEST_0_PS		(TEST__0__PS)
#define TEST_0_PC		(TEST__0__PC)
#define TEST_0_DR		(TEST__0__DR)
#define TEST_0_SHIFT	(TEST__0__SHIFT)
#define TEST_0_INTR	((uint16)((uint16)0x0003u << (TEST__0__SHIFT*2u)))

#define TEST_INTR_ALL	 ((uint16)(TEST_0_INTR))


#endif /* End Pins TEST_ALIASES_H */


/* [] END OF FILE */
