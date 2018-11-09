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
#include "main.h"

int main(void)
{
    uint8_t key;
    CyGlobalIntEnable; /* Enable global interrupts. */
    CySysWdtDisable();
    //---------------------------------------------------------------------
    Record_InitSet();
    //---------------------------------------------------------------------
    FL3225_Init(); 
    //---------------------------------------------------------------------
    Led_Init();
    //---------------------------------------------------------------------
    Rtc_init();
    //---------------------------------------------------------------------
    Card_Init();
    //---------------------------------------------------------------------
    Voice_init();
    //---------------------------------------------------------------------
    Lock_Init(1);
    //---------------------------------------------------------------------
    Display_init();	
    //---------------------------------------------------------------------
    Tuch_init();
    //---------------------------------------------------------------------
    Fingerprint_init();
    //---------------------------------------------------------------------
    Function_init();
    //---------------------------------------------------------------------
    Voice_play(HAUN_YING_YIN);
    //---------------------------------------------------------------------
    WDT_Init();       
    //---------------------------------------------------------------------
    for(;;)
    {
        Wdt_Feed_Dog();                                     //清看门狗
    	switch(Function_status()->workstate)
    	{
    		case INIT:	
    			Function_status()->workstate = SLEEP;       //浅度睡眠
    			break;
    		case SLEEP:			
                TEST_IO_Write(!TEST_IO_Read());             //IO测试
    			
                Lock_Task();                                //锁体
                SwitchKey_Cover_Scan();                     //滑盖检测
                key = SwitchKey_Scan();                     //一键锁定、复位按键
                Tuch_Scan();                                //触摸扫描
                if(SwitchKey_Slot_Scan())                   //锁舌检测
                {
                    LockHal_Locked();
                } 
    			FunctionProcess();		                    //主功能模块
                Led_Process();                              //LED模块
                
//            	if (key > 0)
//            	{
//            		Function_status()->sleeptimeout = 0;  
//            		switch(key)
//            		{
//            		case LOCK_BNT_KEY:
//                        if (Lock_Status()->door_status == LOCK_MSG_OPEN_DOOR)
//            			    LockHal_Locked();    
//                        I2C_WriteReg(FL3235_REG_PWM_OUT8, 0xFF);\
//                        I2C_WriteReg(FL3235_REG_CTRL_OUT8, I_MAX_1_4|OFF);
//                        I2C_WriteReg(FL3235_REG_PWM_OUT9, 0xFF);\
//                        I2C_WriteReg(FL3235_REG_CTRL_OUT9, I_MAX_1_4|ON);
//            	        FL3225_DataUpdate();
//            			break;
//            		case LOCK_BNT_KEY|DOUBLE_CLICK:
//            			LockHal_Unlock();
//                        I2C_WriteReg(FL3235_REG_PWM_OUT8, 0xFF);\
//                        I2C_WriteReg(FL3235_REG_CTRL_OUT8, I_MAX_1_4|ON);
//                        I2C_WriteReg(FL3235_REG_PWM_OUT9, 0xFF);\
//                        I2C_WriteReg(FL3235_REG_CTRL_OUT9, I_MAX_1_4|OFF);
//            	        FL3225_DataUpdate();
//            			break;
//            		case RESET_KEY:
//            			Voice_play(YI_HUI_FU_DAO_CHU_CHANG_SHE_ZHI);//已恢复到出厂设置
//            			break;
//            		default:
//            			break;
//            		}
//            	}
                 	   

                do
                {
                   CySysPmSleep();		                    //浅度睡眠
                }while(Function_status()->Tick == false);
                Function_status()->Tick = false;
                
                if(Function_status()->workmode==DEEP_SLEEP) //只有返回到正常模式下才休眠计时   
                {
                    Function_status()->Wakeup = false;
                    Function_Sleep();
                }                
    			break;	
    		case DEEP_SLEEP:
                TEST_IO_Write(!TEST_IO_Read());             //IO测试
                
                Sleep_SearchCard();                         //低功耗检卡
                CapSense_LowPower();                        //深度睡眠下触摸接近检测
                SwitchKey_Cover_Scan();                     //滑盖检测
                
                if(Tuch_status()->even || Function_status()->Wakeup || Card_Status()->CardAct)
                {
                    Tuch_status()->even = false;            //触摸唤醒后不动作
                    
    				Function_status()->sleeptimeout = 0;
    				Function_status()->workstate = SLEEP;
                    Function_status()->Wakeup = false;
                    Function_Wakup();  
                }
                
                do
                {
                   EnterDeepSleepLowPowerMode();            //深度睡眠
                }while((WDT_Status()->Isr == false) && (Function_status()->Wakeup==false));
                WDT_Status()->Isr = false;                  //清看门狗中断标记
                
    			break;	
    		default:Function_status()->workstate = INIT;    //其它情况都跳到初始化    
                break;
    	}   
    }
}

/* [] END OF FILE */
