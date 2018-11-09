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
#include "Function.h"

static  Function_t Function;

Function_t *Function_status(void)
{
    return &Function;
}

void FunctionProcess(void)
{
    switch(Function.workmode)
    {
        case NORMAL_MODE:
                NormalMode();
                break;
        case MANAGE_MODE:
                ManageMode();
                break;
        case LOCK_MODE: 
                LockMode();
                break;  
        default:
                Function.workmode=NORMAL_MODE;
                break;
    }
}

#ifdef PLATFORM
    
void Function_ISR1(void)
{
    uint8_t regdata;
	regdata = LOCK_INT_ClearInterrupt();//清除中断
    
    Function_status()->Wakeup = true; //唤醒测试    	
            
	isr_1_ClearPending();
}
void Function_ISR2(void)
{
    uint8_t regdata;
    regdata = GPIO_ISR1_ClearInterrupt();//清除中断
    
    if ((regdata&(0x01<<3))>>3)         //指纹中断
    {
         Fingerprint_status()->SensorIsr = true;     
    }

    Function_status()->Wakeup = true; //唤醒测试  
    
    isr_2_ClearPending();
}
#endif 

void Function_Sleep(void)
{
#ifdef PLATFORM     
//    GPIO_SaveStat(); 
//    
//    UART_0_FPC_Stop();
//    UART_1_BLE_Stop(); 
//    //UART_2_EXT_Stop();                //测试关闭
//    SPI_0_OLED_FLASH_Stop();
//    SPI_1_CARD_Stop();
//    ADC_Stop(); 
//    
//    GPIO_WritePin(GPIO1,PIN6,1);    //滑盖
//    GPIO_SleepSet();      
    UART_0_FPC_Stop();//关闭指纹串口
    
    UART_1_BLE_Stop();//关闭蓝牙串口
    
    UART_2_EXT_Stop();//关闭扩展串口
    
    SPI_0_OLED_FLASH_Stop();//关闭OLED的SPI接口
    
    SPI_1_CARD_Stop();//关闭刷卡的SPI接口
    
    ADC_Stop();//停止ADC
    
    CapSense_Sleep();//触摸睡眠前设置
    
    GPIO_SleepSet();//所有的GPIO睡眠前设置    
#endif    
}
void Function_Wakup(void)
{
#ifdef PLATFORM  
    GPIO_LoadStat();//GPIO状态恢复
    
    Led_Init();//LED初始化
    
    CapSense_Wakeup();//触摸唤醒恢复
    
    ADC_Start();//ADC初始化
    
    //FL3225_Init();  //电源初始化
    SN_SDB_PIN_High();//开硬件开关 替代FL3225_Init  休眠唤醒时会闪一下的问题
    
    Display_init();//显示初始化
    
    UART_0_FPC_Start();;//指纹初始化
    
    //Ble_Init();//蓝牙初始化
    
    Extend_Init();//扩展接口初始化
    
    Voice_init();//语音初始化
    
    Nxp520_Init();//刷卡芯片初始化TSC9820,包括SPI接口和9820寄存器初始化
	
	LOCK_POWER(ON);//打开锁体电源  
#endif   
}

void SystickCB(void)
{  
   Function.Tick = true;
   //if(Lock_Status()->Lock_run) TIM4_IRQHandler();  
   // LockHal_Scan();
}


//=====================================================================================
//功能体初始化，主要是初始化MCU工作状态，功能体模式，读取一些参数等，清空一些标记
//=====================================================================================
void Function_init(void)
{
    OptionRecord_t optionTemp;
    uint8_t p[12]={8,9,4,5,6,1,2,3,7,0,1,7};
    uint8_t userid;
    uint8_t testpsw[]={0x85,0x87,0x85,0x87,0x85,0x87,0x00,0x00,0x00,0x00,0x00,0x00};
    
    Function.TimeOutCount =0;
    Function.McuState = INIT;
    Function.Wakeup = false;
    Function.Tick = false;
    
    ReadOptionRecord(&optionTemp);                                  //读取系统设置
    Function_status()->ManageMode.Language = optionTemp.Language;
    Function_status()->ManageMode.Voice = optionTemp.Voice;
    Function_status()->ManageMode.OpenDoorMode = optionTemp.OpenDoorMode;
    
#ifdef PLATFORM  
    CySysTickStart();
    CySysTickSetReload(CYDEV_BCLK__SYSCLK__HZ/100);
    CySysTickSetCallback(1,SystickCB);
    
    isr_1_StartEx(Function_ISR1);
    isr_2_StartEx(Function_ISR2);
    
    Fingerprint_status()->NormalEven = F_NULL;                  //清空指纹动作标记 BUG
    Fingerprint_status()->SensorIsr = false;                    //清空指纹中断标记
#endif   
}


/* [] END OF FILE */
