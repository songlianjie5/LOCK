#include "Record.h"


#ifdef PLATFORM
       
const uint8_t Em_EEPROM_em_EepromStorage[Em_EEPROM_PHYSICAL_SIZE]

CY_SECTION(".my_emulated_eeprom") __ALIGNED(CY_FLASH_SIZEOF_ROW) = {0};

#endif//end PLATFORM

bool Record_InitSet()
{
#ifdef PLATFORM   
    
    cy_en_em_eeprom_status_t returnValue;
    
    returnValue = Em_EEPROM_Init((uint32_t)Em_EEPROM_em_EepromStorage);  
    
    if (returnValue != CY_EM_EEPROM_SUCCESS)
    {
        return false;   
    }
    else
    {
        //NvMan_Init();
        return true;
    } 
#endif //end PLATFORM   
}

bool Record_Read(uint32_t addr,void * eepromData,uint32_t size)
{
#ifdef PLATFORM     
    
    cy_en_em_eeprom_status_t returnValue;
    
    if (addr<EEPROM_BASE_ADD)
    {
        return false;
    }
    
    returnValue = Em_EEPROM_Read(addr-EEPROM_BASE_ADD, eepromData, size);
    
    if (returnValue != CY_EM_EEPROM_SUCCESS)
    {
        return false;   
    }
    else
    {
        return true;
    }
    
#endif //end PLATFORM 
}

bool Record_Write(uint32_t addr,void * eepromData,uint32_t size)
{
#ifdef PLATFORM     
    
    cy_en_em_eeprom_status_t returnValue;
    
    if (addr<EEPROM_BASE_ADD)
    {
        return false;
    }
    
    CyGlobalIntDisable;
    
    returnValue = Em_EEPROM_Write(addr-EEPROM_BASE_ADD, eepromData, size);
    
    CyGlobalIntEnable;
    
    if (returnValue != CY_EM_EEPROM_SUCCESS)
    {
        return false;   
    }
    else
    {
        return true;
    }
#endif //end PLATFORM
}


const Record_t LockRecord __attribute__((at(EEPROM_BASE_ADD)))={\
    {'A','B','C','D','E','F','G','1','2','3','4','5','6','7','8','9','1'},  //初始化SN
    {0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x00,0x00,0x00,0x00},      //默认管理密码12345678
    {MUTE_OFF,CHN,DOOR_NORMAL},                                         //开机不静音、中文、正常模式
    {0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x00,0x00,0x00,0x00},      //默认开机用户秘密12345678
    {0},
    {0}
};

//写管理秘密
bool WriteManagePSW(uint8_t p[])
{
    Record_Write((uint32_t)&LockRecord.ManagePSWRecord,p,sizeof(LockRecord.ManagePSWRecord));
    return true;
}
//读管理密码
bool ReadManagePSW(uint8_t p[])
{
    uint8_t i;
    Record_Read((uint32_t)&LockRecord.ManagePSWRecord,p,sizeof(LockRecord.ManagePSWRecord));
    if(p[0]==0)//如果为空，则返回默认密码
    {
        for(i=0;i<sizeof(LockRecord.ManagePSWRecord);i++)
         p[i] = LockRecord.ManagePSWRecord[i];
    }
    return true;
}
//验证管理密码
bool ReadAndcheckUserPmpswByPSW(uint8_t psw[])
{
    uint8_t p[sizeof(LockRecord.ManagePSWRecord)];
    ReadManagePSW(p);
    if(0 == strncmp((const char *)p,(const char *)psw,sizeof(LockRecord.ManagePSWRecord)))
    {
        return true;
    }
    return false;
}
//写用户密码
bool WriteUserPSW(uint8_t id,uint8_t p[])
{
    UserPswRecord_t pswtemp;
    uint8_t i;
    for(i=0;i<LEN_USER_PSW_RECORD;i++)   //先查找是否已存在的ID
    {
        Record_Read((uint32_t)&LockRecord.UserPswRecord[i],&pswtemp,sizeof(UserPswRecord_t));
        if(id==pswtemp.ID)
        {
            return false; //已存在
        }
        else if(0x00==pswtemp.UPSW[0])
        {
            break;        //空的，没存过的
        }
    } 
    if(i<LEN_USER_PSW_RECORD)              //在10组内 且没写过的
    {
        memcpy(pswtemp.UPSW,p,12);
        pswtemp.ID = id;
        Record_Write((uint32_t)&LockRecord.UserPswRecord[i],&pswtemp,sizeof(UserPswRecord_t));
        return true;
    }
    return false;
}
//查找用户密码 时按ID查找
bool ReadAndcheckUserPswByID(uint8_t upsw[],uint8_t ID)
{
    UserPswRecord_t p;
    uint8_t i;
    for(i=0;i<10;i++)
    {
        Record_Read((uint32_t)&LockRecord.UserPswRecord[i],&p,sizeof(UserPswRecord_t));
        if(p.ID==ID)
        {
            memcpy(upsw,p.UPSW,12);
            return true;
        }
    }
    return false;
}
//查找用户密码 时按PSW查找
bool ReadAndcheckUserPswByPSW(uint8_t *id,uint8_t upsw[])
{
    UserPswRecord_t p;
    uint8_t i;
    for(i=0;i<LEN_USER_PSW_RECORD;i++)
    {
        Record_Read((uint32_t)&LockRecord.UserPswRecord[i],&p,sizeof(UserPswRecord_t));
        if( (p.ID !=0x00) && (memcmp(p.UPSW,upsw,12)==0) )  //不为空且相等
        {
            *id = p.ID;
            return true;
        }            
    }
    return false;
}
//删除用户密码 true 为清空
bool DelUserPswRecord(uint8_t id,bool option)
{
    uint8_t i;
    UserPswRecord_t PswTemp;                                    //10组密码
    UserPswRecord_t p;
    memset(&PswTemp,0,sizeof(UserPswRecord_t));
    if(option)
    {
        for(i=0;i<LEN_USER_PSW_RECORD;i++)
        {
            Wdt_Feed_Dog();                                                     //清看门狗
            Record_Write((uint32_t)&LockRecord.UserPswRecord[i],&PswTemp,sizeof(UserPswRecord_t));
        }
        return true;
    }
    else
    {
        for(i=0;i<LEN_USER_PSW_RECORD;i++)                                                       //按Id来清除
        {
            Record_Read((uint32_t)&LockRecord.UserPswRecord[i],&p,sizeof(UserPswRecord_t));
            Wdt_Feed_Dog();                                                     //清看门狗
            if(p.ID==id)                                
            {
                Record_Write((uint32_t)&LockRecord.UserPswRecord[i],&PswTemp,sizeof(UserPswRecord_t));
                return true;
            }
        }
    }
    return false;
}
//写卡片
bool WriteCardRecord(uint8_t id,uint8_t CID[])
{
    CardRecord_t temp;
    uint8_t i;
    for(i=0;i<LEN_CRAD_RECORD;i++)   //先查找是否已存在的ID
    {
        Record_Read((uint32_t)&LockRecord.CardRecord[i],&temp,sizeof(CardRecord_t));
        if(id==temp.ID)
        {
            return false; //已存在
        }
        else if(0x00==temp.CID[0])
        {
            break;        //空的，没存过的
        }
    } 
    if(i<LEN_CRAD_RECORD)              //在100组内 且没写过的
    {
        memcpy(temp.CID,CID,4);
        temp.ID = id;
        Record_Write((uint32_t)&LockRecord.CardRecord[i],&temp,sizeof(CardRecord_t));
    }
    return true;
}
//读与查找卡片 通过UID查找
bool ReadAndcheckCardByUID(uint8_t *id,uint8_t CID[])
{
    CardRecord_t p;
    uint8_t i;
    for(i=0;i<LEN_CRAD_RECORD;i++)
    {
        Record_Read((uint32_t)&LockRecord.CardRecord[i],&p,sizeof(CardRecord_t));
        if(memcmp(p.CID,CID,4)==0)
        {
            *id = p.ID;
            return true;
        }
    }
    return false;
}
//读与查找卡片 通过id查找
bool ReadAndcheckCardByid(uint8_t id,uint8_t CID[])
{
    CardRecord_t p;
    uint8_t i;
    for(i=0;i<LEN_CRAD_RECORD;i++)
    {
        Record_Read((uint32_t)&LockRecord.CardRecord[i],&p,sizeof(CardRecord_t));
        if(p.ID == id)
        {
            memcpy(CID,p.CID,4);
            return true;
        }
    }
    return false;
}
//删除卡片 true为清空 false为按id删除或按CID删
bool DelCardRecord(uint8_t id,uint8_t CID[],bool option)
{
    uint8_t i;
    CardRecord_t CardTemp;                                                      //100组卡片
    memset(&CardTemp,0,sizeof(CardRecord_t));
    for(i=0;i<LEN_CRAD_RECORD;i++)
    {
        Wdt_Feed_Dog();                                                         //清看门狗
        if(option)                                                              //全清
        {
            Record_Write((uint32_t)&LockRecord.CardRecord[i],&CardTemp,sizeof(CardRecord_t)); 
            return true;
        }
        else                                                                    //按id 或 按CID
        {
            Record_Read((uint32_t)&LockRecord.CardRecord[i],&CardTemp,sizeof(CardRecord_t));
            if((id== CardTemp.ID) || (memcmp(CardTemp.CID,CID,4)==0)) 
            {
                memset(&CardTemp,0,sizeof(CardRecord_t));                       //清空
                Record_Write((uint32_t)&LockRecord.CardRecord[i],&CardTemp,sizeof(CardRecord_t));
                return true;                                                    //单个删除成功
            }
        }                                                               
    }
    return false;
}

//写存指纹
bool WriteFigRecord(uint8_t id,uint8_t FID)
{
    FigRecord_t temp;
    uint8_t i;
    for(i=0;i<LEN_FIG_RECORD;i++)   //先查找是否已存在的ID
    {
        Record_Read((uint32_t)&LockRecord.FigRecord[i],&temp,sizeof(FigRecord_t));
        if(id==temp.ID)
        {
            return false; //已存在
        }
        else if(0x00==temp.FID)
        {
            break;        //空的，没存过的
        }
    } 
    if(i<LEN_FIG_RECORD)              //在10组内 且没写过的
    {
        temp.FID = FID;
        temp.ID = id;
        Record_Write((uint32_t)&LockRecord.FigRecord[i],&temp,sizeof(FigRecord_t));
    }
    return true;
}
//删除指纹 true为清空
bool DelFigRecord(uint8_t id,bool option)
{
    uint8_t i;
    FigRecord_t temp;
    if(option)               //清空指纹库
    {
        temp.FID = temp.ID = 0x00;
        for(i=0;i<LEN_FIG_RECORD;i++)Record_Write((uint32_t)&LockRecord.FigRecord[i],&temp,sizeof(FigRecord_t));
        return true;         //清空成功
    }
    else
    {
        for(i=0;i<LEN_FIG_RECORD;i++)   //先查找ID
        {
            Record_Read((uint32_t)&LockRecord.FigRecord[i],&temp,sizeof(FigRecord_t));
            if(id==temp.ID)
            {
                temp.FID = temp.ID = 0x00;
                Record_Write((uint32_t)&LockRecord.FigRecord[i],&temp,sizeof(FigRecord_t));
                return true; //删除成功
            }
        }
    }
    return false;
}
//读与查找指纹 通过id
bool ReadAndcheckFigByid(uint8_t id,uint8_t *FID)
{
    FigRecord_t p;
    uint8_t i;
    for(i=0;i<LEN_FIG_RECORD;i++)
    {
        Record_Read((uint32_t)&LockRecord.FigRecord[i],&p,sizeof(FigRecord_t));
        if(id==p.ID)
        {
            *FID = p.FID;
            return true;
        }
    }
    return false;
}
//读开门记录 注：一个id有多种类型的记录
//bool ReadOpenDoorRecordByid(uint8_t index,uint8_t id,DoordRecord_t *record)
//{
//    DoordRecord_t doorTemp;
//    uint8_t i;
//    for(i=0;i<LEN_DOOR_RECORD;i++)                                                          //查找所有记录                                                 
//    {
//        Record_Read((uint32_t)&LockRecord.DoodrRecord[i],&doorTemp,sizeof(DoordRecord_t));
//        if(doorTemp.ID == id)
//        {
//            memcpy(record,&doorTemp,sizeof(DoordRecord_t));                                 //返回记录
//            return true;
//        }
//    }
//    return false;
//}
//写开门记录
bool WriteOpenDoorRecordById(OpenDoorClass_t class,uint8_t id,RtcRecord_t *t,uint8_t *para)
{
    DoordRecord_t doorTemp,doorTemp1,doorTemp2;
    volatile uint8_t i;
    for(i=0;i<(LEN_DOOR_RECORD-1);i++)                                                        //查找所有记录                                                 
    {
        Record_Read((uint32_t)&LockRecord.DoodrRecord[i],&doorTemp1,sizeof(DoordRecord_t));
        Record_Read((uint32_t)&LockRecord.DoodrRecord[i+1],&doorTemp2,sizeof(DoordRecord_t));
        
        if(doorTemp1.loop != doorTemp2.loop)                                                //标记不相同
        {
            if(doorTemp2.loop=='A')doorTemp.loop = 'B';
            else doorTemp.loop = 'A';
            i = i+1;                                                                        //写入第二个
            break;                                                                          //退出查找
        }
        else                                                                                //标记相同
        {
            if(i==0)                                                                        //只需要验证第一个和最后一个
            {
                Record_Read((uint32_t)&LockRecord.DoodrRecord[(LEN_DOOR_RECORD-1)],&doorTemp2,sizeof(DoordRecord_t));//读最后一个
                if(doorTemp1.loop == doorTemp2.loop)                                        //已记满 需要从0开始记                        
                {
                    if(doorTemp1.loop=='A')doorTemp.loop = 'B';
                    else doorTemp.loop = 'A';
                    break;                                                                  //退出查找
                }
            }
        }
    }
    
    doorTemp.Class = class;
    doorTemp.ID = id;
    memcpy(&doorTemp.Time,t,sizeof(Rtc_t));
    if(class==C_CARD)
        memcpy(&doorTemp.Para,para,4);                                                       //4字节卡片ID 没有写0
    if(Record_Write((uint32_t)&LockRecord.DoodrRecord[i],&doorTemp,sizeof(DoordRecord_t)))
        return true;
    else 
        return false; 
}
//读系统设置
bool ReadOptionRecord(OptionRecord_t *option)                                      
{
    Record_Read((uint32_t)&LockRecord.OptionRecord,option,sizeof(OptionRecord_t));
    return true;
}
//写系统设置
bool WriteOptionRecord(OptionRecord_t *option)
{
    Record_Write((uint32_t)&LockRecord.OptionRecord,option,sizeof(OptionRecord_t));
    return false;
}
//恢复出厂设置
void ResteLockRecord(void)
{
	UserPswRecord_t PswTemp;                                    //10组密码
    CardRecord_t CardTemp;                                      //100组卡片                                    
	FigRecord_t FigTemp;                                        //100组指纹 
    DoordRecord_t DoordTemp;                                    //100组开门记录
    uint8_t i;
    //所有开门密码 指纹 卡片 管理密码为12345678
    Record_Write((uint32_t)&LockRecord.SysSN,(uint8_t *)&LockRecord.SysSN,sizeof(LockRecord.SysSN));
    Record_Write((uint32_t)&LockRecord.ManagePSWRecord,(uint8_t *)&LockRecord.ManagePSWRecord,sizeof(LockRecord.ManagePSWRecord));
    Record_Write((uint32_t)&LockRecord.OptionRecord,(uint8_t *)&LockRecord.OptionRecord,sizeof(OptionRecord_t));
    memset(&PswTemp,0,sizeof(UserPswRecord_t));
    memset(&CardTemp,0,sizeof(CardRecord_t));
    memset(&FigTemp,0,sizeof(FigRecord_t));
    memset(&DoordTemp,0,sizeof(DoordRecord_t));
    for(i=0;i<10;i++)
    {
        Wdt_Feed_Dog();                                                     //清看门狗
        Record_Write((uint32_t)&LockRecord.UserPswRecord[i],&PswTemp,sizeof(UserPswRecord_t));
    }
    for(i=0;i<100;i++)
    {
        Wdt_Feed_Dog();                                                     //清看门狗
        Record_Write((uint32_t)&LockRecord.CardRecord[i],&CardTemp,sizeof(CardRecord_t));
        Record_Write((uint32_t)&LockRecord.FigRecord[i],&FigTemp,sizeof(FigRecord_t));
        //需要调用指纹头全清空函数
        Record_Write((uint32_t)&LockRecord.DoodrRecord[i],&DoordTemp,sizeof(DoordRecord_t)); //清空开门记录
    }
}
//void ReadLockRecord(void)                                               //读取开机参数
//{
//    Record_Read((uint32_t)&LockRecord.SysSN,&Function.ManageMode.SysSN,sizeof(LockRecord.SysSN));
//    Record_Read((uint32_t)&LockRecord.ManagePSWRecord,&Function.ManageMode.ManagePSW,sizeof(LockRecord.ManagePSWRecord));
//    Record_Read((uint32_t)&LockRecord.OpionRecord,&Function.ManageMode.Voice,sizeof(OpionRecord_t));
//}