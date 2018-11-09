/**
  ******************************************************************************
  * @file         lock_evt.c
  * @brief        锁体延时事件管理，实现精确无阻塞延时执行某个事件
  * @author       Fli
  * @date         2018-06-22
  * @version      v001
  * @copyright    kaadas
  * 
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#include "lock_evt.h"

#define EVENT_MAX_QTY  (5)

static __IO uint32_t timeBeatCount;//时间节拍

static struct
{
	void (*pFunction)(void);
	uint32_t timeout;
}eventList[EVENT_MAX_QTY]; //事件表

static uint8_t mutexFlag;  //事件表访问互斥标志



/**
  * @brief  清除所有延时事件
  *         
  * @note   
  */
void LockEvt_Clear(void)
{
	mutexFlag = 1;
	
	timeBeatCount = 0;
	memset(eventList, 0, sizeof(eventList));
	
	mutexFlag = 0;
}

/**
  * @brief  创建一个延时事件
  * @note   
  *         
  * @param  pFun：事件处理函数
  * @param  timeMs：延时时间MS
  */
ErrorStatus LockEvt_Create(void (*pFun)(void), uint16_t timeMs)
{
	uint8_t i;
	
	mutexFlag = 1;

	if (pFun == NULL)
	{
		mutexFlag = 0;
		return ERROR;
	}
	
	for (i=0; i<EVENT_MAX_QTY; i++)
	{
		if (eventList[i].pFunction == NULL)
		{
			break;
		}
	}
	
	if (i == EVENT_MAX_QTY)
	{
		mutexFlag = 0;
		return ERROR;
	}

	eventList[i].timeout = timeMs + timeBeatCount;
	eventList[i].pFunction = pFun;
	
	mutexFlag = 0;
	return SUCCESS;
}

/**
  * @brief  延时事件 时间更新函数
  *         
  * @note   该函数必须使用定时器中断每间隔2ms调用一次
  */
void LockEvt_TimeUpdate(void)
{
	uint8_t i;
	void (*pFun)(void);
	
	timeBeatCount += TIME_BASIC_MS;
	
	if (mutexFlag != 0)
	{
		return;
	}
	
	for (i=0; i<EVENT_MAX_QTY; i++)
	{
		if (eventList[i].pFunction != NULL && timeBeatCount > eventList[i].timeout)
		{
			pFun = eventList[i].pFunction;
			
			eventList[i].pFunction = NULL;
			eventList[i].timeout = 0;
			
			pFun();
			break;
		}
	}
}

