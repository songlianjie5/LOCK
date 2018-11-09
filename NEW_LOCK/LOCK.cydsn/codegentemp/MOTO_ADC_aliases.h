/*******************************************************************************
* File Name: MOTO_ADC.h  
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

#if !defined(CY_PINS_MOTO_ADC_ALIASES_H) /* Pins MOTO_ADC_ALIASES_H */
#define CY_PINS_MOTO_ADC_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define MOTO_ADC_0			(MOTO_ADC__0__PC)
#define MOTO_ADC_0_PS		(MOTO_ADC__0__PS)
#define MOTO_ADC_0_PC		(MOTO_ADC__0__PC)
#define MOTO_ADC_0_DR		(MOTO_ADC__0__DR)
#define MOTO_ADC_0_SHIFT	(MOTO_ADC__0__SHIFT)
#define MOTO_ADC_0_INTR	((uint16)((uint16)0x0003u << (MOTO_ADC__0__SHIFT*2u)))

#define MOTO_ADC_INTR_ALL	 ((uint16)(MOTO_ADC_0_INTR))


#endif /* End Pins MOTO_ADC_ALIASES_H */


/* [] END OF FILE */
