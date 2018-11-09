/**
  ******************************************************************************
  * @file         lock_hal.h
  * @brief        与lock_hal.c对应的头文件
  * @author       Fli
  * @date         2018-06-21
  * @version      v001
  * @copyright    kaadas
  *
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#ifndef __LOCK_HAL_H__
#define __LOCK_HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

	/* 电机控制参数 */
#define MOTOR_MAX_LOCKED_TIMES           3    //最大上锁次数（上锁失败后，会自动重复上锁）
#define MOTOR_MAX_ADC_VALUE              238  //电机堵转限制电流280mA 采样电阻0.5Ω 
#define MOTOR_LOCKED_MORE_TIME           50   //上锁过推时间Ms
#define MOTOR_ABNORMAL_REVERSE_GAP_TIME  100  //电机异常时，反转空隙时间Ms
#define MOTOR_REVERSE_GAP_TIME           1000 //电机正常时，反转空隙时间Ms
#define MOTOR_RUNNING_TIMEOUT            2000 //电机正常运行，超时时间Ms
#define MOTOR_BACK_TIMEOUT               1000 //电机回拖运行，超时时间Ms
#define MOTOR_ADC_DETECT_DELAY_TIME      100   //电机启动后，ADC检测延迟时间Ms
#define MOTOR_BRAKE_TIME                 30   //电机刹车时间Ms


	/* 锁体底层驱动产生的事件 */
#define LOCK_EVT_LOCK_CHANGE     0X01 //锁状态改变
#define LOCK_EVT_DOOR_CHANGE     0X02 //门状态改变
#define LOCK_EVT_LOCKED_ABNORMAL 0X04 //上锁发生异常
#define LOCK_EVT_UNLOCK_ABNORMAL 0X08 //开锁发生异常

	/* 锁状态 */
#define	LOCK_STATUS_LOCKING        0X01 //正在上锁
#define	LOCK_STATUS_LOCKED         0X02 //已上锁
#define	LOCK_STATUS_UNLOCKING      0X04 //正在开锁
#define	LOCK_STATUS_UNLOCK         0X08 //已开锁
#define	LOCK_STATUS_UNKNOWN        0X10 //锁状态未知
#define	LOCK_STATUS_LOCKED_MECH    0X20 //已上锁（机械方式）
#define	LOCK_STATUS_UNLOCK_MECH    0X40 //已开锁（机械方式）

	/* 门状态 */
#define DOOR_STATUS_OPENED       0X01//门已打开
#define DOOR_STATUS_CLOSED       0X02//门已关闭
#define DOOR_STATUS_CLOSED_LONG  0X04//门已长时间关闭（1.5s）
#define DOOR_STATUS_ABNORMAL     0X08//门状态异常--被撬开

	/* 异常状态 */
#define ABNORMAL_STA_TIMEOUT     0X01//锁体异常：超时
#define ABNORMAL_STA_AWD         0X02//锁体异常：过流（模拟看门狗）
#define ABNORMAL_STA_DOOR_OPEN   0X04//锁体异常：上锁过程中门开了


/* 回到函数类型 */
typedef void (*LockHalCallBackFunction)(uint8_t event, uint8_t param);


ErrorStatus LockHal_Init(LockHalCallBackFunction cb);
ErrorStatus LockHal_Unlock(void);
ErrorStatus LockHal_UnlockBack(void);
ErrorStatus LockHal_Locked(void);
void LockHal_Scan(void);


#ifdef __cplusplus
}
#endif

#endif /*__LOCK_HAL_H__ */
