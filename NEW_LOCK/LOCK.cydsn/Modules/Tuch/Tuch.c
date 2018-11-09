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
#include "Tuch.h"

static Tuch_t Tuch;

int8_t Tuch_init(void)
{
#ifdef	PLATFORM  
    uint8_t i;
    CapSense_Start();
    
    for(i=0;i<20;i++)
    {
        CapSense_ScanAllWidgets();
        /* Wait till scan is complete */
        while(!(CapSense_NOT_BUSY == CapSense_IsBusy()));
        /* Update baseline and sensor status for both the button sensors */
        CapSense_ProcessAllWidgets();
    }  
#endif    
    return 0;
}

Tuch_t *Tuch_status(void)
{
	return &Tuch;
}
static uint16_t Tuch_read(void)
{
	uint16_t buttonStatus = 0;
#ifdef	PLATFORM
    uint8_t i;
    if(CapSense_NOT_BUSY==CapSense_IsBusy())
    {
        CapSense_ProcessAllWidgets();
        if(CapSense_IsAnyWidgetActive())
        {     
            for(i=0;i<CapSense_TOTAL_WIDGETS;i++)
            {
                if(CapSense_IsSensorActive(i,0) )
                    buttonStatus = buttonStatus | (0x0001<<i);
                else
                    buttonStatus = buttonStatus & (~(0x0001<<i));
            }
        }
        CapSense_ScanAllWidgets();    //Start Next scan
    }
    /* Wait till scan is complete */
    while(!(CapSense_NOT_BUSY == CapSense_IsBusy()))  //1ms总扫描时间
    {
        CySysPmSleep();
    }
#endif
	return (buttonStatus&0xFFF);
}
bool Tuch_Scan(void)
{
    bool rev = false;
    static uint16_t newkey = 0, oldkey = 0;
    static uint8_t debug = 0;
    static bool fallflag = false;
    static uint16_t presstime = 0; 
    
    newkey = Tuch_read();
    if(newkey != oldkey)
    {
        if(++debug > 3)	//40ms消抖
        {
            debug = 0;
            oldkey = newkey;

            if(newkey == 0) //上升沿
            {
                fallflag = false;
                presstime = 0;//清
                if(Tuch.val == TUCH_KEY_ENTER)            //NUM #为上升沿 输出        
                {
                    Tuch.val = TUCH_KEY_ENTER_UP;
                    Tuch.even= true; 
                }
            }
            else//下降沿
            {
                rev = true;		//输出
                fallflag = true;
                Tuch.even= true;
                switch(newkey)
                {
                    case 0x008:Tuch.val =TUCH_KEY1;break;//NUM 1
                    case 0x080:Tuch.val =TUCH_KEY2;break;//NUM 2    
                    case 0x100:Tuch.val =TUCH_KEY3;break;//NUM 3
                    case 0x002:Tuch.val =TUCH_KEY4;break;//NUM 4
                    case 0x040:Tuch.val =TUCH_KEY5;break;//NUM 5    
                    case 0x200:Tuch.val =TUCH_KEY6;break;//NUM 6
                    case 0x004:Tuch.val =TUCH_KEY7;break;//NUM 7
                    case 0x010:Tuch.val =TUCH_KEY8;break;//NUM 8    
                    case 0x400:Tuch.val =TUCH_KEY9;break;//NUM 9 
                    case 0x001:Tuch.val =TUCH_KEY_ESC;break;//NUM *
                    case 0x020:Tuch.val =TUCH_KEY0;break;//NUM 0
                    case 0x800:Tuch.val =TUCH_KEY_ENTER;break;//NUM # 这个上升沿才动作
                    default:Tuch.val = TUCH_KEY_NULL;break;
                }
                Tuch.val |= CLICK;						//单击
                
                //if(Tuch.val ==TUCH_KEY_ENTER)Tuch.even= false; //NUM #为上升沿 屏蔽
            }
        }
    }
    else
    {
        if(fallflag)
        {
            if(++presstime == (3000/TIME_BASIC_MS)) //长按 3000ms
            {
                rev = true;			//输出
			    Tuch.even = true;
                if(Tuch.val == TUCH_KEY_ENTER) //NUM # 长按
			         Tuch.val |= LONG_CLICK;					//长按
            }            
        }
        debug = 0;		//清
    }
    return rev;
}

bool Tuch_LowpowerScan(void)
{
#ifdef	PLATFORM
    CapSense_CSDSetupWidgetExt(CapSense_PROXIMITY0_WDGT_ID,CapSense_PROXIMITY0_SNS0_ID);
    CapSense_CSDScanExt();
    while(!(CapSense_NOT_BUSY == CapSense_IsBusy()))
    {
        /* If CapSense scannning is in progress, put CPU to sleep */
        CySysPmSleep();
    }
    CapSense_ProcessWidget(CapSense_PROXIMITY0_WDGT_ID);
    /* Update baseline and sensor status for both the button sensors */
    if(CapSense_IsSensorActive(CapSense_PROXIMITY0_WDGT_ID,0))
    {
        Tuch.even = true;
        return true;
    }
    else 
        return false;
#endif
}

/*******************************************************************************
* Function Name: EnterDeepSleepLowPowerMode
********************************************************************************
* Summary:
*  Put the device to DeepSleep power mode. Reconfigures the Components for 
*  normal operation after wake-up. 
*
* Parameters:
*  void
*
* Return:
*  void
*
* Theory: Before going to deep sleep, the API checks for any
*         I2C activity and waits till the I2C transaction is complete before 
*         the device is put to deep sleep. 
*
* Side Effects: None
*
* Note:
*
*******************************************************************************/
void EnterDeepSleepLowPowerMode(void)
{
    /* EZI2C_Sleep routine should be called only after on-going I2C 
    *  transaction is complete
    *  Enter critical section to lock slave state 
    */
    uint8_t interruptState;
    interruptState = CyEnterCriticalSection();
    
    /* Check if I2C is busy. If so, skip deep sleep until I2C is free */
    //if(!(EZI2C_EzI2CGetActivity() & EZI2C_EZI2C_STATUS_BUSY))
    {
        /* Prepare for deep sleep - stop ILO calibration */
        CySysClkIloStopMeasurement();
        
        /* Configure CapSense to be wakeup source */
        CapSense_Sleep();
        
        /* Configure slave to be wakeup source */
        //EZI2C_Sleep();
        
        //I2C_ST_Write(0); //EBS
        
    	/* Enter DeepSleep. */
    	CySysPmDeepSleep();
        
        /* Delay when wakeup from DeepSleep. */
        CyDelayUs(30);
        
        /* WDT or any I2C communication wakes up device from deep sleep. */
        
        /* Configure slave for active mode operation */
        //EZI2C_Wakeup();
        
        /* Configure CapSense for active mode operation */
        CapSense_Wakeup();
        
        /* Start the ILO accuracy measurement for accurate scan interval generation */
        CySysClkIloStartMeasurement();
    }
    /* Enable interrupts */
    CyExitCriticalSection(interruptState);
}


/* [] END OF FILE */
