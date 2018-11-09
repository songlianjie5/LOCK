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
#ifndef _SWITCHKEY_H
#define _SWITCHKEY_H

#include "main.h"

#define COVER_OPEN    1
#define COVER_CLOSE   2    
#define AUTO_MODE     1
#define MANUAL_MODE   2
    
typedef struct{
	uint8_t DoorOneKey;
	uint8_t ResetKeyVal;
    bool ModeSwAct;
    uint8_t ModeSwStatus;
    bool ConverAct;
    uint8_t ConverStatus;
}SwitchKey_t;

enum
{
	LOCK_BNT_KEY       = 12,//一键开关门键
	LOCK_MODE_KEY      = 13,//手自动模式按键
	SLOT_KEY           = 14,//锁体叉舌
	RESET_KEY          = 15,//复位按键
};

extern void SwitchKey_init(void);
extern SwitchKey_t *SwitchKey_Status(void);
extern uint8_t SwitchKey_Scan(void);
extern uint8_t SwitchKey_Cover_Scan(void);
//extern uint8_t SwitchKey_Slot_Scan(void);
extern uint8_t SwitchKey_Mode_Scan(void);


#endif

/* [] END OF FILE */
