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
#ifndef _RECORD_H
#define _RECORD_H
#include "main.h"    
    
#define EEPROM_BASE_ADD  0x1B000                                    //Flash存储地址  非性线            
    
typedef struct
{
    uint8_t Attr;
    uint8_t ID;
	uint8_t UPSW[16];
}UserPswRecord_t;
typedef struct
{
    uint8_t Attr;
    uint8_t ID;
	uint8_t CID[4];
}CardRecord_t;
typedef struct
{
    uint8_t Attr;
    uint8_t ID;
	uint16_t FID;
}FigRecord_t;

typedef enum
{
    C_PSW=1,
    C_FIG=2,
    C_CARD=3,
    C_BLE=4,
}OpenDoorClass_t;
typedef struct
{
    uint16_t year;  
    uint8_t month;      
    uint8_t day;        
    uint8_t hour;   
    uint8_t minute;     
    uint8_t second;     
}RtcRecord_t;
typedef struct
{
    OpenDoorClass_t Class;                                          //开门类型 指纹、卡片、密码、蓝牙
    uint8_t ID;                                                     //开门ID
    uint8_t Para[4];                                                //参数 比如卡片4ID
    uint8_t loop;                                                   //已记录满开始循环记录
	RtcRecord_t Time;                                               //开门时间
}DoordRecord_t;

typedef struct
{
    uint8_t Voice;                                                  //语音
    uint8_t Language;                                               //语言
    uint8_t OpenDoorMode;                                           //开门模式
}OptionRecord_t;


#define LEN_SN                      17
#define LEN_MANAGE_PSW_RECORD       12                              //管理密码长度
#define LEN_USER_PSW_RECORD         10                              //用户密码组数
#define LEN_CRAD_RECORD             100                             //卡片组数
#define LEN_FIG_RECORD              100                             //指纹组数
#define LEN_DOOR_RECORD             200                             //开门记录组数
typedef struct
{ 
    uint8_t SysSN[17];                                              //列号      17
    uint8_t ManagePSWRecord[12];                                    //管理密码  12
    OptionRecord_t OptionRecord;                                    //系统设置  3
	UserPswRecord_t UserPswRecord[LEN_USER_PSW_RECORD];             //10组密码  18*10   
    CardRecord_t CardRecord[LEN_CRAD_RECORD];                       //100组卡片 6*100                                    
	FigRecord_t FigRecord[LEN_FIG_RECORD];                          //100组指纹 4*100                 
	DoordRecord_t DoodrRecord[LEN_DOOR_RECORD];                     //200开门记录 16*200
}Record_t;

extern const Record_t LockRecord; 
extern bool Record_InitSet();
extern bool Record_Read(uint32_t addr,void * eepromData,uint32_t size);
extern bool Record_Write(uint32_t addr,void * eepromData,uint32_t size);

extern bool WriteManagePSW(uint8_t p[]);                            //写管理秘密
extern bool ReadManagePSW(uint8_t p[]);                             //读管理密码
extern bool ReadAndcheckUserPmpswByPSW(uint8_t psw[]);              //验证管理密码

extern bool WriteUserPSW(uint8_t id,uint8_t p[]);                   //写用户密码        
extern bool ReadAndcheckUserPswByID(uint8_t upsw[],uint8_t ID);     //查找用户密码 时按ID查找upsw
extern bool ReadAndcheckUserPswByPSW(uint8_t *id,uint8_t upsw[]);   //查找用户密码 时按upsw查找ID
extern bool DelUserPswRecord(uint8_t id,bool option);               //删除用户密码 true为清空

extern bool WriteCardRecord(uint8_t id,uint8_t CID[]);              //写卡片
extern bool ReadAndcheckCardByUID(uint8_t *id,uint8_t CID[]);       //读与查找卡片 通过UID查找
extern bool ReadAndcheckCardByid(uint8_t id,uint8_t CID[]);         //读与查找卡片 通过id查找
extern bool DelCardRecord(uint8_t id,uint8_t CID[],bool option);    //删除卡片 true为清空 false为按id删除或按CID删

extern bool WriteFigRecord(uint8_t id,uint8_t FID);                 //写存指纹
extern bool DelFigRecord(uint8_t id,bool option);                   //删除指纹 true为清空
extern bool ReadAndcheckFigByid(uint8_t id,uint8_t *FID);           //读与查找指纹 通过id 

extern void ResteLockRecord(void);                                  //恢复出厂设置

extern bool ReadOptionRecord(OptionRecord_t *option);               //读系统设置
extern bool WriteOptionRecord(OptionRecord_t *option);              //写系统设置

extern bool WriteOpenDoorRecordById(OpenDoorClass_t class,uint8_t id,RtcRecord_t *t,uint8_t *para);//写开门记录

#endif

/* [] END OF FILE */
