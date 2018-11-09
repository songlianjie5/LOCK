#include "Led.h"

static Led_t Led;

uint8_t LED_tab[]={
    FL3235_REG_PWM_OUT18,       //0 
    FL3235_REG_PWM_OUT10,       //1
    FL3235_REG_PWM_OUT5,        //2 
    FL3235_REG_PWM_OUT24,       //3 
    FL3235_REG_PWM_OUT11,       //4 
    FL3235_REG_PWM_OUT7,        //5 
    FL3235_REG_PWM_OUT22,       //6 
    FL3235_REG_PWM_OUT13,       //7 
    FL3235_REG_PWM_OUT15,       //8 
    FL3235_REG_PWM_OUT16,       //9 
    FL3235_REG_PWM_OUT12,       //10 *
    FL3235_REG_PWM_OUT17,       //11 #
    
    FL3235_REG_PWM_OUT19,       //前面板 左下角，锁上
    FL3235_REG_PWM_OUT20,       //前面板 右下角，电量低符号
    FL3235_REG_PWM_OUT23,       //前面板 右上角 OPEN
    FL3235_REG_PWM_OUT1,        //前面板 左上角 CLOSE    
    FL3235_REG_PWM_OUT8,        //后面板绿OPEN
    FL3235_REG_PWM_OUT9         //后面板红CLOSE
};
void LED_switch(uint8_t num,uint8_t sta)
{
#ifdef PLATFORM
    if(num<sizeof(LED_tab))
    {
        I2C_WriteReg(LED_tab[num], Led.Luma);
    	I2C_WriteReg(LED_tab[num]+0x25, Led.Ia|sta);  
        FL3225_DataUpdate();
    }
#endif
}
const uint8_t XXX_tab[]={
1,0,1,
0,1,0,
1,0,1,    
0,0,0    
};
const uint8_t ALL_On_tab[]={
1,1,1,
1,1,1,
1,1,1,    
1,1,1    
};
const uint8_t ALL_Off_tab[]={
0,0,0,
0,0,0,
0,0,0,    
0,0,0    
};

void LED_play_tab(const uint8_t tab[])
{
#ifdef PLATFORM
    uint8_t i;
    I2C_WriteReg(LED_tab[0], Led.Luma);
	I2C_WriteReg(LED_tab[0]+0x25, Led.Ia|tab[10]);       //0
    for(i=1;i<11;i++)
    {
        I2C_WriteReg(LED_tab[i], Led.Luma);
    	I2C_WriteReg(LED_tab[i]+0x25, Led.Ia|tab[i-1]);  // 
    }
    I2C_WriteReg(LED_tab[11], Led.Luma);
	I2C_WriteReg(LED_tab[11]+0x25, Led.Ia|tab[11]);      //
    FL3225_DataUpdate();
#endif    
}

uint8_t Led_Init(void)
{
    Led.Ia = I_MAX_1_4;
    Led.Luma = 255;
    Led_Play(ALL_ON);
    return 0;
}

Led_t *Led_Status(void)
{
    return &Led;
}

void Led_Play(Led_e m)
{
    Led.Status = m;
}

void Led_Process(void)
{
    uint8_t Temp_tab[12];
	static uint16_t T_count=0;
    static uint8_t lastNum=0;
    static bool tog =false;
    static uint8_t upLin=0;
    
    static Led_e lastSta=IDEL;
    if(Led.Status != lastSta)
    {
        lastSta = Led.Status;
        T_count = 0;
        tog =false;
        upLin=0;
    }
    switch(Led.Status)
    {
        case ALL_ON:
                    LED_play_tab(ALL_On_tab);
                    Led.Status = IDEL;
                    break;
        case ALL_OFF:
                    LED_play_tab(ALL_Off_tab);
                    Led.Status = IDEL; 
                    break;
		case TUCH:
					if(T_count==0)
                    {
                        LED_switch(Led.Num,OFF);
                    }
                    if(Led.Num != lastNum)
                    {
                        LED_switch(lastNum,ON);
                        lastNum = Led.Num;
                    }
					if(++T_count>(60/TIME_BASIC_MS))
					{
						Led.Status = IDEL;
                        if(Led.Num<12)
                        LED_switch(Led.Num,ON);
					}
					break;
        case XXX:
					if(T_count==0)
                    {
                        LED_play_tab(XXX_tab);
                        tog=false;
                    }
					if(++T_count>(800/TIME_BASIC_MS))
					{
                        LED_play_tab(ALL_On_tab);
						Led.Status = IDEL;
					}
                    else if((T_count%(200/TIME_BASIC_MS))== 0)
                    {
                        tog = !tog;
                        if(tog)LED_play_tab(ALL_Off_tab);
                        else LED_play_tab(XXX_tab);
                    }
                    break;
        case UPDOWN:
                    if(T_count==0)upLin=0;
                    if((T_count++%(300/TIME_BASIC_MS))== 0)
                    {
                        memcpy(Temp_tab,ALL_Off_tab,12);
                        memset(&Temp_tab[upLin],1,3);
                        LED_play_tab(Temp_tab);
                        upLin+=3;
                        if(upLin>=12)
                        {
                            LED_play_tab(ALL_On_tab);
    						Led.Status = IDEL;
                        }
                    }
                    break;
        default:
                    Led.Status = IDEL;
                    break;
    }
}

