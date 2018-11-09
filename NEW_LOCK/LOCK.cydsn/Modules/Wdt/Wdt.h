/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef _WDT_H
#define _WDT_H
    
#include "main.h"
    
typedef struct{
    bool clear;
    bool Isr;
}Wdt_t;
    
extern void WDT_Init(void);
extern void Wdt_Feed_Dog(void);
extern Wdt_t *WDT_Status(void);

#endif

/* [] END OF FILE */
