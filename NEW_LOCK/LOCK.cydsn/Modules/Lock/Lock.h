/**
  ******************************************************************************
  * @file         lock.h
  * @brief        与lock.c对应的头文件
  * @author       Fli
  * @date         2018-06-22
  * @version      v001
  * @copyright    kaadas
  * 
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#ifndef __LOCK_H__
#define __LOCK_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "lock_hal.h"

#define LOCK_DEBUG_EN   (0)
#define DEBUG_EN        (0)

#if (LOCK_DEBUG_EN && DEBUG_EN)
#define LOCK_LOG(format, ...) printf("[File: "__FILE__"   Line:%05d]  "format"\r\n", __LINE__, ##__VA_ARGS__)  
#else
#define LOCK_LOG(format, ...)
#endif


/* 锁体状态 */
#define LOCK_MSG_LOCKED_OK      1//上锁成功 /*lock_status上锁状态*/
#define LOCK_MSG_LOCKED_ERR     3//上锁失败

#define LOCK_MSG_UNLOCK_OK      2//开锁成功 /*unlock_status开锁状态*/
#define LOCK_MSG_UNLOCK_ERR     4//开锁失败

#define LOCK_MSG_ANTI_LOCK_ON   5//启用反锁 /*anti_lock_status启用关闭反锁*/
#define LOCK_MSG_ANTI_LOCK_OFF  6//关闭反锁

#define LOCK_MSG_INPUT_ON       7//使能用户输入 /*input_status禁止使能用户输入*/
#define LOCK_MSG_INPUT_OFF      8//禁止用户输入

#define LOCK_MSG_NOT_LOCKED     9//门未上锁 /*locking_status门锁状态*/
#define LOCK_MSG_TAMPER         10//门锁被撬
#define LOCK_MSG_UNKNOWN_MECH   11//锁状态未知（机械方式操作）

#define LOCK_MSG_OPEN_DOOR      12//门已打开 /*door_status门开关状态*/
#define LOCK_MSG_CLOSE_DOOR     13//门已关闭

/* 开关锁参数 */
#define LOCK_PARAM_CLOSE        1//使用CLOSE键 开关锁 /*lock_parameter开关锁参数*/
#define LOCK_PARAM_KEYS         2//使用密钥 开关锁
#define LOCK_PARAM_AUTO         3//自动 开关锁
#define LOCK_PARAM_MECHANICAL   4//机械方式 开关锁
#define LOCK_PARAM_APP          5//APP方式开锁

/* 锁体发给主任务的消息结构 */
//typedef struct
//{
//	MsgType_enum_t type;  //消息类型
//	uint8_t msg;          //消息
//	uint8_t param;        //参数
//}LockMsg_stu_t;

/* 锁体工作模式 */
typedef enum
{
	LOCK_MODE_AUTO,
	LOCK_MODE_MANUAL,
}LockMode_enum_t; /*lock_workmode; //锁体工作模式*/

typedef struct
{
    uint8_t status;
    uint8_t param;
}Msg_t;

typedef struct
{
    Msg_t locked;//上锁状态
    Msg_t unlock;//开锁状态
    Msg_t anti_lock;//启用关闭反锁
    Msg_t input;//禁止使能用户输入
    Msg_t locking;//门锁状态
    Msg_t door;//门开关状态
      
    LockMode_enum_t lock_workmode; //锁体工作模式      
    
}LockStatus_t,*pLockStatus_t;

void Lock_Init(uint8_t opt);
void Lock_Task(void);
void Lock_SetMode(LockMode_enum_t mode);
void Lock_ProcessVerifySuceed(void);
void Lock_ProcessCloseKey(uint8_t action);
void Lock_ProcessAppOpenAndClose(uint8_t opt);
void Lock_SleepAutoLocked(void);
pLockStatus_t Lock_Status(void);//锁状态

#ifdef __cplusplus
}
#endif

#endif /*__LOCK_H__ */
