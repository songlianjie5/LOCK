/**
  ******************************************************************************
  * @file         lock_evt.h
  * @brief        与lock_evt.c对应的头文件
  * @author       Fli
  * @date         2018-06-22
  * @version      v001
  * @copyright    kaadas
  * 
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#ifndef __LOCK_EVT_H__
#define __LOCK_EVT_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


ErrorStatus LockEvt_Create(void (*pFun)(void), uint16_t timeMs);
void LockEvt_Clear(void);
void LockEvt_TimeUpdate(void);


#ifdef __cplusplus
}
#endif

#endif /*__LOCK_EVT_H__ */
