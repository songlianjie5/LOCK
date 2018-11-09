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
#ifndef _TUCH_H_
#define _TUCH_H_
    
#include "project.h"
#include "stdbool.h"
#include "main.h"
    
#define USE_TUNER                       0     

//#define CapSense_TOTAL_BUTTON 12
//#define ABOVE_BASELINE_MAX  100
//#define BLOW_BASELINE_MAX   100
//#define RESET_BASELINE_MAX  5000

/* Device power mode related macros */

/* Active status scan timeout */ 
//#define ACTIVE_SCAN_TIMEOUT_COUNTER     (500)
    
    
//#define WORK_ACTIVE 0
//#define WORK_LOWPOWER 1

//#define SET_BIT(data, bitPosition)  ((data) |= (1 << (bitPosition)))
//#define CLEAR_BIT(data, bitPosition)  ((data) &= (~(1 << (bitPosition))))
    
    

//void CapSense_Active(void);


extern void EnterDeepSleepLowPowerMode(void);  //深度
//==========================================================================
typedef enum {
    TUCH_KEY0 = 0,
    TUCH_KEY1 = 1,
    TUCH_KEY2 = 2,
    TUCH_KEY3 = 3,
    TUCH_KEY4 = 4,
    TUCH_KEY5 = 5,
    TUCH_KEY6 = 6,
    TUCH_KEY7 = 7,
    TUCH_KEY8 = 8,
    TUCH_KEY9 = 9,
    TUCH_KEY_ESC    = 10,
    TUCH_KEY_ENTER  = 11,
    TUCH_KEY_ENTER_UP  = 12,
    TUCH_KEY_NULL = 13,
} TUCH_KEY_t;

typedef enum {
	RELEASE     	= 0x40,
    LONG_CLICK  	= 0x20,
    DOUBLE_CLICK 	= 0x10,
    CLICK       	= 0x00,
} TUCH_STATE_t;

typedef struct {
    uint8_t even;
    uint8_t val;
}Tuch_t;

extern int8_t Tuch_init(void);
extern Tuch_t *Tuch_status(void);
extern bool Tuch_Scan(void);        //浅度
extern bool Tuch_LowpowerScan(void);//深度

#endif
/* [] END OF FILE */
