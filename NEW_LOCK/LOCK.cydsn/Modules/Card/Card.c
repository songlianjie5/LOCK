#include "Card.h"
#include "nxp520.h"

#define _CARD_DEBUG     0

#define EVRBUFSIZE      6

static Card_t Card;

static uint16_t EvrBuf[EVRBUFSIZE];            //前6*600ms=3600ms才正常都在


/************内部函数声明****************/

static uint16_t Rf_Adc_Out();

static void Cal_Rf_Snr(uint16_t pcs);

static uint16_t maxmin(uint16_t *array,uint16_t len,uint16_t *maxval,uint16_t *minval);//计算数组中最大值和最小值的差值

static uint16_t Sleep_SearchGetAdc(void);

extern ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel);
extern uint16_t AdcHal_ReadResult(uint8_t chan);
/************内部函数实现****************/

static uint16_t Rf_Adc_Out()
{
    uint16_t tmp;
#ifdef PLATFORM 
    if (AdcHal_InitChange(2)==SUCCESS)
    {
        tmp = AdcHal_ReadResult(2);
    }
#endif
    return tmp;
}

static void Cal_Rf_Snr(uint16_t pcs)
{
#ifdef PLATFORM
    uint8_t i;
    
    if (pcs > EVRBUFSIZE)
        return;
    
    for (i=0;i<pcs;i++)
    {
        CyDelay(2);      
        EvrBuf[i] = Sleep_SearchGetAdc();
    }
    
    Card.Evr =(EvrBuf[5]+EvrBuf[4]+EvrBuf[3]+EvrBuf[2]+EvrBuf[1]+EvrBuf[0])/6 ;  

    CARD_RESET_Write(0);
    CARD_RESET_SetDriveMode(DM_STRONG);                                                             //设置9820 复位脚
    SPI_1_CARD_miso_m_SetDriveMode(DM_DIG_HIZ);                                                     //设置9820 SPI接口
    SPI_1_CARD_mosi_m_SetDriveMode(DM_STRONG);
    SPI_1_CARD_sclk_m_SetDriveMode(DM_STRONG);
    SPI_1_CARD_ss0_m_SetDriveMode(DM_STRONG);

    ADC_Start();                                                                                    //启动ADC
    
#endif
}

/******************外部函数实现*****************/
void Card_Init()
{
#ifdef PLATFORM       
    Cal_Rf_Snr(EVRBUFSIZE); 
    Nxp520_Init();  
#endif 
    Card.CardAct = false;
}
Card_t *Card_Status(void)
{
    return &Card;
}

//ST那套500 ms 一次2.5ms 执行时间  执行时电源纹波跳动250mv的峰峰值  与低功耗是同一函数不分开
int8_t Card_ReadId(uint8_t *cardid)
{ 
    int8_t rev = MI_NOTAGERR;
    uint8_t len;
    uint8_t type[2];
//    uint8_t str[20];
    static uint16_t delay = 0;

    static uint8_t LastID[11];
    static uint8_t same=0;    
    
    if (++delay >= (200/TIME_BASIC_MS))
    {   
        delay = 0;
        if (Lowpower_Request(PICC_REQALL,type) == MI_OK)
    	{
    	    UL_PcdAnticoll(cardid,&len);
            
            const uint8_t defaultKeyA[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            if(Nxp520_Auth_State(0x60, 2, (uint8_t *)defaultKeyA, cardid) == 0)
            { 
                if(memcmp(LastID,cardid,4))                                                 //对比
                {
                    if(++same>2)                                                            //多次对比相同
                    {
                        same = 0;
                        rev = MI_OK;
                        #if 0
                        char str[30];
                        Display_clear_screen();
                        sprintf((char*)str,"%X %X %X %X",cardid[0],cardid[1],cardid[2],cardid[3]);
                        Display_english_str(0,0,str,false);     
                        
                        
                        memset(str,0,sizeof(str));
                        //Nxp520_write(2,"123456789");
                        Nxp520_Read(2,str); 
                        Display_english_str(0,1,str,false); 
                        #endif
                    }
                }
                else
                {
                    memcpy(LastID,cardid,4);
                    same =0;
                }                
            } 
    	}
        Nxp520_Antenna_Off();    
    }
    return rev;
}

uint16_t Sleep_SearchGetAdc(void)
{
    uint16_t adc = 0;
    //==============================================================================================//以下执行340us左右
    CARD_RESET_SetDriveMode(DM_STRONG);                                                            //设置9820 复位脚
    CARD_RESET_Write(1);
    Opamp_Rf_Start();
    ADC_Start();
    SPI_1_CARD_miso_m_SetDriveMode(DM_DIG_HIZ);                                                     //设置9820 SPI接口
    SPI_1_CARD_mosi_m_SetDriveMode(DM_STRONG);
    SPI_1_CARD_sclk_m_SetDriveMode(DM_STRONG);
    SPI_1_CARD_ss0_m_SetDriveMode(DM_STRONG);
                                                                                    //启动ADC
    SPI_1_CARD_Start();
    CY_SET_REG32((void *)CYREG_HSIOM_PORT_SEL4,CY_GET_REG32(CYREG_HSIOM_PORT_SEL4)&(~0xF0000000));  //设置P4_7 CS脚为固件控制
//    CyDelay(1);
    Nxp520_Reset_Wait();                                                                            //等待复位完成
    
    //-----------------50uS时间
    Nxp520_Antenna_On();                                                                                        
    Nxp520_Antenna_Off();
//    CyDelayUs(10);
    adc = Rf_Adc_Out(); //测量
    Card.adc = adc;//测试
    SPI_1_CARD_Stop();                                                                              //关SPI
    
    ADC_Stop();                                                                                     //停ADC
    Opamp_Rf_Stop();
    CARD_RESET_Write(0);                                                                            //设置9820 复位脚
    CARD_RESET_SetDriveMode(DM_ALG_HIZ);                                                            //设置9820 复位脚 
    SPI_1_CARD_miso_m_SetDriveMode(DM_ALG_HIZ);                                                     //设置9820 SPI接口
    SPI_1_CARD_mosi_m_SetDriveMode(DM_ALG_HIZ);
    SPI_1_CARD_sclk_m_SetDriveMode(DM_ALG_HIZ);
    SPI_1_CARD_ss0_m_SetDriveMode(DM_ALG_HIZ);
    //==============================================================================================
    return adc;
}
//call in 600 ms
bool Sleep_SearchCard()
{
    bool rev = false;
    uint16_t adc = 0;
    static bool Update = false;
    uint8_t i;
    
#ifdef _UART_DEBUG    
    #if _CARD_DEBUG
        
        UART_2_EXT_rx_SetDriveMode(DM_DIG_HIZ);
        
        UART_2_EXT_tx_SetDriveMode(DM_STRONG);
        
        UART_2_EXT_Start(); //启用串口2
        
    #endif
#endif
    
    adc = Sleep_SearchGetAdc();//单次获取ADC值 执行340us左右
    
#ifdef _UART_DEBUG    
    #if _CARD_DEBUG  
        Rtc_t Rtc;
        
        Rtc_Get(&Rtc);
        
        printf("lowpoweradc:%d  Evr:%d  ",adc,Card.Evr);
        
        printf("%.4d-%.2d-%.2d  %.2d:%.2d:%.2d\r\n",Rtc.year, Rtc.month, Rtc.day,Rtc.hour, Rtc.minute, Rtc.second);
        
        CyDelay(10);
        
        UART_2_EXT_Stop();
        
        UART_2_EXT_rx_SetDriveMode(DM_ALG_HIZ);
        
        UART_2_EXT_tx_SetDriveMode(DM_ALG_HIZ);
    
    #endif
#endif
    
    if(adc<Card.Evr)
    {
        if((Card.Evr-adc) > (uint16_t)((float)Card.Evr*0.05))                                           //变小x%触发
        {
            if(adc > (uint16_t)((float)Card.Evr*0.30))                                                  //突变小30%以上，刷卡心片数字电路正常，模拟部分有问题 
            {
                for(i=0;i<3;i++)                                                                        //30ms最多再检测
                {
                    do
                    {
                       CySysPmSleep();		                                                            //浅度睡眠
                    }while(Function_status()->Tick == false);                                           //10ms Tick唤醒
                    Function_status()->Tick = false;                    
                    

                    adc = Sleep_SearchGetAdc();                                                         //单次获取ADC值 执行340us左右
                    
                    if(adc >= Card.Evr)                                                                 //这是个抖动
                        break;                                                                          //退出循环
                    else if((Card.Evr-adc) > (uint16_t)((float)Card.Evr*0.04) )                         //重新栓测多几次
                    {
                        Card.CardAct = true;                                                            //30ms都是一样时确认为有卡
                        rev = true;  
                    }
                    else 
                    {
                        Card.CardAct = false;                                                           //只要有一次抖动都示为干扰
                        rev = false;
                        break;                                      
                    }
                }
            }            
        }   
        else
        {
            Card.CardAct = false;
            rev = false;  
            Update = true;                                                                          //需要更新   
        }   
    }
    else                                                                                            //在没有卡情况下才更新
    {
        if((adc-Card.Evr) > (uint16_t)((float)Card.Evr*0.08))                                       //突然的高噪声 去掉
         adc = Card.Evr;
        
        Update = true;                                                                              //需要更新
    }
    
    if(Update==true)                                                                                //需要更新环境
    {
        Update = false;
        EvrBuf[0] = EvrBuf[1];
        EvrBuf[1] = EvrBuf[2];
        EvrBuf[2] = EvrBuf[3];
        EvrBuf[3] = EvrBuf[4];
        EvrBuf[4] = EvrBuf[5];
        EvrBuf[5] = adc;                        
        Card.Evr =(EvrBuf[5] +EvrBuf[4] +EvrBuf[3] +EvrBuf[2] +EvrBuf[1] +EvrBuf[0])/6 ;  
    }
    
    return rev;
}


