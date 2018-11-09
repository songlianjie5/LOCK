#include "Bat.h"

#define BAT_FULL    (6000)//满电电压6000mv
#define BAT_ZERO    (4800)//电池用尽时电压4800mv
#define NUMERIC     (100)//百分比
#define RES_UP      (2000)//分压电阻上，单位千欧姆
#define RES_DOWN    (1000)//分压电阻下

static uint16_t VolBuf[3];
static uint16_t voltmp = 0;

extern ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel);
extern uint16_t AdcHal_ReadResult(uint8_t chan);

void Bat_Init()
{
#ifdef PLATFORM
	uint8_t i;

    Opamp_Bat_Start();
    if (AdcHal_InitChange(0))
    {
        voltmp = ADC_CountsTo_mVolts(0,AdcHal_ReadResult(0));
    } 
    Opamp_Bat_Stop();
	for(i=0;i<sizeof(VolBuf);i++)VolBuf[i]=voltmp;
#endif	
}
//call in 500 ms
uint8_t Bat_Numeric()//电量百分比
{
    static uint8_t call=0;
    uint8_t numeric=0;
    int tmp1;
    float tmp2;
    
#ifdef PLATFORM
    if(++call>8)
    {
        call  = 0;
        Opamp_Bat_Start();
        if (AdcHal_InitChange(0))
        {
            voltmp = ADC_CountsTo_mVolts(0,AdcHal_ReadResult(0));
        } 
        Opamp_Bat_Stop();
    	
        if(VolBuf[2]>voltmp)
        {
            if((VolBuf[2]-voltmp) > (uint16_t)((float)VolBuf[2]*0.02))//变小2% 按1%来变
            {
                voltmp = VolBuf[2]-(uint16_t)((float)VolBuf[2]*0.01); 
            }        
        }
        else
        {
            if((voltmp-VolBuf[2]) > (uint16_t)((float)VolBuf[2]*0.02))//变大于2% 按1%来变
            {
                voltmp = VolBuf[2]+ (uint16_t)((float)VolBuf[2]*0.01); 
            }         
        }
        VolBuf[0] = VolBuf[1];
    	VolBuf[1] = VolBuf[2];
    	VolBuf[2] = voltmp;                        
    	voltmp =(VolBuf[1] +VolBuf[0])>>1; 
    }
#endif
    
    tmp1 = (int)(voltmp*(float)((RES_UP+RES_DOWN)/RES_DOWN)) - BAT_ZERO;//低于4V电量为0
    
    if (tmp1 <= 0)
    {
        return 0;
    }

    tmp2 = (float)((float)tmp1/(BAT_FULL-BAT_ZERO));
    
    if (tmp2 >= 1)
    {
        numeric = NUMERIC;    
    }
    else
    {
        numeric = (uint8_t)(tmp2*NUMERIC);    
    }
    
    return numeric;    
}


