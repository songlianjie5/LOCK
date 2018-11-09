/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *typedef struct{
    
    volatile uint32_t TimeOutCount;
    
    volatile uint8_t WakeFlag;
    
    volatile uint8_t LOW_FLAG;//模式标志  ACTIVITY 活动模式  LOWPOWER 低功耗模式
    
    volatile uint8_t Tick_Flag;
    
    volatile uint8_t Sys_status;
    
    volatile uint8_t Lock_run;
    
}SYSFlAG,*pSYSFLAG;
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef _RTC_H
#define _RTC_H

#include "main.h"

typedef struct
{
    uint16_t year;  /*!< Range from 1970 to 2099.*/
    uint8_t month;  /*!< Range from 1 to 12.*/
    uint8_t day;    /*!< Range from 1 to 31 (depending on month).*/
    uint8_t hour;   /*!< Range from 0 to 23.*/
    uint8_t minute; /*!< Range from 0 to 59.*/
    uint8_t second; /*!< Range from 0 to 59.*/
}Rtc_t;

extern int8_t Rtc_init(void);
extern int8_t Rtc_Set(Rtc_t *t);
extern int8_t Rtc_Get(Rtc_t *t);

#endif

/* [] END OF FILE */
