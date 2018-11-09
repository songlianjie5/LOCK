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
#ifndef _LOCK_H
#define _LOCK_H
    
#include "main.h"
      
void AdcHal_Init(void);
extern ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel);
extern uint16_t AdcHal_ReadResult(uint8_t chan);

typedef struct
{
    uint8_t option;
    uint8_t status;
    bool act;
    uint8_t slot_status;
    bool slot_act;
}Lock_t;

#define MOTOR_MAX_ADC_VALUE     238     //堵转电流
#define MAINLOCK_TRY            3       //最多尝试次数
#define MAINLOCK_TIMEOUT        500     //电机空转超时5000ms
//option
#define MAINLOCK_NULL_CMD       0       //空命令
#define MAINLOCK_CLOSE_CMD      1       //伸出命令
#define MAINLOCK_OPEN_CMD       2       //缩入命令
//status
#define MAINLOCK_DEFAUL         0       //默认不知道状态
#define MAINLOCK_CLOSEED        1       //伸出到位
#define MAINLOCK_OPENED         2       //缩入到位
#define MAINLOCK_CLOSEING       3       //正在伸出
#define MAINLOCK_OPENING        4       //正在缩入

#define MAINLOCK_CLOSE_EER      5       //伸出堵异常 
#define MAINLOCK_OPEN_EER       6       //缩进堵异常
#define MAINLOCK_ALL_EER        7       //伸出缩进都堵异常
#define MAINLOCK_TIMEOUT_EER    8       //电机运行超时异常
//solt status
#define SLOT_DOOR_OPENED        1       //叉舌松开
#define SLOT_DOOR_CLOSEED       2       //叉舌压紧


extern bool Lock_Init(void);
extern Lock_t *Lock_Status(void);
extern uint8_t LockOpen(void);
extern uint8_t LockClose(void);
extern uint8_t LockProceess(void);
#endif

/* [] END OF FILE */
