#include "Lock.h"


/* 锁当前工作标志 */
typedef enum
{
	WF_APP_UNLOCK,       //正在执行app开锁
	WF_APP_LOCKED,       //正在执行app上锁
	WF_AUTO_UNLOCK,      //正在执行自动开锁
	WF_AUTO_LOCKED,      //正在执行自动上锁
	WF_KEYS_LOCKED,      //正在执行验证密钥上锁
	WF_KEYS_UNLOCK,      //正在执行密钥验证开锁
	WF_CLOSE_KEY_LOCKED, //正在执行CLOSE键上锁
	WF_CLOSE_KEY_UNLOCK, //正在执行CLOSE键开锁
	WF_ANTI_LOCK,        //正在执行反锁
	WF_IDLE,             //空闲
} WorkFlag_enum_t;

static LockStatus_t Lock_Stat;

static WorkFlag_enum_t lockWorkFlag;

static uint8_t lockStatus;       //锁状态（正在上锁、已上锁、正在开锁、已开锁、锁状态未知）
static uint8_t doorStatus;       //门状态（门已打开、门已关闭、门已长时间关闭、门状态异常--被撬开）
static uint8_t lockedAbnormal;   //上锁异常（超时、过流、门开了）
static uint8_t unlockAbnormal;   //开锁异常（超时、过流）
static uint8_t autoUnlockFlag;   //自动开锁标记
static FlagStatus lockedFailedFlag;//上锁失败标记

//static void Lock_SendMsgToApp(uint8_t msg, uint8_t param);

/**
  * @brief  设置当前工作标志，使能或禁能用户输入
  * @note
  *
  * @param  sta：使能ENABLE   禁止DISABLE
  * @param  flag：当前工作标志
  */
static void Lock_SetWorkFlag(FunctionalState sta, WorkFlag_enum_t flag)
{
	if (sta == ENABLE)
	{
		/* 使能用户输入 */
//		Lock_SendMsgToApp(LOCK_MSG_INPUT_ON, 0);
		Lock_Stat.input.status = LOCK_MSG_INPUT_ON;
        Lock_Stat.input.param = 0;
	}
	else
	{
		/* 禁止用户输入 */
//		Lock_SendMsgToApp(LOCK_MSG_INPUT_OFF, 0);
		Lock_Stat.input.status = LOCK_MSG_INPUT_OFF;
        Lock_Stat.input.param = 0;
	}
	lockWorkFlag = flag;
}

/**
  * @brief  发送消息给主任务
  * @note
  *
  * @param  msg：消息
  * @param  param：消息对应的参数
  */
//static void Lock_SendMsgToApp(uint8_t msg, uint8_t param)
//{
//	static LockMsg_stu_t lockToAppMsg[10];
//	static uint8_t cnt;
//
//	lockToAppMsg[cnt].type = MSG_TYPE_LOCK;
//	lockToAppMsg[cnt].msg = msg;
//	lockToAppMsg[cnt].param = param;
//	System_SendQmsg(pQmsgApp, &lockToAppMsg[cnt]);
//	if (++cnt >= 10)
//	{
//		cnt = 0;
//	}
//}

/**
  * @brief  锁体 底层回调
  * @note   注意：不要在该函数直接处理事件，记录下事件即可
  *
  * @param  event：事件
	*         LOCK_EVT_STATUS_CHANGE：锁状态改变
	* 				LOCK_DOOR_STATUS_CHANGE：门状态改变
	* 				LOCK_EVT_ABNORMAL：锁体产生异常
	*
  * @param  param：对应事件的参数
  */
static  void Lock_HalCallBack(uint8_t event, uint8_t param)
{
	switch (event)
	{
	case LOCK_EVT_LOCK_CHANGE:
		lockStatus = param;
		break;
	case LOCK_EVT_DOOR_CHANGE:
		doorStatus = param | 0X80;
		break;
	case LOCK_EVT_LOCKED_ABNORMAL:
		lockedAbnormal = param | 0X80;
		break;
	case LOCK_EVT_UNLOCK_ABNORMAL:
		unlockAbnormal = param | 0X80;
		break;
	}
	//通知APP Fli   ？？？
}

/**
  * @brief  处理门状态改变
  * @note
  *
  * @param  sta：当前门的状态
  */
static void Lock_DoorChange(uint8_t sta)
{
	lockedFailedFlag = RESET;
	if (sta == DOOR_STATUS_ABNORMAL)
	{
		/* 门被撬开 */
//		Lock_SendMsgToApp(LOCK_MSG_TAMPER, 0);
		Lock_Stat.locking.status = LOCK_MSG_TAMPER;
        Lock_Stat.locking.param = 0;
//		Lock_SendMsgToApp(LOCK_MSG_ANTI_LOCK_OFF, 0);
		Lock_Stat.anti_lock.status = LOCK_MSG_ANTI_LOCK_OFF;
        Lock_Stat.anti_lock.param = 0;
	}
	else if (sta == DOOR_STATUS_OPENED)
	{
		/* 门已打开 */
		Lock_SetWorkFlag(ENABLE, lockWorkFlag);
//		Lock_SendMsgToApp(LOCK_MSG_OPEN_DOOR, 0);
		Lock_Stat.door.status = LOCK_MSG_OPEN_DOOR;
        Lock_Stat.door.param = 0;
	}
	else if (sta == DOOR_STATUS_CLOSED)
	{
		/* 门已关闭 */
		if (Lock_Stat.lock_workmode == LOCK_MODE_AUTO)
		{
			Lock_SetWorkFlag(DISABLE, lockWorkFlag);
		}
//		Lock_SendMsgToApp(LOCK_MSG_CLOSE_DOOR, 0);
		Lock_Stat.door.status = LOCK_MSG_CLOSE_DOOR;
        Lock_Stat.door.param = 0;
	}
	else if (sta == DOOR_STATUS_CLOSED_LONG)
	{
		/* 门已关闭1.5s+ */
		if (Lock_Stat.lock_workmode == LOCK_MODE_AUTO)//自动模式
		{
			if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Locked();
				Lock_SetWorkFlag(DISABLE, WF_AUTO_LOCKED);//设置正在自动上锁
			}
//			else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH)
//			{
//				Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_AUTO);
//			}
		}
		else if (Lock_Stat.lock_workmode == LOCK_MODE_MANUAL)//手动模式
		{
			if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
////				Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 0);
                Lock_Stat.locking.status = LOCK_MSG_NOT_LOCKED;//门未上锁
                Lock_Stat.locking.param = 0;
			}
		}
	}
}

/**
  * @brief  处理上锁成功
  *
  * @note
  */
static void Lock_LockedSucced(void)
{
	lockedFailedFlag = RESET;
	Lock_SetWorkFlag(ENABLE, lockWorkFlag);//使能用户输入

	if (doorStatus == DOOR_STATUS_OPENED || doorStatus == DOOR_STATUS_ABNORMAL)
	{
		/* 上锁成功，如果门是开的，则提示防撬报警（锁体复位时，会有这种情况） */
		doorStatus = DOOR_STATUS_ABNORMAL | 0X80;
		return;
	}

	switch (lockWorkFlag)
	{
	case WF_ANTI_LOCK:
//        Lock_SendMsgToApp(LOCK_MSG_ANTI_LOCK_ON, 0)
		Lock_Stat.anti_lock.status = LOCK_MSG_ANTI_LOCK_ON;
        Lock_Stat.anti_lock.param = 0;
//        Lock_Stat.lock_parameter = 0;
		break;
	case WF_AUTO_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_AUTO)
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
		Lock_Stat.locked.param = LOCK_PARAM_AUTO;
		break; //正在执行自动上锁
	case WF_KEYS_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_KEYS);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
		Lock_Stat.locked.param = LOCK_PARAM_AUTO;
		break; //正在执行验证密钥上锁
	case WF_CLOSE_KEY_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_CLOSE);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
		Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
		break;//正在执行CLOSE键上锁
	case WF_APP_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_APP);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
		Lock_Stat.locked.param = LOCK_PARAM_APP;
		break;  //正在执行app上锁
	default:
		break;
	}
}

/**
  * @brief  处理开锁成功
  *
  * @note
  */
static void Lock_UnlockSucced(void)
{
	lockedFailedFlag = RESET;
	switch (lockWorkFlag)
	{
	case WF_AUTO_UNLOCK:
//        Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_AUTO);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
		Lock_Stat.unlock.param = LOCK_PARAM_AUTO;
		break; //正在执行自动开锁
	case WF_KEYS_UNLOCK:
//        Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_KEYS);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
		Lock_Stat.unlock.param = LOCK_PARAM_KEYS;
		break; //正在执行密钥验证开锁
	case WF_CLOSE_KEY_UNLOCK:
//        Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_CLOSE);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
		Lock_Stat.unlock.param = LOCK_PARAM_CLOSE;
		break;//正在执行CLOSE键开锁
	case WF_APP_UNLOCK:
//        Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_APP);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
		Lock_Stat.unlock.param = LOCK_PARAM_APP;
		break;  //正在执行app开锁
	default:
		break;
	}
//	Lock_SendMsgToApp(LOCK_MSG_ANTI_LOCK_OFF, 0);//通知应用层，取消反锁
	Lock_Stat.anti_lock.status = LOCK_MSG_ANTI_LOCK_OFF;
    Lock_Stat.anti_lock.param = 0;
//    Lock_Stat.lock_parameter = 0;
	Lock_SetWorkFlag(ENABLE, WF_IDLE);//使能用户输入
}

/**
  * @brief  处理上锁失败
  *
  * @note
  */
static void Lock_LockedFailed(void)
{
	Lock_SetWorkFlag(ENABLE, lockWorkFlag);//使能用户输入

	if (lockedAbnormal == ABNORMAL_STA_DOOR_OPEN)
	{
		return;//上锁过程中门开了，不当做上锁失败处理
	}

	lockedFailedFlag = SET;
	switch (lockWorkFlag)
	{
	case WF_ANTI_LOCK:
	case WF_CLOSE_KEY_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_ERR, LOCK_PARAM_CLOSE);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_ERR;
		Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
		break;//正在执行CLOSE键上锁
	case WF_AUTO_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_ERR, LOCK_PARAM_AUTO);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_ERR;
		Lock_Stat.locked.param = LOCK_PARAM_AUTO;
		break; //正在执行自动上锁
	case WF_KEYS_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_ERR, LOCK_PARAM_KEYS);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_ERR;
		Lock_Stat.locked.param = LOCK_PARAM_KEYS;
		break; //正在执行验证密钥上锁
	case WF_APP_LOCKED:
//        Lock_SendMsgToApp(LOCK_MSG_LOCKED_ERR, LOCK_PARAM_APP);
		Lock_Stat.locked.status = LOCK_MSG_LOCKED_ERR;
		Lock_Stat.locked.param = LOCK_PARAM_APP;
		break;  //正在执行APP密钥上锁
	default:
		break;
	}
}

/**
  * @brief  处理开锁失败
  *
  * @note
  */
static void Lock_UnlockFailed(void)
{
	Lock_SetWorkFlag(ENABLE, lockWorkFlag);//使能用户输入
	switch (lockWorkFlag)
	{
	case WF_AUTO_UNLOCK:
//        Lock_SendMsgToApp(LOCK_MSG_UNLOCK_ERR, LOCK_PARAM_AUTO);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_ERR;
		Lock_Stat.unlock.param = LOCK_PARAM_AUTO;
		break; //正在执行自动开锁
	case WF_KEYS_UNLOCK:
//		Lock_SendMsgToApp(LOCK_MSG_UNLOCK_ERR, LOCK_PARAM_KEYS);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_ERR;
		Lock_Stat.unlock.param = LOCK_PARAM_KEYS;
		break; //正在执行密钥验证开锁
	case WF_CLOSE_KEY_UNLOCK:
//		Lock_SendMsgToApp(LOCK_MSG_UNLOCK_ERR, LOCK_PARAM_CLOSE);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_ERR;
		Lock_Stat.unlock.param = LOCK_PARAM_CLOSE;
		break;//正在执行CLOSE键开锁
	case WF_APP_UNLOCK:
//		Lock_SendMsgToApp(LOCK_MSG_UNLOCK_ERR, LOCK_PARAM_APP);
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_ERR;
		Lock_Stat.unlock.param = LOCK_PARAM_APP;
		break;  //正在执行APP开锁
	default:
		break;
	}
}

/**
  * @brief  处理锁状态改变
  * @note
  *
  * @param  lastSta：锁上次的状态
  * @param  sta：当前锁的状态
  */
static void Lock_LockChange(uint8_t lastSta, uint8_t sta)
{
	if (sta == LOCK_STATUS_LOCKING)
	{
		/* 正在上锁 */
	}
	else if (sta == LOCK_STATUS_UNLOCKING)
	{
		/* 正在开锁 */
	}
	else if (sta == LOCK_STATUS_UNKNOWN)
	{
		/* 锁状态未知 */
		if (lastSta == LOCK_STATUS_UNLOCKING)
		{
			Lock_UnlockFailed();//开锁失败
		}
		else if (lastSta == LOCK_STATUS_LOCKING)
		{
			Lock_LockedFailed();//上锁失败
		}
		else
		{
			/* 机械方式操作，锁状态未知 */
//			Lock_SendMsgToApp(LOCK_MSG_UNKNOWN_MECH, 0);
			Lock_Stat.locking.status = LOCK_MSG_UNKNOWN_MECH;
            Lock_Stat.locking.param = 0;
//            Lock_Stat.lock_parameter = 0;
		}
	}
	else if (sta == LOCK_STATUS_UNLOCK)
	{
		/* 已开锁 */
		if (lastSta == LOCK_STATUS_LOCKING)
		{
			Lock_LockedFailed();//上锁失败
		}
		else if (lastSta == LOCK_STATUS_UNLOCKING)
		{
			Lock_UnlockSucced();//开锁成功
		}
	}
	else if (sta == LOCK_STATUS_LOCKED)
	{
		/* 已上锁 */
		if (lastSta == LOCK_STATUS_UNLOCKING)
		{
			Lock_UnlockFailed();//开锁失败
		}
		else if (lastSta == LOCK_STATUS_LOCKING)
		{
			Lock_LockedSucced();//上锁成功
		}
	}
	else if (sta == LOCK_STATUS_UNLOCK_MECH)
	{
		/* 机械方式开锁 */
//		Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_MECHANICAL);
//		Lock_SendMsgToApp(LOCK_MSG_ANTI_LOCK_OFF, 0);//通知应用层，取消反锁
		Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
		Lock_Stat.unlock.param = LOCK_PARAM_MECHANICAL;
		Lock_Stat.anti_lock.status = LOCK_MSG_ANTI_LOCK_OFF;
        Lock_Stat.anti_lock.param = 0;
		lockedFailedFlag = RESET;
	}
	else if (sta == LOCK_STATUS_LOCKED_MECH)
	{
		/* 机械方式上锁 */
		if (doorStatus == DOOR_STATUS_OPENED || doorStatus == DOOR_STATUS_ABNORMAL)
		{
			/* 上锁成功，如果门是开的，则提示防撬报警（锁体复位时，会有这种情况） */
			doorStatus = DOOR_STATUS_ABNORMAL | 0X80;
		}
		else
		{
//			Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_MECHANICAL);
			Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
			Lock_Stat.locked.param = LOCK_PARAM_MECHANICAL;
			lockedFailedFlag = RESET;
		}
	}
}

/**
  * @brief  锁体管理任务
  *
  * @note   处理锁状态改变逻辑
  */
void Lock_Task(void)
{
	static uint8_t lastLockStatus;
    
    static uint8_t delay = 0;
    
//    LockHal_Scan();
    if (++delay == 20/TIME_BASIC_MS)//20MS执行一次
    {
        delay = 0;
    	if (lockedAbnormal & 0X80)
    	{
    		/* 锁体上锁异常 */
    		lockedAbnormal &= 0X7F;
    		LOCK_LOG("lockedAbnormal:%02X\r\n", lockedAbnormal);

    		if (lockedAbnormal != ABNORMAL_STA_DOOR_OPEN)
    		{
    //			Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 0);
    			Lock_Stat.locking.status = LOCK_MSG_NOT_LOCKED;
                Lock_Stat.locking.param = 0;
    		}
    	}

    	if (unlockAbnormal & 0X80)
    	{
    		/* 锁体开锁异常 */
    		unlockAbnormal &= 0X7F;
    		LOCK_LOG("unlockAbnormal:%02X\r\n", unlockAbnormal);
    	}

    	if (doorStatus & 0X80)
    	{
    		/* 门状态改变 */
    		doorStatus &= 0X7F;
    		Lock_DoorChange(doorStatus);

    		LOCK_LOG("doorStatus:%02X\r\n", doorStatus);
    	}

    	if (lastLockStatus != lockStatus)
    	{
    		/* 锁状态改变 */
    		Lock_LockChange(lastLockStatus, lockStatus);
    		lastLockStatus = lockStatus;

    		LOCK_LOG("lockStatus:%02X\r\n", lockStatus);
    	}

    	/* 手动模式：唤醒自动开锁 */
    	if (autoUnlockFlag != 0)
    	{
    		autoUnlockFlag = 0;
    		if ( (Lock_Stat.lock_workmode == LOCK_MODE_MANUAL)
    		        && (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
    		        && (doorStatus == DOOR_STATUS_CLOSED || doorStatus == DOOR_STATUS_CLOSED_LONG))
    		{
    			LockHal_Unlock();
    			Lock_SetWorkFlag(DISABLE, WF_AUTO_UNLOCK);
    		}
    	}
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @brief  锁体初始化
  *
  * @note   初始化锁体，并注册回调
  * @param  opt：1执行自动开锁，0不执行自动开锁
  */
void Lock_Init(uint8_t opt)
{
	LockHal_Init(Lock_HalCallBack);
	lockWorkFlag = WF_AUTO_LOCKED;
	lockedFailedFlag = RESET;
//	System_CreateTask(20, 20, Lock_Task);//创建锁体管理任务
	autoUnlockFlag = opt;
}

/**
  * @brief  设置锁体工作模式
  *
  * @note   自动模式、手动模式
  */
void Lock_SetMode(LockMode_enum_t mode)
{
	Lock_Stat.lock_workmode = mode;

	if (Lock_Stat.lock_workmode == LOCK_MODE_AUTO && doorStatus == DOOR_STATUS_CLOSED_LONG)
	{
		if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
		{
			LockHal_Locked();
			Lock_SetWorkFlag(DISABLE, WF_AUTO_LOCKED);//设置正在自动上锁
		}
	}
}

/**
  * @brief  处理验证密钥成功
  *
  * @note
  * @param  opt：操作选项
  *         0本地验证成功，1手机app下发开锁指令验证成功
  */
void Lock_ProcessVerifySuceed()
{
	if (doorStatus == DOOR_STATUS_CLOSED || doorStatus == DOOR_STATUS_CLOSED_LONG)
	{
		/* 门已关闭 */
		if (Lock_Stat.lock_workmode == LOCK_MODE_MANUAL)
		{
			/* 手动模式 */
			if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Locked();
//				Lock_SetWorkFlag(DISABLE, (opt==1) ? WF_APP_LOCKED : WF_KEYS_LOCKED);//设置正在密钥上锁
                Lock_SetWorkFlag(DISABLE,WF_KEYS_LOCKED);//设置正在密钥上锁
			}
			else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH)
			{
				LockHal_Unlock();
//				Lock_SetWorkFlag(DISABLE, (opt==1) ? WF_APP_UNLOCK : WF_KEYS_UNLOCK);//设置正在密钥开锁
                Lock_SetWorkFlag(DISABLE,WF_KEYS_UNLOCK);//设置正在密钥开锁
			}
		}
		else if (Lock_Stat.lock_workmode == LOCK_MODE_AUTO)
		{
			/* 自动模式 */
			if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Unlock();
//				Lock_SetWorkFlag(DISABLE, (opt==1) ? WF_APP_UNLOCK : WF_KEYS_UNLOCK);//设置正在密钥开锁
                Lock_SetWorkFlag(DISABLE,WF_KEYS_UNLOCK);//设置正在密钥开锁
			}
			else if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
			{
//				Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, (opt==1) ? LOCK_PARAM_APP : LOCK_PARAM_KEYS);
				Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
//				if (opt == 1)
//					Lock_Stat.lock_parameter = LOCK_PARAM_APP;
//				else
					Lock_Stat.unlock.param = LOCK_PARAM_KEYS;
			}
		}
	}
	else
	{
		/* 门已打开 */
		if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
		{
//			Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, (opt==1) ? LOCK_PARAM_APP : LOCK_PARAM_KEYS);
			Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
//			if (opt == 1)
//				Lock_Stat.lock_parameter = LOCK_PARAM_APP;
//			else
				Lock_Stat.unlock.param = LOCK_PARAM_KEYS;
		}
		else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
		{
			LockHal_Unlock();
//			Lock_SetWorkFlag(DISABLE, (opt==1) ? WF_APP_UNLOCK : WF_KEYS_UNLOCK);//设置正在密钥开锁
            Lock_SetWorkFlag(DISABLE,WF_KEYS_UNLOCK);//设置正在密钥开锁
		}
	}
}

/**
  * @brief  处理App开锁或者关锁
  *         
  * @note   
  * @param  opt：操作选项
  *         0开锁，1上锁
  */
void Lock_ProcessAppOpenAndClose(uint8_t opt)
{
	if (opt == 1)//APP上锁
	{
		if (doorStatus == DOOR_STATUS_CLOSED || doorStatus == DOOR_STATUS_CLOSED_LONG)
		{
			/* 门已关闭 */
			if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Locked();
				Lock_SetWorkFlag(DISABLE, WF_APP_LOCKED);//设置正在APP上锁
			}
			else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH)
			{
//				Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_APP);//已关门
                Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
                Lock_Stat.locked.param = LOCK_PARAM_APP;
			}
		}
		else
		{
//			Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 1);//门未上锁
            Lock_Stat.locking.status = LOCK_MSG_NOT_LOCKED;
            Lock_Stat.locking.param = 1;            
		}
	}
	else if (opt == 0)//APP开锁
	{
//		if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
//		{
//			Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_APP);//已开门
//		}
//		else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
//		{
//			LockHal_Unlock();
//			Lock_SetWorkFlag(DISABLE, WF_APP_UNLOCK);//设置正在APP开锁
//		}

		if ( (doorStatus == DOOR_STATUS_ABNORMAL || doorStatus == DOOR_STATUS_OPENED)
			&& (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH) )
		{
//			Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_APP);
            Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
            Lock_Stat.unlock.param = LOCK_PARAM_APP;
		}
		else
		{
			if (LockHal_Unlock() == SUCCESS)
			{
				Lock_SetWorkFlag(DISABLE, WF_APP_UNLOCK);//设置正在APP开锁
			}
			else
			{
//				Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_APP);//已开门
                Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
                Lock_Stat.unlock.param = LOCK_PARAM_APP;
			}
		}
	}
}

/**
  * @brief  处理CLOSE键动作
  * @note
  *
  * @param  action：按键动作
  *         0：长按，    1：单机，   2：双击
  */
void Lock_ProcessCloseKey(uint8_t action)
{
	if (action == 0)
	{
		/* 执行长按动作 */
		if (doorStatus == DOOR_STATUS_CLOSED || doorStatus == DOOR_STATUS_CLOSED_LONG)
		{
			/* 门已关闭 */
			if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Locked();
				Lock_SetWorkFlag(DISABLE, WF_ANTI_LOCK);//设置正在反锁
			}
			else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH)
			{
//				Lock_SendMsgToApp(LOCK_MSG_ANTI_LOCK_ON, 0);
				Lock_Stat.anti_lock.status = LOCK_MSG_ANTI_LOCK_ON;
                Lock_Stat.anti_lock.param = 0;
			}
		}
		else
		{
			/* 门已开启 */
//			Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 1);
			Lock_Stat.locked.status = LOCK_MSG_NOT_LOCKED;
			Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
		}
	}
	else if (action == 1)
	{
		/* 执行单击动作 */
		if (doorStatus == DOOR_STATUS_CLOSED || doorStatus == DOOR_STATUS_CLOSED_LONG)
		{
			/* 门已关闭 */
			if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH)
			{
//				Lock_SendMsgToApp(LOCK_MSG_LOCKED_OK, LOCK_PARAM_CLOSE);
				Lock_Stat.locked.status = LOCK_MSG_LOCKED_OK;
				Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
			}
			else if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
			{
				LockHal_Locked();
				Lock_SetWorkFlag(DISABLE, WF_CLOSE_KEY_LOCKED);//设置正在CLOSE上锁
			}
		}
		else
		{
			/* 门已开启 */
//			Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 1);
			Lock_Stat.locked.status = LOCK_MSG_NOT_LOCKED;
			Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
		}
	}
	else if (action == 2)
	{
		/* 执行双击动作 */
//		if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
//		{
////			Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_CLOSE);
//			Lock_Stat.unlock_status = LOCK_MSG_UNLOCK_OK;
//			Lock_Stat.lock_parameter = LOCK_PARAM_CLOSE;
//		}
//		else if (lockStatus == LOCK_STATUS_LOCKED || lockStatus == LOCK_STATUS_LOCKED_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
//		{
//			LockHal_Unlock();
//			Lock_SetWorkFlag(DISABLE, WF_CLOSE_KEY_UNLOCK);//设置正在CLOSE开锁
//		}
        if ( (doorStatus == DOOR_STATUS_ABNORMAL || doorStatus == DOOR_STATUS_OPENED)
			&& (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH) )
		{
//			Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_CLOSE);
            Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
			Lock_Stat.unlock.param = LOCK_PARAM_CLOSE;
		}
		else
		{
			if (LockHal_Unlock() == SUCCESS)
			{
				Lock_SetWorkFlag(DISABLE, WF_CLOSE_KEY_UNLOCK);//设置正在上锁
			}
			else
			{
//				Lock_SendMsgToApp(LOCK_MSG_UNLOCK_OK, LOCK_PARAM_CLOSE);//ÒÑ¿ªÃÅ
                Lock_Stat.unlock.status = LOCK_MSG_UNLOCK_OK;
			    Lock_Stat.unlock.param = LOCK_PARAM_CLOSE;
			}
		}
	}
}

/**
  * @brief  系统休眠前5s，自动上锁
  *
  * @note   满足2个条件：1）门关闭状态，2）没有出现上锁异常
  *         则 系统休眠前，执行自动上锁（自动模式：执行上锁，手动模式：执行开锁回拖）
  */
void Lock_SleepAutoLocked(void)
{
	if (doorStatus == DOOR_STATUS_CLOSED_LONG || doorStatus == DOOR_STATUS_CLOSED)
	{
		if (lockedFailedFlag != SET)
		{
			if (Lock_Stat.lock_workmode == LOCK_MODE_AUTO)//自动模式
			{
				if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH || lockStatus == LOCK_STATUS_UNKNOWN)
				{
					LockHal_Locked();
					Lock_SetWorkFlag(DISABLE, WF_AUTO_LOCKED);//设置正在自动上锁
				}
			}
			else if (Lock_Stat.lock_workmode == LOCK_MODE_MANUAL)//手动模式
			{
				if (lockStatus == LOCK_STATUS_UNLOCK || lockStatus == LOCK_STATUS_UNLOCK_MECH)
				{
					LockHal_UnlockBack();
//					Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 1);
					Lock_Stat.locking.status = LOCK_MSG_NOT_LOCKED;
					Lock_Stat.locking.param = LOCK_PARAM_CLOSE;
				}
				else if (lockStatus == LOCK_STATUS_UNKNOWN)
				{
					LockHal_Locked();
					Lock_SetWorkFlag(DISABLE, WF_AUTO_LOCKED);//设置正在自动上锁
				}
			}
		}
		else
		{
//			Lock_SendMsgToApp(LOCK_MSG_NOT_LOCKED, 1);
			Lock_Stat.locked.status = LOCK_MSG_NOT_LOCKED;
			Lock_Stat.locked.param = LOCK_PARAM_CLOSE;
		}
	}
}

pLockStatus_t Lock_Status()
{
	return &Lock_Stat;
}




