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
#ifndef _FUNCTION_H_
#define _FUNCTION_H_
    
#include "main.h" 
#include "MenuTips.h"  
#include "Utilities.h"

typedef enum{
    MUTE_OFF=0,
    MUTE_ON=1
}VOICE_E;
typedef enum{
    CHN=0,
    EN=1
}LANGUAGE_E;
typedef enum{
    DOOR_NORMAL=0,
    DOOR_SAVFT=1
}DOORMODE_E;

typedef struct{
    uint8_t SysSN[16];                                              //列号
    uint8_t UserPSW[12];
    uint8_t ManagePSW[12];
    VOICE_E Voice;                                                  //语音
    LANGUAGE_E Language;                                            //语言
    DOORMODE_E OpenDoorMode;                                        //开门模式
    uint8_t NetStaues;
}ManageMode_t;

#define DEEPSLEEP_TIME   10000                                      //无操作时睡眠超时
#define USER_PSW_LEN     16                                         //正常模式密码长度
#define MAX_ERRO_TIME    5                                          //最多错误次数
typedef struct{ 
    uint8_t ManagePSW[USER_PSW_LEN];
    bool ALM;                                                       //布防状态
    bool FANG_SUO;                                                  //反锁
    bool SYSTEM_LOCK;                                               //系统锁定状态
    bool BAT_ALM;                                                   //低电量报警
    bool AUTOMODE;                                                  //手自动
}NormalMode_t;

typedef struct{
    uint8_t Option;
    uint8_t Status;
}LockMode_t;

typedef struct{
    enum{INIT=0,SLEEP=1,DEEP_SLEEP=2,INT_MODE=3}McuState;           //MCU工作状态
    enum{NORMAL_MODE=0,MANAGE_MODE=1,LOCK_MODE=2}workmode;          //功能体模式
	ManageMode_t ManageMode;                                        //管理模式下的变量
    NormalMode_t NormalMode;                                        //正常模式下的变量
    LockMode_t LockMode;                                            //锁定模式下的变量
    uint16_t TimeOutCount;                                          //进入睡眠的计时
    bool Wakeup;                                                    //从睡眠唤醒标记
    bool Tick;                                                      //Tick标记
}Function_t;

extern void Function_init(void);
extern Function_t *Function_status(void);
extern void FunctionProcess(void);
extern uint8_t ManageMode(void);
extern uint8_t NormalMode(void);
extern uint8_t LockMode(void);
    
extern void Function_Sleep(void);
extern void Function_Wakup(void);

#endif
/* [] END OF FILE */
