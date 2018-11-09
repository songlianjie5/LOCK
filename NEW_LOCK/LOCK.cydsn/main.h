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
#ifndef _MAIN_H_
#define _MAIN_H_

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"
     
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {LOW = 0, HIGH = !LOW}PinLevel ;
    
#include "Function.h"  
#include "List.h"
#include "Tuch.h"
#include "Display.h"
#include "Rtc.h"
#include "Voice.h"
#include "Card.h"
#include "Fingerprint.h"
#include "Bat.h"
#include "Power.h"
#include "Led.h"
#include "MLock.h"
#include "SwitchKey.h"
//#include "Lock.h"
#include "Wdt.h"
#include "Record.h"
#include "Extend.h"

    
#define PLATFORM        
#define TIME_BASIC_MS 10

#ifdef PLATFORM
#include "project.h"
#include "gpio.h"
#endif




#endif
/* [] END OF FILE */
