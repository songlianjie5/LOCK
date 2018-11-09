/**
  ******************************************************************************
  * @file         lock_hal.c
  * @brief        锁体硬件层驱动程序
  * @author       Fli
  * @date         2018-06-21
  * @version      v001
  * @copyright    kaadas
  *
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
//#include "system.h"

#include "lock_hal.h"

static bool lock_run = true;

#ifdef PLATFORM

/* 传感器硬件IO定义 */
#define SENSOR_LOCKED_GPIO  OP3_INT_Read() //上锁传感器IO（主锁舌伸出到位感应）
#define SENSOR_UNLOCK_GPIO  ((LOCK_INT_Read()&(1<<0))>>0) //OP2_INT_Read() //开锁传感器IO（主锁舌缩进到位感应）
#define SENSOR_BACK_GPIO    OP1_INT_Read() //回拖传感器IO
#define SENSOR_DOOR_GPIO    ((LOCK_INT_Read()&(1<<3))>>3) //门状态检测IO（叉舌）

#endif

#define MOTOA_SET(x)    do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT27, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT27, I_MAX_1_4|(x));\
                        }while(0)  //MOTO_A脚控制

#define MOTOB_SET(x)    do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT26, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT26, I_MAX_1_4|(x));\
                        }while(0)  //MOTO_B脚控制

#define NSLEEP_SET(x)   do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT25, 0xFF);\#ifdef PLATFORM

/* 传感器硬件IO定义 */
#define SENSOR_LOCKED_GPIO  OP3_INT_Read() //上锁传感器IO（主锁舌伸出到位感应）
#define SENSOR_UNLOCK_GPIO  ((LOCK_INT_Read()&(1<<0))>>0) //OP2_INT_Read() //开锁传感器IO（主锁舌缩进到位感应）
#define SENSOR_BACK_GPIO    OP1_INT_Read() //回拖传感器IO
#define SENSOR_DOOR_GPIO    ((LOCK_INT_Read()&(1<<3))>>3) //门状态检测IO（叉舌）

#endif

#define MOTOA_SET(x)    do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT27, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT27, I_MAX_1_4|(x));\
                        }while(0)  //MOTO_A脚控制

#define MOTOB_SET(x)    do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT26, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT26, I_MAX_1_4|(x));\
                        }while(0)  //MOTO_B脚控制

#define NSLEEP_SET(x)   do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT25, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT25, I_MAX_1_4|(x));\
                          }while(0)//NSLEEP脚控制

#define T_POWER_CTRL(x) do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT6, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT6, I_MAX_1_4|(x));\
                          }while(0)//锁体电源控制

/* 电机动作 */
#define LOCK_HAL_MOTOR_RIGHT()  {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(LOW);FL3225_DataUpdate();} //左转（上锁）
#define LOCK_HAL_MOTOR_LEFT() {NSLEEP_SET(HIGH);MOTOB_SET(LOW);MOTOA_SET(HIGH);FL3225_DataUpdate();} //右转（开锁）
#define LOCK_HAL_MOTOR_STOP()  {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(HIGH);FL3225_DataUpdate();} //停止
#define LOCK_HAL_MOTOR_BRAKE() {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(HIGH);FL3225_DataUpdate();} //刹车

#define Power_LockSensorControl(endis)  {T_POWER_CTRL(endis);FL3225_DataUpdate();}    //锁体电源开关
                            I2C_WriteReg(FL3235_REG_CTRL_OUT25, I_MAX_1_4|(x));\
                          }while(0)//NSLEEP脚控制

#define T_POWER_CTRL(x) do{\
                            I2C_WriteReg(FL3235_REG_PWM_OUT6, 0xFF);\
                            I2C_WriteReg(FL3235_REG_CTRL_OUT6, I_MAX_1_4|(x));\
                          }while(0)//锁体电源控制

/* 电机动作 */
#define LOCK_HAL_MOTOR_RIGHT()  {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(LOW);FL3225_DataUpdate();} //左转（上锁）
#define LOCK_HAL_MOTOR_LEFT() {NSLEEP_SET(HIGH);MOTOB_SET(LOW);MOTOA_SET(HIGH);FL3225_DataUpdate();} //右转（开锁）
#define LOCK_HAL_MOTOR_STOP()  {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(HIGH);FL3225_DataUpdate();} //停止
#define LOCK_HAL_MOTOR_BRAKE() {NSLEEP_SET(HIGH);MOTOB_SET(HIGH);MOTOA_SET(HIGH);FL3225_DataUpdate();} //刹车

#define Power_LockSensorControl(endis)  {T_POWER_CTRL(endis);FL3225_DataUpdate();}    //锁体电源开关


/* 传感器类型 */
typedef enum
{
	SENSOR_TYPE_DOOR = 0,    //叉舌传感器（门状态检测）
	SENSOR_TYPE_LOCKED = 1,  //上锁传感器（主锁舌伸出传感器）
	SENSOR_TYPE_UNLOCK = 2,  //开锁传感器（主锁舌缩进传感器）
	SENSOR_TYPE_BACK = 3,    //回拖传感器
	SENSOR_MAX_QTY,
} SensorType_enum_t;

/* 传感器状态 */
typedef enum
{
	SENSOR_STA_RELEASE,     //状态为松开（未遮挡）
	SENSOR_STA_PRESS,       //状态为按下（遮挡）
	SENSOR_STA_LONG_RELEASE,//状态为长松开（1.5s）
	SENSOR_STA_LONG_PRESS,  //状态为长按下
	SENSOR_STA_UNKNOWN,     //状态未知
} SensorStatus_enum_t;

/* 电机当前工作状态 */
__IO static enum
{
	MOTOR_STA_UNLOCKING,       //正在开锁
	MOTOR_STA_UNLOCK_BACK,     //正在开锁回拖
	MOTOR_STA_WAIT_UNLOCK_BACK,//正在等待开锁回拖
	MOTOR_STA_UNLOCKED,    //已开锁
	MOTOR_STA_LOCKING,     //正在上锁
	MOTOR_STA_LOCKED_BACK, //正在上锁回拖
	MOTOR_STA_LOCKED,      //已上锁
	MOTOR_STA_UNKNOWN,     //未知状态
} motorStatus;

/* 锁体当前工作状态 */
__IO static enum
{
	LOCK_STA_UNLOCKING,   //正在开锁（锁体正忙）
	LOCK_STA_LOCKING,     //正在上锁（锁体正忙）
	LOCK_STA_IDLE,        //未执行任何动作
    LOCK_STA_INIT,        //初始状态
} lockStatus;

__IO static SensorStatus_enum_t sensorRealStatus[(int)SENSOR_MAX_QTY]; //传感器实时状态
__IO static LockHalCallBackFunction pFunctionCb; //回调
__IO static uint8_t repeatLockedCnt;             //重复上锁计数（上锁失败会重复上锁）
__IO static uint8_t awdEventFlag;                //AWD事件标志
__IO static uint8_t unlockOkFlag;                //开锁成功标志
__IO static uint8_t abnormalEvent;               //锁体异常事件
__IO static uint8_t firstFlag;                   //锁体首次上电校准标记

/* 本地函数声明 */
static uint8_t LockHal_MotorUnlock(void);
static uint8_t LockHal_MotorLocked(void);
static void LockHal_MotorRepeatLocked(void);
static void LockHal_AbnormalProcess(void);


/**
  * @brief  模拟看门狗回调函数
  *
  * @note   开锁和上锁时会启动模拟看门狗
	*         产生模拟看门狗事件，自动执行该函数
  */
//static void LockHal_AdcDogCB(void)
//{
//	if (awdEventFlag == 0)
//	{
//		awdEventFlag = 1; //标记电机过流
//	}
//	else
//	{
//		AdcHal_StopAdcDog();//关闭模拟看门狗
//	}
//}

/**
  * @brief  启动模拟看门狗
  *
  * @note   PC5 -- ADC_IN15
  */
static void LockHal_AdcDogStart(void)
{
//	if (AdcHal_InitChangeAndDog(1,LockHal_AdcDogCB) != SUCCESS)
//	{
//		LockEvt_Create(LockHal_AdcDogStart, 10);//10ms后再次启动
//		return;
//	}
    uint16_t adcval = 0;
    
    awdEventFlag = 0;//过流标记
    
    if (AdcHal_InitChange(1))
    {
        adcval = AdcHal_ReadResult(1);
        
        if (adcval > MOTOR_MAX_ADC_VALUE)
        {
            awdEventFlag = 1; //过流
        }
        else
        {
            awdEventFlag = 0;    
        }
    }
    LockEvt_Create(LockHal_AdcDogStart, 10);//10ms后再次启动
}

/**
  * @brief  停止电机
  *
  * @note
  */
static void LockHal_MotorStop(void)
{
	LOCK_HAL_MOTOR_STOP(); //停电机
//	AdcHal_StopAdcDog();   //关闭模拟看门狗

	if (motorStatus == MOTOR_STA_UNLOCK_BACK)
	{
		/* 清除事件表，当前开锁流程结束 */
		LockEvt_Clear();
		motorStatus = (unlockOkFlag == 1) ? MOTOR_STA_UNLOCKED : MOTOR_STA_UNKNOWN;

		if (lockStatus == LOCK_STA_UNLOCKING && unlockOkFlag == 0)
		{
			/* 回调通知：开锁失败 */
			pFunctionCb(LOCK_EVT_UNLOCK_ABNORMAL, abnormalEvent);
			if (sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_LONG_PRESS)
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED);
			}
			else
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNKNOWN);
			}
		}
		else if (lockStatus == LOCK_STA_LOCKING)
		{
			if (unlockOkFlag == 1)
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);
			}
			else
			{
				pFunctionCb(LOCK_EVT_LOCKED_ABNORMAL, abnormalEvent);//回调通知：上锁失败
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNKNOWN);
			}
		}
		lockStatus = LOCK_STA_IDLE;
	}
	else if (motorStatus == MOTOR_STA_LOCKED_BACK)
	{
		/* 清除事件表，当前上锁流程结束 */
		LockEvt_Clear();

		/* 判断伸出和叉舌传感器状态 */
		if ( (sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_LONG_PRESS)
		        && (sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_RELEASE || sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_LONG_RELEASE) )
		{
			motorStatus = MOTOR_STA_LOCKED;
		}
		else
		{
			motorStatus = MOTOR_STA_UNKNOWN;
		}

		if (lockStatus == LOCK_STA_LOCKING)
		{
			if (motorStatus == MOTOR_STA_LOCKED)//上锁成功
			{
				lockStatus = LOCK_STA_IDLE;
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED);//回调通知：锁状态改变，上锁成功
			}
			else
			{
				pFunctionCb(LOCK_EVT_LOCKED_ABNORMAL, abnormalEvent);//回调通知：上锁失败
				if (sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_LONG_PRESS)
				{
					/* 上锁失败，锁舌退回去 */
					if (LockHal_MotorUnlock() == 0)
					{
						lockStatus = LOCK_STA_IDLE;
						pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);
					}
				}
				else
				{
					LockEvt_Create(LockHal_MotorRepeatLocked, MOTOR_REVERSE_GAP_TIME);
				}
			}
		}
	}
}

/**
  * @brief  停止回拖
  *
  * @note   1.正常调用：回拖传感器到位会调用该函数
  *         2.超时调用：回拖超时会调用该函数
  */
static void LockHal_MotorBackStop(void)
{
	LOCK_HAL_MOTOR_BRAKE();//立即刹车

	LockEvt_Clear();
	LockEvt_Create(LockHal_MotorStop, MOTOR_BRAKE_TIME);//30ms后停止电机
}

/**
  * @brief  启动电机回拖
  *
  * @note
  */
static void LockHal_MotorBackStart(void)
{
	switch (motorStatus)
	{
		/* 电机正在执行上锁 */
	case MOTOR_STA_LOCKING:
		motorStatus = MOTOR_STA_LOCKED_BACK;
		LOCK_HAL_MOTOR_RIGHT();
		LockEvt_Clear();
		LockEvt_Create(LockHal_MotorBackStop, MOTOR_BACK_TIMEOUT);//启动回拖超时
		break;

		/* 电机正在执行开锁 */
	case MOTOR_STA_UNLOCKING:
		if ( (sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_LONG_PRESS) )
		{
			unlockOkFlag = 1;
		}

		if (lockStatus == LOCK_STA_UNLOCKING && unlockOkFlag == 1)
		{
			/* 开锁成功：回调通知，开锁流程结束 */
			LockEvt_Clear();
			lockStatus = LOCK_STA_IDLE;
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);

			/* 如果门没有打开：设置电机状态为等待开锁回拖 */
			if ((sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_RELEASE || sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_LONG_RELEASE))
			{
				motorStatus = MOTOR_STA_WAIT_UNLOCK_BACK;
				break;
			}
		}

		/* 电机正在等待开锁回拖 */
	case MOTOR_STA_WAIT_UNLOCK_BACK:
		motorStatus = MOTOR_STA_UNLOCK_BACK;
		LOCK_HAL_MOTOR_LEFT();
		LockEvt_Clear();
		LockEvt_Create(LockHal_MotorBackStop, MOTOR_BACK_TIMEOUT);//启动回拖超时
		break;

	default:
		break;
	}
}

/**
  * @brief  锁体异常处理
  *
  * @note  异常事件
  *         ABNORMAL_STA_DOOR_OPEN 0X04（锁体异常：上锁过程中门开了）
  *         ABNORMAL_STA_AWD       0X02（锁体异常：过流-模拟看门狗）
  *         ABNORMAL_STA_TIMEOUT   0X01（上锁、开锁超时）
  */
static void LockHal_AbnormalProcess(void)
{
	LockHal_MotorStop();
	LockEvt_Clear();
	LockEvt_Create(LockHal_MotorBackStart, MOTOR_ABNORMAL_REVERSE_GAP_TIME);
}

/**
  * @brief  检测锁体状态
  *
  * @note   第一次上电需要读取锁体状态并通知上层
  *         必须等传感器状态稳定检测到后（上电20ms后），再调用
  */
static void LockHal_MotorCheck(void)
{
	if (sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_RELEASE)
	{
		pFunctionCb(LOCK_EVT_DOOR_CHANGE, DOOR_STATUS_CLOSED);
	}
    
    if (sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_PRESS
	        || sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_LONG_PRESS)
	{
		if ( sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_PRESS
		        || sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_LONG_PRESS)
		{
			motorStatus = MOTOR_STA_LOCKED;//设置状态为已上锁
		}
		else if ( sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_PRESS
		          || sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_LONG_PRESS)
		{
//			motorStatus = MOTOR_STA_WAIT_UNLOCK_BACK;//设置状态为已开锁
            motorStatus = MOTOR_STA_UNLOCKED;//设置状态为已开锁
		}
	}

	if (motorStatus == MOTOR_STA_LOCKED)
	{
		pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED_MECH);//回调通知
		firstFlag = 0XFF;
	}
//	else if (motorStatus == MOTOR_STA_WAIT_UNLOCK_BACK)
    else if (motorStatus == MOTOR_STA_UNLOCKED)
	{
		pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK_MECH);//回调通知
		firstFlag = 0XFF;
	}
	else
	{
		firstFlag = 0;//标记需要校准锁体位置
		pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNKNOWN);//回调通知
	}
    lockStatus = LOCK_STA_IDLE;
}

/**
  * @brief  启动电机开锁
  * @note
  *
  * @return 当前电机位置已处于正常的开锁状态返回0，否则返回1
  */
static uint8_t LockHal_MotorUnlock(void)
{
	LockEvt_Clear();

	if ( sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_PRESS
	        || sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_LONG_PRESS)
	{
		if (sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_PRESS
		        || sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_LONG_PRESS)
		{
			motorStatus = MOTOR_STA_UNLOCKED;//已开锁
			return 0;
		}
	}

	if (firstFlag == 0)
	{
		firstFlag = 1;
	}
	else
	{
		firstFlag = 0XFF;
	}

	unlockOkFlag = 0;
	motorStatus = MOTOR_STA_UNLOCKING;  //设置状态为正在开锁
	LOCK_HAL_MOTOR_RIGHT();             //启动电机开锁

	LockEvt_Create(LockHal_AdcDogStart, MOTOR_ADC_DETECT_DELAY_TIME);//100ms后启动电流监控

	abnormalEvent = ABNORMAL_STA_TIMEOUT;
    
	LockEvt_Create(LockHal_AbnormalProcess, MOTOR_RUNNING_TIMEOUT);  //设置2s后执行异常处理
	return 1;
}

/**
  * @brief  启动电机上锁
  * @note
  *
  * @return 当前电机位置已处于正常的上锁状态返回0，否则返回1
  */
static uint8_t LockHal_MotorLocked(void)
{
	LockEvt_Clear();

	if ( sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_PRESS
	        || sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_LONG_PRESS)
	{
		if (sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_PRESS
		        || sensorRealStatus[(int)SENSOR_TYPE_BACK] == SENSOR_STA_LONG_PRESS)
		{
			motorStatus = MOTOR_STA_LOCKED;//设置状态为已上锁
			return 0;
		}
	}

	if (firstFlag == 0)
	{
		firstFlag = 1;
	}
	else
	{
		firstFlag = 0XFF;
	}

	motorStatus = MOTOR_STA_LOCKING;  //设置状态为正在上锁
	LOCK_HAL_MOTOR_LEFT();            //启动电机上锁

	LockEvt_Create(LockHal_AdcDogStart, MOTOR_ADC_DETECT_DELAY_TIME);//100ms后启动电流监控

	abnormalEvent = ABNORMAL_STA_TIMEOUT;
	LockEvt_Create(LockHal_AbnormalProcess, MOTOR_RUNNING_TIMEOUT);  //设置2s后执行异常处理
	return 1;
}

/**
  * @brief  重新上锁
  *
  * @note   上锁异常后，会将电机回到开锁状态，再调用该函数重新上锁
  *         门未关上则放弃上锁
  */
static void LockHal_MotorRepeatLocked(void)
{
	if ( (++repeatLockedCnt < MOTOR_MAX_LOCKED_TIMES)
	        && (sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_RELEASE || sensorRealStatus[(int)SENSOR_TYPE_DOOR] == SENSOR_STA_LONG_RELEASE) )
	{
		if (LockHal_MotorLocked() == 0)
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED);//回调通知：锁状态改变，已上锁
			lockStatus = LOCK_STA_IDLE;
		}
	}
	else
	{
		/* 上锁流程结束 */
		if (sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_UNLOCK] == SENSOR_STA_LONG_PRESS)
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);//回调通知：锁状态改变，已开锁
		}
		else
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNKNOWN);//回调通知：锁状态改变，状态未知
		}
		lockStatus = LOCK_STA_IDLE;
	}
}

/**
  * @brief  传感器状态改变处理
  *
  * @note   1.伸出传感器检测到下降沿：当前为正在上锁，50ms后停止电机并回拖
  *         2.缩进传感器检测到下降沿：当前为正在开锁，立即停止电机并回拖
  *         3.叉舌传感器检测到下降沿（门开了）：
  *           当前正在上锁：执行异常处理
  *           当前已上锁：门被撬，报警
  *         4.叉舌传感器检测到长时间松开（门关并保持1.5s）：回调通知
  *         5.回拖传感器检测到下降沿：当前正在开锁回拖或关锁回拖，立即停止回拖
  *
  * @param  sensorType：传感器类型
  * @param  sensorSta：传感器当前状态
  */
static void LockHal_SensorChangeProcess(SensorType_enum_t sensorType, SensorStatus_enum_t sensorSta)
{
	if (sensorType == SENSOR_TYPE_LOCKED)
	{
		if (sensorSta == SENSOR_STA_PRESS)
		{
			if (motorStatus == MOTOR_STA_LOCKING)
			{
				/* 上锁到位了 */
				if (firstFlag != 1)//如果电机处于校准状态下：到位了也不停电机，等待过流再停
				{
					LockEvt_Create(LockHal_MotorStop, MOTOR_LOCKED_MORE_TIME);//过推---50ms后停止电机
					LockEvt_Create(LockHal_MotorBackStart, MOTOR_REVERSE_GAP_TIME + MOTOR_LOCKED_MORE_TIME);//1050ms后回拖
				}
			}
			else if (lockStatus == LOCK_STA_IDLE)
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED_MECH);//机械方式上锁
				motorStatus = MOTOR_STA_LOCKED;
			}
		}
		else if (sensorSta == SENSOR_STA_RELEASE)
		{
			if (lockStatus == LOCK_STA_IDLE && motorStatus == MOTOR_STA_LOCKED)
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNKNOWN);//机械方式操作：锁状态未知
				motorStatus = MOTOR_STA_UNKNOWN;
			}
		}
	}
	else if (sensorType == SENSOR_TYPE_UNLOCK && sensorSta == SENSOR_STA_PRESS)
	{
		if (motorStatus == MOTOR_STA_UNLOCKING)
		{
			/* 开锁到位了 */
			if (firstFlag != 1)//如果电机处于校准状态下：到位了也不停电机，等待过流再停
			{
				LockHal_MotorStop();//停止电机
				LockEvt_Create(LockHal_MotorBackStart, MOTOR_REVERSE_GAP_TIME);//1000ms后回拖
			}
		}
		else if (lockStatus == LOCK_STA_IDLE)
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK_MECH); //机械方式开锁
			motorStatus = MOTOR_STA_UNLOCKED;
		}
	}
	else if (sensorType == SENSOR_TYPE_DOOR) //门状态改变（叉舌动作）
	{
		switch (sensorSta)
		{
		case SENSOR_STA_PRESS:
			if (motorStatus == MOTOR_STA_LOCKING || motorStatus == MOTOR_STA_LOCKED_BACK)
			{
				abnormalEvent = ABNORMAL_STA_DOOR_OPEN;
				if (motorStatus == MOTOR_STA_LOCKING)
				{
					LockHal_AbnormalProcess();//执行异常处理（上锁过程中门开了）
				}
			}
			else if (motorStatus == MOTOR_STA_WAIT_UNLOCK_BACK)
			{
				LockHal_MotorBackStart();//门开了，执行开锁回拖
			}
			else if (sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_PRESS || sensorRealStatus[(int)SENSOR_TYPE_LOCKED] == SENSOR_STA_LONG_PRESS)
			{
				if (lockStatus == LOCK_STA_IDLE)
				{
					pFunctionCb(LOCK_EVT_DOOR_CHANGE, DOOR_STATUS_ABNORMAL);//门被撬
					break;
				}
			}
			pFunctionCb(LOCK_EVT_DOOR_CHANGE, DOOR_STATUS_OPENED);
			break;

		case SENSOR_STA_RELEASE:
			pFunctionCb(LOCK_EVT_DOOR_CHANGE, DOOR_STATUS_CLOSED);
			break;
		case SENSOR_STA_LONG_RELEASE:
			pFunctionCb(LOCK_EVT_DOOR_CHANGE, DOOR_STATUS_CLOSED_LONG);
			break;
		default:
			break;
		}
	}
	else if (sensorType == SENSOR_TYPE_BACK && sensorSta == SENSOR_STA_PRESS)
	{
		if (motorStatus == MOTOR_STA_UNLOCK_BACK || motorStatus == MOTOR_STA_LOCKED_BACK)
		{
			LockHal_MotorBackStop();//停止回拖
		}
	}
}

/**
  * @brief  锁体传感器扫描，该函数必须使用定时器中断每间隔2ms调用一次
  *
  * @note   读取传感器电平状态，去抖并刷新状态
	*         根据传感器状态变化，进行相应的电机控制
  */
static void LockHal_SensorScan(void)
{
	static uint8_t  gpioBuf[(int)SENSOR_MAX_QTY] = {0};
	static uint16_t timing[(int)SENSOR_MAX_QTY] = {0};//计时器
	uint8_t i;

	for (i=0; i<(int)SENSOR_MAX_QTY; i++)
	{
		gpioBuf[i] <<= 1;
		switch (i)
		{
		case SENSOR_TYPE_LOCKED:
			gpioBuf[i] |= SENSOR_LOCKED_GPIO;
			break;
		case SENSOR_TYPE_UNLOCK:
			gpioBuf[i] |= SENSOR_UNLOCK_GPIO;
			break;
		case SENSOR_TYPE_BACK:
			gpioBuf[i] |= SENSOR_BACK_GPIO;
			break;
		case SENSOR_TYPE_DOOR:
			gpioBuf[i] |= SENSOR_DOOR_GPIO;
			break;
		}

		if (gpioBuf[i] == 0)
		{
			/* 硬件IO稳定低电平 */
//      if (sensorRealStatus[i] == SENSOR_STA_UNKNOWN)
			if (sensorRealStatus[i] != SENSOR_STA_PRESS && sensorRealStatus[i] != SENSOR_STA_LONG_PRESS)
			{
				timing[i] = 0;
				sensorRealStatus[i] = SENSOR_STA_PRESS;//标记为按下
				LockHal_SensorChangeProcess((SensorType_enum_t)i, sensorRealStatus[i]);
			}

			/* 按下状态计时 */
			if (timing[i] < (1500/TIME_BASIC_MS))
			{
				timing[i]++;
				if (timing[i] >= (1500/TIME_BASIC_MS))  //长按时间设置为750 * 2 = 1500MS
				{
					sensorRealStatus[i] = SENSOR_STA_LONG_PRESS;//标记为长按
					LockHal_SensorChangeProcess((SensorType_enum_t)i, sensorRealStatus[i]);
				}
			}
		}
		else if (gpioBuf[i] == 0XFF)
		{
			/* 硬件IO稳定高电平 */
//      if (sensorRealStatus[i] == SENSOR_STA_UNKNOWN)
			if (sensorRealStatus[i] != SENSOR_STA_RELEASE && sensorRealStatus[i] != SENSOR_STA_LONG_RELEASE)
			{
				timing[i] = 0;
				sensorRealStatus[i] = SENSOR_STA_RELEASE;//标记为松开
				LockHal_SensorChangeProcess((SensorType_enum_t)i, sensorRealStatus[i]);
			}

			/* 松开状态计时 */
			if (timing[i] < 1500/TIME_BASIC_MS)
			{
				timing[i]++;
				if (timing[i] == 1500/TIME_BASIC_MS)  //长按时间设置为750 * 2 = 1500MS
				{
					sensorRealStatus[i] = SENSOR_STA_LONG_RELEASE;//标记为长松开
					LockHal_SensorChangeProcess((SensorType_enum_t)i, sensorRealStatus[i]);
				}
			}
		}
		else
		{
			/* 硬件IO状态未知 */
//			sensorRealStatus[i] = SENSOR_STA_UNKNOWN;
		}
	}
}

/**
  * @brief  开锁接口
  *
  * @note   调用该函数，执行开锁流程，开锁结果通过回调通知应用层
  * @return 当前电机正忙：返回ERROR，否则返回SUCCESS
  */
ErrorStatus LockHal_Unlock(void)
{
	ErrorStatus result = ERROR;

//	TIM4->CR1 &= ~1;//暂停定时器，不要再中断
    lock_run = false;

	if (lockStatus == LOCK_STA_IDLE)
	{
		if (motorStatus != MOTOR_STA_WAIT_UNLOCK_BACK)
		{
			if (LockHal_MotorUnlock() != 0)
			{
				lockStatus = LOCK_STA_UNLOCKING;//标记当前正在执行开锁
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCKING);//回调通知
                result = SUCCESS;
			}
			else
			{
				pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);  //回调通知
			}
		}
		else
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_UNLOCK);  //回调通知
		}
//		result = SUCCESS;
	}

//	TIM4->CR1 |= 1;//启动定时器
    lock_run = true;
	return result;
}

/**
  * @brief  开锁回拖接口
  *
  * @note   只有当：电机处于开锁状态下，并且未回拖，执行该函数才有效
  * @return 正常返回SUCCESS, 失败返回ERROR
  */
ErrorStatus LockHal_UnlockBack(void)
{
	ErrorStatus result = ERROR;

//	TIM4->CR1 &= ~1;//暂停定时器，不要再中断
    lock_run = false;
	if (motorStatus == MOTOR_STA_WAIT_UNLOCK_BACK)
	{
		LockHal_MotorBackStart();
		result = SUCCESS;
	}
//	TIM4->CR1 |= 1;//启动定时器
    lock_run = true;
	return result;
}

/**
  * @brief  上锁接口
  *
  * @note   调用该函数，执行上锁流程，上锁结果通过回调通知应用层
  * @return 当前电机正忙：返回ERROR，否则返回SUCCESS
  */
ErrorStatus LockHal_Locked(void)
{
	ErrorStatus result = ERROR;

//	TIM4->CR1 &= ~1;//暂停定时器，不要再中断
    
    lock_run = false;
    
	if (lockStatus == LOCK_STA_IDLE)
	{
		repeatLockedCnt = 0;              //清零重复上锁计数
		if (LockHal_MotorLocked() != 0)
		{
			lockStatus = LOCK_STA_LOCKING;  //标记当前正在执行上锁
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKING);//回调通知
		}
		else
		{
			pFunctionCb(LOCK_EVT_LOCK_CHANGE, LOCK_STATUS_LOCKED);//回调通知
		}
		result = SUCCESS;
	}

//	TIM4->CR1 |= 1;//启动定时器
    lock_run = true;
    
	return result;
}

/**
  * @brief  锁体硬件层初始化
  *
  * @note   传感器GPIO
	*         PC0 -- 叉舌感应
	*         PC1 -- 回拖感应
	*         PC2 -- 主锁舌缩进感应
	*         PC3 -- 主锁舌伸出感应
	*         PC5 -- 电机电流监测（ADC_IN15）
  *
  * @param  cb：回调函数
  * @return 参数错误返回ERROR，正常返回SUCCESS
  */
ErrorStatus LockHal_Init(LockHalCallBackFunction cb)
{
	static uint8_t resetFlag;
	uint8_t i;

	/* 设置回调 */
	if (cb == NULL)
	{
		return ERROR;
	}
	pFunctionCb = cb;

	/* 第一次上电：执行锁体复位 */
	if (resetFlag == 0)
	{
		resetFlag = 1;
		LockEvt_Create(LockHal_MotorCheck, 30);

		/* 状态初始化 */
		for (i=0; i<(int)SENSOR_MAX_QTY; i++)
		{
			sensorRealStatus[i] = SENSOR_STA_UNKNOWN;
		}
        sensorRealStatus[SENSOR_TYPE_DOOR] = SENSOR_STA_RELEASE;
		motorStatus = MOTOR_STA_UNKNOWN;
//		lockStatus = LOCK_STA_IDLE;
        lockStatus = LOCK_STA_INIT;
	}

	/* 传感器电源关闭 */
	Power_LockSensorControl(ENABLE);

	return SUCCESS;
}

/**
  * @brief  timer4中断服务函数
  *
  * @note   周期2ms
  */
void LockHal_Scan(void)
{
    if (lock_run)
    {
    //	TIM4->SR = 0;

    /* 扫描传感器 */
    //	Power_LockSensorControl(ENABLE);
    	LockHal_SensorScan();
    //	Power_LockSensorControl(DISABLE);

    	/* 延时事件事件更新 */
    	LockEvt_TimeUpdate();

    	/* 电机过流处理 */
    	if (awdEventFlag == 1)
    	{
    		awdEventFlag = 0;
            
//            LockEvt_Clear();

    		abnormalEvent = ABNORMAL_STA_AWD;
            
    		LockHal_AbnormalProcess();//执行异常处理（电机过流）
    	}
    }
}

