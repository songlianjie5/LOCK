#ifndef _UTILITIES_H
#define _UTILITIES_H
#include "main.h"
    
uint8_t KeysMan_MatchKeysLibrary(uint8_t *pData, uint8_t len);

#define PWD_LENGHT_MAX      12		        //有效密码最大长度
extern bool Pwd_Analyse(uint8_t *pbuf);     //判断密码是否简单密码 true为过于简单
    
#endif

