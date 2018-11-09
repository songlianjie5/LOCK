#include "SwitchKey.h"

static SwitchKey_t SwitchKey;

#ifdef PLATFORM

#define GPIO_0_PS   (* (reg32 *) CYREG_GPIO_PRT0_PS)
#define GPIO_1_PS   (* (reg32 *) CYREG_GPIO_PRT1_PS)
#define GPIO_2_PS   (* (reg32 *) CYREG_GPIO_PRT2_PS)
#define GPIO_3_PS   (* (reg32 *) CYREG_GPIO_PRT3_PS)
#define GPIO_4_PS   (* (reg32 *) CYREG_GPIO_PRT4_PS)
#define GPIO_5_PS   (* (reg32 *) CYREG_GPIO_PRT5_PS)
#define GPIO_6_PS   (* (reg32 *) CYREG_GPIO_PRT6_PS)
#define GPIO_7_PS   (* (reg32 *) CYREG_GPIO_PRT7_PS)
    
#define   LOCK_BNT_STATUS()    (uint8_t)((GPIO_0_PS & (1<<PIN1)) >> PIN1) //一键开关门键
#define   LOCK_MODE_STATUS()   (uint8_t)((GPIO_0_PS & (1<<PIN2)) >> PIN2) //手自动模式按键
//#define   SLOT_STATUS()        (uint8_t)((GPIO_0_PS & (1<<PIN3)) >> PIN3) //锁体叉舌
#define   RESET_KEY_STATUS()   (uint8_t)((GPIO_1_PS & (1<<PIN2)) >> PIN2) //复位按键
#define   COVER_STATUS()        GPIO_ReadPin(GPIO1,PIN6) //滑盖

#define   SwitchKey_Val()      (LOCK_BNT_STATUS()|(RESET_KEY_STATUS()<<1))
#endif

SwitchKey_t *SwitchKey_Status(void)
{
    return &SwitchKey;
}

void SwitchKey_init(void)
{
    SwitchKey_Mode_Scan();                                                          //开机扫一次 初始化状态
    SwitchKey.ModeSwAct = true;                                                     //清手自动活动标记
    SwitchKey.ConverAct = false;                                                    //清滑盖活动标记
}
//====================================================================================
uint8_t SwitchKey_Scan(void)
{
    uint8_t rev = 0;

    static uint16_t newkey = 0, oldkey = 0;
	static uint8_t debug = 0;
    static bool fallflag = false;
    static uint16_t presstime = 0; 

    static uint8_t edgnum = 0;
    static uint16_t doubletime = 0;
	
#ifdef PLATFORM
	newkey = SwitchKey_Val()&0x03;								                        //IO输入
#endif	
    if(newkey != oldkey)
    {
        if(++debug >(10/TIME_BASIC_MS))						                            //20ms消抖
        {
            debug = 0;
            oldkey = newkey;

            if(newkey == 0x03) 				                                            //上升沿
            {
                fallflag = false;
                //-------------------------------------------
#if 0           //稀放检测判断
                if(presstime < 100) 		//点按
                {
                    rev = true;
                }
                else if(presstime >= 300) 	//长按 3000ms
                {
                    rev = true;
                }
#endif
                //-------------------------------------------
                presstime = 0;				                                            //清
            }
            else							                                            //下降沿
            {
                //rev = true;					//输出
				fallflag = true;
				edgnum++; 					                                            //边沿数自加
				if ((newkey&0x01) == 0x00)                                              //一键开关门键
				{
					rev = LOCK_BNT_KEY|CLICK;                                           //一键开关门键单击
                    SwitchKey.DoorOneKey = LOCK_BNT_KEY|CLICK;   
				}
            }
        }
    }
	else
	{
		//-------------------------------------------
		if(fallflag)
		{
			if(++presstime == (3000/TIME_BASIC_MS)) 			                        //长按
			{
				if ((newkey&0x02) == 0x00)                                              //长按复位按
				{
					rev = RESET_KEY;
                    SwitchKey.ResetKeyVal = RESET_KEY;                   
				}
                else if((newkey&0x01) == 0x00)                                          //长按开关键                                                 
                {
                    rev = (LOCK_BNT_KEY|LONG_CLICK);  
                    SwitchKey.DoorOneKey = (LOCK_BNT_KEY|LONG_CLICK); 
                }
			}
		}
		debug = 0;
	}  
    
    //-------------------------------------------
	if(edgnum > 0)
	{
		if(++doubletime > (500/TIME_BASIC_MS)) 			                                //1000ms以内为双击事件
		{
			doubletime = 0;
			edgnum = 0;
		}
		else if(edgnum > 1) 			                                                //1000ms以内有2个下降沿
		{
			doubletime = 0;
			edgnum = 0;
			if ((newkey&0x01) == 0x00)                                                  //一键开关门键双击
			{ 
				rev = (LOCK_BNT_KEY|DOUBLE_CLICK);
                SwitchKey.DoorOneKey = (LOCK_BNT_KEY|DOUBLE_CLICK); 
			}
		}
	}
    //-------------------------------------------      
    return rev;	
}

//====================================================================================
//滑盖
uint8_t SwitchKey_Cover_Scan(void)
{
    static uint8_t newlevel=0,oldlevel=0;
    
    GPIO_SetPinDriveMode(GPIO1,PIN6,DM_RES_UP);                                         //IO输入
    GPIO_WritePin(GPIO1,PIN6,1);
//    CyDelayUs(50);                                                                    //这个要命啊 去掉电容试试 去掉这个延时
    newlevel = COVER_STATUS();                                                          //盖上是0 打开是1
    GPIO_SetPinDriveMode(GPIO1,PIN6,DM_ALG_HIZ);                                        //IO高阻　

    if(oldlevel != newlevel)
    {
       SwitchKey.ConverAct = true;                                                      //有动作
       oldlevel = newlevel;
       if(newlevel)                                                                     //在深度睡眠下只有上滑打开才唤醒
            Function_status()->Wakeup = true;                                           //滑盖唤醒
    }
    
    if(newlevel) 
    {
        SwitchKey.ConverStatus = COVER_OPEN;                                            //更新状态
        return COVER_OPEN;
    }
    else 
    {
        SwitchKey.ConverStatus = COVER_CLOSE;                                           //更新状态
        return COVER_CLOSE;
    }
}

//====================================================================================
//模式开关
uint8_t SwitchKey_Mode_Scan()
{
    uint8_t rev = 0;
    static uint8_t staLast=0,staNow=0;
#ifdef PLATFORM
    staNow = LOCK_MODE_STATUS();                                                      //1为自动 0为手动
#endif   
    if(staNow != staLast)
    {
       staLast = staNow; 
       //Function_status()->Wakeup = true;
       SwitchKey.ModeSwAct = true;                                                    //有活动
    }

    if(staNow==1)                                                                    //输出状态
        SwitchKey.ModeSwStatus = rev = AUTO_MODE;
    else 
        SwitchKey.ModeSwStatus = rev = MANUAL_MODE;
    
	return rev; 
}
//====================================================================================