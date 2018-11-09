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
//#include "你只管努力，剩下的就交给天意"

int main(void)
{
    CyGlobalIntEnable;                                                      /* Enable global interrupts. */
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
    //Lock_Init(1);
    Lock_Init();
    //---------------------------------------------------------------------
    Voice_init();
    //---------------------------------------------------------------------
    Card_Init();
    //---------------------------------------------------------------------
    Display_init();	
    //---------------------------------------------------------------------
    Tuch_init();
    //---------------------------------------------------------------------
    SwitchKey_init();
    //---------------------------------------------------------------------
    Fingerprint_init();
    //---------------------------------------------------------------------
    Function_init();
    //---------------------------------------------------------------------
    Extend_Init();
    //---------------------------------------------------------------------
    Bat_Init();
    //---------------------------------------------------------------------
    WDT_Init();       
    //---------------------------------------------------------------------
    GPIO_SaveStat();                                                        //保存所有GPIO寄存器的设置
    //---------------------------------------------------------------------
    Voice_play(HAUN_YING_YIN);
    //---------------------------------------------------------------------
    for(;;)
    {
        Wdt_Feed_Dog();                                                     //清看门狗
    	switch(Function_status()->McuState)
    	{
    		case INIT:                                                      //初始化，这里不用到	
    			Function_status()->McuState = SLEEP;                        //浅度睡眠
    			break;
    		case SLEEP:
                //---------------------------------------------------------
                //TEST_Write(!TEST_Read());                             //IO测试
                //ListRun(&List);                                             //每隔 BASE_TICK_MS 执行一次
    			
                //Lock_Task();                                                
                //LockProceess(Function_status()->NormalMode.AUTOMODE);       //锁体
                
                LockProceess();                                             //锁体
                
                SwitchKey_Cover_Scan();                                     //滑盖检测
                SwitchKey_Mode_Scan();                                      //模式开关
                
                if(SwitchKey_Status()->ConverAct)
                {
                   SwitchKey_Status()->ConverAct = false;                   //清除动作
                   if(SwitchKey_Status()->ConverStatus == COVER_CLOSE)
                        Function_status()->McuState = DEEP_SLEEP;          
                }
                
                SwitchKey_Scan();                                           //一键锁定、复位按键
                if (SwitchKey_Status()->ResetKeyVal == RESET_KEY)           //复位按键
                {
                    SwitchKey_Status()->ResetKeyVal = 0; 
                    DisplayTips(&TIPS_ZZHFQSH);                             //正在恢复，请稍后
                    Voice_play(ZHNEG_ZAI_HUI_FU_QING_SHAO_HOU);             //正在恢复，请稍后
                    ResteLockRecord();                                
                    Function_status()->TimeOutCount = 0;                    //清休眠计时
                    DisplayTips(&TIPS_YHFDCCSZ);                            //已恢复到出厂设置
                    Voice_play(YI_HUI_FU_DAO_CHU_CHANG_SHE_ZHI);            //已恢复到出厂设置
                }
                Tuch_Scan();                                                //触摸扫描
                
                if(Lock_Status()->slot_act)                                 //锁舌检测
                {
                    Lock_Status()->slot_act = false;
                    if(Lock_Status()->slot_status==SLOT_DOOR_CLOSEED)       //自动上锁
                    {
                        LockClose();
                    }
                }
//                if(SwitchKey_Slot_Scan())                                   //锁舌检测
//                {
//                    ;//LockHal_Locked();
//                } 
    			FunctionProcess();		                                    //主功能模块
                Led_Process();                                              //LED模块状态更新                  	   

                if(Function_status()->McuState == DEEP_SLEEP)               //由正常模式转换   
                {
                    Tuch_status()->even = false;                            //触摸唤醒后第一次不动作，只醒
                    Function_status()->Wakeup = false;                      //清唤醒标记，由IO中断，指纹中断置起
                    Card_Status()->CardAct = false;                         //清卡片唤醒标记
                    Function_Sleep();                                       //深度睡眠前的　配置
                }    
                
                do
                {
                   CySysPmSleep();		                                    //浅度睡眠
                }while(Function_status()->Tick == false);                   //Tick唤醒
                Function_status()->Tick = false;
    			break;
                //---------------------------------------------------------
    		case DEEP_SLEEP:
                //---------------------------------------------------------
                //TEST_Write(!TEST_Read());                               //IO测试
                
                Sleep_SearchCard();                                         //低功耗检卡,内已开始SPI及ADC 
                Tuch_LowpowerScan();                                        //深度睡眠下触摸接近检测
                SwitchKey_Cover_Scan();                                     //滑盖检测
                
                if(Tuch_status()->even || Function_status()->Wakeup || Card_Status()->CardAct)
                //if(Card_Status()->CardAct)                                  //测试卡片唤醒
                {
                    Tuch_status()->even = false;                            //触摸唤醒后第一次不动作，只醒
                    Function_status()->Wakeup = false;                      //清唤醒标记，由IO中断，指纹中断置起
                    Card_Status()->CardAct = false;                         //清卡片唤醒标记
                    
    				Function_status()->TimeOutCount = 0;                    //清休眠计时
    				Function_status()->McuState = SLEEP;                    //跳到浅度睡眠
                    Function_Wakup();                                       //深度转到浅度睡眠　配置　初始化一些外设
                    Function_status()->NormalMode.BAT_ALM = true;           //只报一次电量
                }
                do
                {
                   EnterDeepSleepLowPowerMode();                            //深度睡眠 只接受Wakeup标记与Wdt中断唤醒
                }while((WDT_Status()->Isr == false) && (Function_status()->Wakeup==false));
                WDT_Status()->Isr = false;                                  //清看门狗中断标记
    			break;	
                //---------------------------------------------------------
    		default:Function_status()->McuState = INIT;                     //其它情况都跳到初始化 实际不可能    
                break;
    	}   
    }
}

/* [] END OF FILE */
