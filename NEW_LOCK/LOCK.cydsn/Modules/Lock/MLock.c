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
#include "MLock.h"

#ifdef PLATFORM

#define Power_LockSensorControl(endis)  {T_POWER_CTRL(endis);FL3225_DataUpdate();}    //锁体电源开关
    
#define SENSOR_LOCKED_GPIO  OP3_INT_Read() //上锁传感器IO（主锁舌伸出到位感应）
#define SENSOR_UNLOCK_GPIO  ((LOCK_INT_Read()&(1<<0))>>0) //OP2_INT_Read() //开锁传感器IO（主锁舌缩进到位感应）
#define SENSOR_BACK_GPIO    OP1_INT_Read() //回拖传感器IO
#define SENSOR_DOOR_GPIO    ((LOCK_INT_Read()&(1<<3))>>3) //门状态检测IO（叉舌）
    
#define SLOT_STATUS()        (uint8_t)(((* (reg32 *) CYREG_GPIO_PRT0_PS) & (1<<PIN3)) >> PIN3) //锁体叉舌

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
    
#endif

#define SINCONVER()       {ADC_SAR_START_CTRL_REG = ADC_FW_TRIGGER;} //单次转换
#define CONTCONVER()      {ADC_SAR_SAMPLE_CTRL_REG |= ADC_CONTINUOUS_EN;}//连续转换
#define CONTCONVERSTOP()  {if (ADC_SAR_SAMPLE_CTRL_REG&(uint32)ADC_CONTINUOUS_EN)) ADC_SAR_SAMPLE_CTRL_REG &= (uint32)(~ADC_CONTINUOUS_EN);}//连续转换停止


__IO static FlagStatus adcMutexFlag; //ADC互斥标志


ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel)
{
	uint8_t chan = (1 << ADC_Channel);
    
    if (adcMutexFlag == SET)
	{
		return ERROR;//ADC已占用
	}
	adcMutexFlag = SET;
    
    ADC_SetChanMask(chan);
	
	return SUCCESS;
}

/**
  * @brief  软件启动一次ADC转换并读取转换结果
  * @note   
  *
  * @return 返回结果
  */
uint16_t AdcHal_ReadResult(uint8_t chan)
{
	uint16_t value = 0;
    
    uint16_t i = 0;
		
	adcMutexFlag = RESET;
    
    SINCONVER();
    
    while(ADC_IsEndConversion(ADC_WAIT_FOR_RESULT)==0 && (++i)<10000);
    
    value = ADC_GetResult16(chan);
	
	return value;
}

void AdcHal_StopAdcDog(void)
{
    if (ADC_SAR_SAMPLE_CTRL_REG&(uint32)ADC_CONTINUOUS_EN)
	{
        ADC_SAR_SAMPLE_CTRL_REG &= (uint32)(~ADC_CONTINUOUS_EN);
		adcMutexFlag = RESET;
	}
}


/**
  * @brief  初始化
  * @note    
  */
void AdcHal_Init(void)
{
	adcMutexFlag = RESET;
    ADC_Start();
}

Lock_t Lock;

//上电主舌只有三种状态，缩入到位、伸出到位、中间状态、异常状态(伸出堵、缩进堵、伸出缩进都堵)、超时异常


//上电
//关门
//1、伸出
//2、伸出到位 并 堵转发生   (没到位 堵转 产生 伸出堵异常)
//3、回拖

//开门
//1、缩进
//2、缩进到位 并 堵转发生   (没到位 堵转 产生 缩进堵异常)
//3、回拖


//相反向转
//100ms
//是否堵转 并 到位         (没到位 堵转 产生 伸出缩进都异常)
//500ms

//是否超过3次

uint8_t Lock_Slot_Scan(void);
bool Lock_Init(void)
{
    #ifdef PLATFORM
    
    #endif
    //先读一遍传感器
    //判断是否已回拖正常
    
    //SLOT 的状态
    Lock_Slot_Scan();
    Lock.slot_act = false;
    return true;
}
Lock_t *Lock_Status(void)
{
    return &Lock;
}
uint8_t LockOpen(void)
{
    if(Lock.status!=MAINLOCK_OPENED)        //已开门状态下不再开门
        Lock.option = MAINLOCK_OPEN_CMD;
    return 0;
}
uint8_t LockClose(void)
{
    if(Lock.status!=MAINLOCK_CLOSEED)       //已关门状态下不再关门
        Lock.option = MAINLOCK_CLOSE_CMD;
    return 0;
}
//叉舌
uint8_t Lock_Slot_Scan(void)
{
    uint8_t rev = 0;
    static uint8_t staLast=0,staNow=0;
#ifdef PLATFORM
    //POWER ON
    staNow = SLOT_STATUS();
    //POWER OFF
#endif   
    if(staNow != staLast)
    {
       staLast = staNow;
       if(staNow==1)
         {rev =1; Lock.slot_status = SLOT_DOOR_CLOSEED;}
       else 
         {rev =2; Lock.slot_status = SLOT_DOOR_OPENED;}
       Lock.slot_act = true;
    }
    else 
        rev = 0;
	return rev;    
}
uint8_t LockProceess(void)
{
    static uint8_t step=0;
    static uint16_t Tcount=0;               //延时
    static uint8_t DelayStep=0;                
    static uint8_t lastStatus=MAINLOCK_DEFAUL;
    //----------------------------------------
    static bool CloseDu = false;            //临时标记
    static bool OpenDu = false;             //临时标记
    static bool dao = false;                //到位标记
    static bool du = false;                 //堵转标记
    static uint8_t DuCount=0;               //堵转次数
    static uint16_t TimeOut=0;              //空转超时
    //----------------------------------------
    switch(step)
    {
        //----------------------------------------
        //等待状态
        case 0:
             if(Lock.option==MAINLOCK_CLOSE_CMD)
             {
                du = dao = OpenDu = CloseDu = false;    //初始化
                DuCount=TimeOut=0;                      //初始化
                step = 1;
                Power_LockSensorControl(true);          //上电
             }
             else if(Lock.option==MAINLOCK_OPEN_CMD)
             {
                du = dao = OpenDu = CloseDu = false;    //初始化
                DuCount=TimeOut=0;                      //初始化
                step = 2;
                Power_LockSensorControl(true);          //上电
             }
            Tcount =0; DelayStep = 0;                   //延时用
        break;
        //----------------------------------------
        //伸出上锁状态
        case 1:
                if(DelayStep==0)
                {
                    if(++Tcount > (500 / TIME_BASIC_MS)) //500ms等待 延长寿命
                    {
                        Tcount = 0;
                        DelayStep = 1;
                        LOCK_HAL_MOTOR_LEFT();            //伸出
                        Lock.status =  MAINLOCK_CLOSEING; //正在伸出
                        TimeOut = 0;                      //清超时
                        du =false; dao = false;           //反方向转重清标记
                    }                   
                }
                else if(DelayStep==1)
                {
                    if(++Tcount > (100 / TIME_BASIC_MS))   //100ms  刚转动一刻电流过大
                    {
                        Tcount = 0;
                        DelayStep = 2;
                    }  
                }
                else 
                {
                    if((SENSOR_LOCKED_GPIO)==0)             //伸到位? 光偶  //伸到光偶顶住还要伸到最大电流  特别第一次上电
                    {
                       dao = true;                          //已到位
                    }
                    if (AdcHal_InitChange(1))
                    {
                        if (AdcHal_ReadResult(1) > MOTOR_MAX_ADC_VALUE) //是否堵转?
                        {
                            du = true;                      //已堵        
                        }
                    }
                    if((dao==true) && (du==true))               //到 而且 堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        step = 3;                           //跳到回拖状态
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du==true))         //不到 但堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        CloseDu = true;                     //临时标记
                        if(++DuCount>(MAINLOCK_TRY-1))      //堵转次数
                           step = 4;                        //跳到异常失败状态
                        else 
                           step = 5;                        //跳到非正常堵转反转状态
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du ==false))       //不到 不堵
                    {
                        if(++TimeOut>MAINLOCK_TIMEOUT)
                        {
                            LOCK_HAL_MOTOR_BRAKE();         //刹
                            LOCK_HAL_MOTOR_STOP();          //停
                            Lock.status = MAINLOCK_TIMEOUT_EER;  //电机运行超时异常
                            step = 0;                       //跳到初始化状态
                            Lock.option = MAINLOCK_NULL_CMD;//清空命令
                        }
                    }
                    else                                    //到 但不堵 （用不到）
                    {
                        TimeOut = 0;
                    }  
                }
        break;
        //----------------------------------------
        //缩入开门状态
        case 2:
                if(DelayStep==0)
                {
                    if(++Tcount > (500 / TIME_BASIC_MS)) //500ms等待 延长寿命
                    {
                        Tcount = 0;
                        DelayStep = 1;
                        LOCK_HAL_MOTOR_RIGHT();           //缩进
                        Lock.status = MAINLOCK_OPENING;   //正在缩入 
                        TimeOut = 0;                      //清超时
                        du =false; dao = false;           //反方向转重清标记
                    }
                }
                else if(DelayStep==1)
                {
                    if(++Tcount > (100 / TIME_BASIC_MS))   //100ms  刚转动一刻电流过大
                    {
                        Tcount = 0;
                        DelayStep = 2;
                    }  
                }
                else 
                {
                    if((SENSOR_UNLOCK_GPIO)==0)             //缩到位? 机械
                    {
                        dao = true;
                    }
                    if (AdcHal_InitChange(1))
                    {
                        if (AdcHal_ReadResult(1) > MOTOR_MAX_ADC_VALUE) //是否堵转?
                        {
                            du = true;                      //已堵        
                        }
                    }
                    
                    if((dao==true) && (du==true))               //到 而且 堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        step = 3;                           //跳到回拖状态
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du==true))         //不到 但堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        OpenDu = true;                      //临时标记
                        if(++DuCount>(MAINLOCK_TRY-1))      //堵转次数
                           step = 4;                        //跳到异常失败状态
                        else 
                           step = 5;                        //跳到非正常堵转反转状态
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du ==false))       //不到 不堵
                    {
                        if(++TimeOut>MAINLOCK_TIMEOUT)
                        {
                            LOCK_HAL_MOTOR_BRAKE();         //刹
                            LOCK_HAL_MOTOR_STOP();          //停
                            Lock.status = MAINLOCK_TIMEOUT_EER;  //电机运行超时异常
                            step = 0;                       //跳到初始化状态
                            Lock.option = MAINLOCK_NULL_CMD;//清空命令
                        }
                    }
                    else                                    //到 但不堵 （用不到）
                    {
                        TimeOut = 0;
                    }
                }
         break;
        //----------------------------------------
        //回拖状态
        case 3: 
                if(DelayStep==0)
                {
                    if(++Tcount > (500 / TIME_BASIC_MS))        //500ms等待 延长寿命
                    {
                        Tcount = 0;
                        DelayStep = 1;
                        TimeOut = 0;
                        if(Lock.status == MAINLOCK_CLOSEING)   //相反方向回拖
                        {
                            LOCK_HAL_MOTOR_RIGHT();             //反向缩进
                        }
                        else if(Lock.status == MAINLOCK_OPENING)//相反方向回拖
                        {
                            LOCK_HAL_MOTOR_LEFT();              //反向伸出
                        } 
                    }
                }
                else 
                {
                    if((SENSOR_BACK_GPIO)==0)               //回拖是否到位 必须要检测到//出现低电平
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停    
                        if(Lock.option == MAINLOCK_CLOSE_CMD) 
                        {
                            Lock.status = MAINLOCK_CLOSEED; //伸出到位
                        }
                        else if(Lock.option == MAINLOCK_OPEN_CMD)
                        {
                            Lock.status = MAINLOCK_OPENED;  //缩入到位
                        }
                        step =0;                            //跳到初始化状态 
                        Lock.option = MAINLOCK_NULL_CMD;    //清空命令
                    }
                    else 
                    {
                        if(++TimeOut>MAINLOCK_TIMEOUT)
                        {
                            LOCK_HAL_MOTOR_BRAKE();         //刹
                            LOCK_HAL_MOTOR_STOP();          //停
                            Lock.status = MAINLOCK_TIMEOUT_EER;  //电机运行超时异常
                            step = 0;                       //跳到初始化状态
                            Lock.option = MAINLOCK_NULL_CMD;   //清空命令
                        }
                    }                
                }
        break;
        //----------------------------------------
        //异常失败状态 
        case 4: 
                if((OpenDu==true)&&(CloseDu==true))      //正反转都堵
                {
                    Lock.status = MAINLOCK_ALL_EER;      //伸出缩进都堵异常
                }
                else if((OpenDu==false)&&(CloseDu==true))
                {
                    Lock.status = MAINLOCK_CLOSE_EER;    //伸出堵异常  
                }
                else if((OpenDu==true)&&(CloseDu==false))
                {
                    Lock.status = MAINLOCK_OPEN_EER;    //缩进堵异常
                }
                step = 0;                               //跳到初始化状态
                Lock.option = MAINLOCK_NULL_CMD;        //清空命令
        break;
        //----------------------------------------
        //非正常堵转时重新尝试状态 不回拖直接反转
        case 5:
               if(DelayStep==0)
                {
                    if(++Tcount > (300 / TIME_BASIC_MS)) //300ms等待 延长寿命
                    {
                        Tcount = 0;
                        DelayStep = 1;
                        TimeOut = 0;                      //清超时
                        du =false; dao = false;           //清标记
                        if(Lock.status == MAINLOCK_CLOSEING)   //相反方向回拖
                        {
                            LOCK_HAL_MOTOR_RIGHT();             //反向缩进
                        }
                        else if(Lock.status == MAINLOCK_OPENING)//相反方向回拖
                        {
                            LOCK_HAL_MOTOR_LEFT();              //反向伸出
                        } 
                    }
                }
                else if(DelayStep==1)
                {
                    if(++Tcount > (100 / TIME_BASIC_MS))   //100ms  刚转动一刻电流过大
                    {
                        Tcount = 0;
                        DelayStep = 2;
                    }  
                }
                else 
                {
                    if(Lock.status == MAINLOCK_CLOSEING) 
                    {
                        if((SENSOR_UNLOCK_GPIO)==0)             //缩到位? 机械
                        {
                            dao = true;
                        }
                    }
                    else if(Lock.status == MAINLOCK_OPENING)
                    {
                        if((SENSOR_LOCKED_GPIO)==0)             //伸到位? 光偶 
                        {
                           dao = true;                          
                        }
                    }
                    if (AdcHal_InitChange(1))
                    {
                        if (AdcHal_ReadResult(1) > MOTOR_MAX_ADC_VALUE) //是否堵转?
                        {
                            du = true;                      //已堵        
                        }
                    }
                    
                    if((dao==true) && (du==true))               //到 而且 堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        if(Lock.option == MAINLOCK_CLOSE_CMD) 
                        {
                            step = 1;                       //跳回原来过程状态 
                        }
                        else if(Lock.option == MAINLOCK_OPEN_CMD)
                        {
                            step = 2;                        //跳回原来过程状态 
                        }           
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du==true))         //不到 但堵
                    {
                        LOCK_HAL_MOTOR_BRAKE();             //刹
                        LOCK_HAL_MOTOR_STOP();              //停
                        if(Lock.status == MAINLOCK_CLOSEING) 
                        {
                            OpenDu = true;                      //临时标记
                        }
                        else if(Lock.status == MAINLOCK_OPENING)
                        {
                            CloseDu =true;
                        }
                        step = 4;                           //跳到异常失败状态
                        Tcount =0; DelayStep = 0;           //延时用
                    }
                    else if((dao==false) && (du ==false))       //不到 不堵
                    {
                        if(++TimeOut>MAINLOCK_TIMEOUT)
                        {
                            LOCK_HAL_MOTOR_BRAKE();         //刹
                            LOCK_HAL_MOTOR_STOP();          //停
                            Lock.status = MAINLOCK_TIMEOUT_EER;  //电机运行超时异常
                            step = 0;                       //跳到初始化状态
                            Lock.option = MAINLOCK_NULL_CMD;//清空命令
                        }
                    }
                    else                                    //到 但不堵 （用不到）
                    {
                        TimeOut = 0;
                    }
                }
        break;
        //----------------------------------------
        default:
        break;
    }
    //----------------------------------------
    if(Lock.status != lastStatus)
    {
        lastStatus = Lock.status;
        Lock.act = true;
    }
    //----------------------------------------
    Lock_Slot_Scan();                  //叉舌
    //----------------------------------------
}
//====================================================================================


/* [] END OF FILE */
