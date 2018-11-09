#include "Voice.h"

#ifdef PLATFORM
    
#define VOICE_DATA(x)   VOICE_DATA_Write(x)
#define VOICE_BUSY(x)   VOICE_BUSY_Write(x)
#define VOICE_STATUS()  VOICE_BUSY_Read()
      
#define DELAY_US(x)     CyDelayUs(x)
#define DELAY_MS(x)     CyDelay(x)
    
#endif

static void Line_1A_WT588_EXT(uint16_t ddata);

static uint8_t voice_status;

int8_t Voice_init(void)
{
#ifdef PLATFORM
	//SPI DMA 或 IO
    VOICE_POWER(ON);
#endif
	return 0;
}

void Voice_play(uint16_t val)
{	   
    Line_1A_WT588_EXT(val);
#if 0
    if(Function_status()->ManageMode.Language==CHN)
    {
        Line_1A_WT588_EXT(val);
    }
    else if(Function_status()->ManageMode.Language==EN)
    {
        Line_1A_WT588_EXT(val+200);
    }
#endif
}

int8_t Voice_process()
{	
#ifdef PLATFORM 
 
#endif
    return 0;
}

void Line_1A_WT588_EXT(uint16_t ddata)
{
    uint8_t i;
    uint8_t B_DATA=0;
    uint16_t data_high=0,data_low=0;
    data_high = ddata/256;
    data_low = ddata%256;

    VOICE_DATA(0);
    DELAY_MS(1); //延时 5ms
    B_DATA = data_high & 0x0001;
    voice_status = 1;
    for(i=0;i<8;i++)
    {       
        if(B_DATA==0)
        { /*表示逻辑电平 0*/
            VOICE_DATA(1); //拉高数据传输线，准备传输数据
            DELAY_US(200); // 延时 200us
            VOICE_DATA(0);
            DELAY_US(600); // 延时 600us
        } 
        else
        { /*表示逻辑电平 1*/
            VOICE_DATA(1); //拉高数据传输线，准备传输数据
            DELAY_US(600); // 延时 600us
            VOICE_DATA(0);
            DELAY_US(200);  // 延时 200us
        }
        data_high = data_high>>1;
        B_DATA = data_high & 0x0001;
    } 
    
    VOICE_DATA(1);
    DELAY_MS(1);
    VOICE_DATA(0);
    DELAY_MS(3); //T3 参数
    
    B_DATA = data_low & 0x0001;
    for(i=0;i<8;i++)
    {       
        if(B_DATA==0)
        { /*表示逻辑电平 0*/
            VOICE_DATA(1); //拉高数据传输线，准备传输数据
            DELAY_US(200); // 延时 200us
            VOICE_DATA(0);
            DELAY_US(600); // 延时 600us
        } 
        else
        { /*表示逻辑电平 1*/
            VOICE_DATA(1); //拉高数据传输线，准备传输数据
            DELAY_US(600); // 延时 600us
            VOICE_DATA(0);
            DELAY_US(200);  // 延时 200us
        }
        data_low = data_low>>1;
        B_DATA = data_low & 0x0001;
    } 
    VOICE_DATA(1);    
    DELAY_US(500);
    voice_status = 0;
}

int8_t Voice_Status()
{
    if (voice_status)
    {
        return VOICE_ST_BUSY;
    }
    else
    {
        if (VOICE_STATUS() == 1)
        {
            return VOICE_ST_IDLE;
        }
        else
        {
            return VOICE_ST_BUSY;    
        }
    }
}

//void playv()
//{
//    uint16_t i;
//    for (i=0;i<366;i++)
//    {
//        Line_1A_WT588_EXT(i);
//        while(1)
//        {
//            if (Voice_Status()==VOICE_ST_IDLE)
//            {
//                break;
//            }
//        }
//    }
//}

//void VoiceProcess(bool intterupt)
//{
//    if(intterupt)
//    {
//        play()
//        clrFifo
//    }
//    else
//    {
//        if(Voice_Status()==VOICE_ST_IDLE)
//        {
//            play()
//        }
//        else
//        {
//            ;//存入FIFO
//        }
//    }
//    if(Fifo!=NULL)
//    {
//        if(Voice_Status()==VOICE_ST_IDLE)
//        {
//            play()
//        }   
//    }
//}
