#include "Utilities.h"

static uint8_t KeysMan_GetKeysLen(uint8_t *pData, uint8_t maxLen)
{
	uint8_t i;
	
	for (i=0; i<maxLen; i++)
	{
		if (pData[i] == 0x00)
		{
			return i;
		}
	}
	return i;
}

static ErrorStatus KeysMan_MemoryCmp(uint8_t *pMinData, uint8_t minLen, uint8_t *pMaxData, uint8_t maxLen, uint8_t opt)
{
  uint8_t i = 0;
  uint8_t offset = 0;
  
  if (maxLen < minLen || minLen == 0)
  {
     return ERROR;
  }
  while ((i < minLen) && ((i+offset) < maxLen))
  {
    if (pMinData[i] != pMaxData[i+offset])
    {
      if (opt == 0)
      {
        return ERROR;
      }
      i = 0;
      offset++;
    }
    else
    {
      i++;
    }
  }
  if (opt == 0)
  {
    return (minLen == maxLen) ? SUCCESS : ERROR;
  }
  else
  {
    return (i == minLen) ? SUCCESS : ERROR;
  }
}

//uint8_t KeysMan_MatchKeysLibrary(uint8_t *pData, uint8_t len)
//{
//	uint8_t id, keysLen;
//	NvPassword_stu_t pwdData;
//	NvFpt_stu_t      fptData;
//	NvCard_stu_t     cardData;
//	
//	for (id=0; id<KEYS_QTY_PWD; id++)
//	{
//		NvMan_ReadKeys(KEYS_TYPE_PWD, id, &pwdData);
//		keysLen = KeysMan_GetKeysLen(pwdData.pwd, PWD_LEN_MAX);
//		if (KeysMan_MemoryCmp(pwdData.pwd, keysLen, pData, len, 0) == SUCCESS)
//		{
////			*pAttr = pwdData.attr;
//			return id;
//		}
//	}
//	return 0XFF;
//}

//密码是否过于简单
bool Pwd_Analyse(uint8_t *pbuf)//判断密码是否简单密码
{
    uint8_t i, len;
    for(i = 0; i < PWD_LENGHT_MAX; i++)
    {
        if(pbuf[i] == 0x00)
        {
            len = i;
            break;
        }
    }
    if(i == PWD_LENGHT_MAX)len = PWD_LENGHT_MAX;                                //获取密码长度

    //----------------------------------------------------------------------------
    for(i = 1; i < len; i++)
    {
        if(pbuf[i - 1] != pbuf[i])
        {
            break;
        }
    }
    if(i == len)                                                                //各位密码一样
    {
        return true;
    }
    //----------------------------------------------------------------------------
    for(i = 1; i < len; i++)
    {
        if((pbuf[i]- pbuf[i - 1]) != 1)
        {
            break;
        }
    }
    if(i == len)                                                                //为连续的密码
    {
        return true;
    }
    //----------------------------------------------------------------------------
    return false;
}