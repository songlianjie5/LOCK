#include "Fingerprint.h"

#define BUF_SIZE	 100
typedef struct {
    uint8_t Buf[BUF_SIZE];
    uint8_t RxIndex;
    uint8_t RxNum;
} UartBuf_t;

static UartBuf_t UartBuf;//接收缓冲区

static Fingerprint_t Fingerprint;

/////////////////////////////内部函数//////////////////////////////
static void Fingerprint_send(uint8_t s[],uint16_t len);

static void Fingerprint_GetImage(void);

static void Fingerprint_GenChar(uint8_t bufferID);

static void Fingerprint_Search(void);

static void Fingerprint_RegModel(void);

static void Fingerprint_StorChar(uint8_t pageID);

static void Fingerprint_DeletChar(uint8_t pageID);

static void Fingerprint_Empty(void);

static void Fingerprint_WriteReg(uint8_t reg,uint8_t val);

#ifdef PLATFORM
void Fingerprint_recive_isr(void)//指纹串口接收中断函数
{
	if(UART_0_FPC_GetRxInterruptSourceMasked()&UART_0_FPC_INTR_RX_NOT_EMPTY)
    {
        if(UartBuf.RxIndex >= BUF_SIZE)UartBuf.RxIndex = 0;					
        UartBuf.Buf[UartBuf.RxIndex++] = UART_0_FPC_SpiUartReadRxData();//接收
        if(++UartBuf.RxNum > BUF_SIZE)UartBuf.RxNum = BUF_SIZE;			
        UART_0_FPC_ClearRxInterruptSource(UART_0_FPC_INTR_RX_NOT_EMPTY); 
	}    
}
#endif

static void Fingerprint_send(uint8_t s[],uint16_t len)
{
    UART_0_FPC_SpiUartPutArray(s, len);
}

static void Fingerprint_GetImage(void)
{
	const uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x01,0x00,0x05};
	Fingerprint_send((uint8_t *)&CMD[0],sizeof(CMD));
}

static void Fingerprint_GenChar(uint8_t bufferID)
{
    uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x04,0x02,0x00,0x00,0x07};
    CMD[10] = bufferID;
    CMD[12] = bufferID+0x07;
	Fingerprint_send(CMD,sizeof(CMD));
}

static void Fingerprint_Search(void)
{
    const uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x64,0x00,0x72};
	Fingerprint_send((uint8_t *)&CMD[0],sizeof(CMD));
}

static void Fingerprint_RegModel(void)
{
    const uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x05,0x00,0x09};
	Fingerprint_send((uint8_t *)&CMD[0],sizeof(CMD));
}

static void Fingerprint_StorChar(uint8_t pageID)
{
    uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x00};
    CMD[12] = pageID;
    CMD[14] = 0x0E + pageID;
	Fingerprint_send(CMD,sizeof(CMD));	   
}

static void Fingerprint_DeletChar(uint8_t pageID)
{
    uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x01,0x00,0x15};
    CMD[11] = pageID;
    CMD[15] = 0x15 + pageID;
	Fingerprint_send(CMD,sizeof(CMD));	
}

static void Fingerprint_Empty(void)
{
    const uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x0D,0x00,0x11};
	Fingerprint_send((uint8_t *)&CMD[0],sizeof(CMD));
}

static void Fingerprint_WriteReg(uint8_t reg,uint8_t val)
{
    uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x05,0x0e,reg,val,0x00,0x00};
    CMD[10] = reg;
    CMD[11] = val;
    CMD[13] = 0x14+reg+val;
	Fingerprint_send(CMD,sizeof(CMD));	
}

static uint8_t Fingerprint_Check_Ack(uint8_t option,uint8_t *pid)
{
	uint8_t ret=0xFF;
	uint8_t i;
	uint16_t cs=0,sum=0;
	static uint8_t ActTime = 0;
	static uint8_t LastRxNum = 0;
	
	if(LastRxNum != UartBuf.RxNum)                                                             //对比接收字数是否改变     
	{
		ActTime = 0;
		LastRxNum = UartBuf.RxNum;
	}
	else if(++ActTime>(20/TIME_BASIC_MS))                                                      //接收期间如果不连续接收超时X ms后重新接收
	{
		ActTime = 0;
		UartBuf.RxNum = UartBuf.RxIndex = 0;
	}
	
	if(option==0x55)                                                                           //是否用来接收0x55的
	{
		if(UartBuf.RxNum>0)
		{
			UartBuf.RxNum = UartBuf.RxIndex = 0;
			if(UartBuf.Buf[0]==0x55)                                                           //确认0x55
				ret = 0x00;
		}
	}
	else 
	{
		if(UartBuf.RxNum == (9+UartBuf.Buf[8]))                                                 //长度确认
		{
			if( UartBuf.Buf[0] == 0xEF && UartBuf.Buf[1] == 0x01 )                              //包头确认
			{
				for(i=6;i<(UartBuf.RxNum-2);i++)sum += UartBuf.Buf[i];                          //计算校验码
				cs = UartBuf.Buf[UartBuf.RxNum-2];cs = (cs<<8)&0xFF00;cs|= UartBuf.Buf[UartBuf.RxNum-1];
				if(sum == cs)                                                                   //校验码确认
				{
					ret = UartBuf.Buf[9];                                                       //返回值
                    if(UartBuf.RxNum==16)
					*pid = UartBuf.Buf[11];                                                     //在收12字节时无效,只有在接收PID包时才有用到
				}
			}
			UartBuf.RxNum = UartBuf.RxIndex = 0;
		}
	}
	return ret;
}
//////////////////////////////////////外部函数////////////////////////////////////////
Fingerprint_t *Fingerprint_status(void)
{
	return &Fingerprint;
}

int8_t Fingerprint_init(void)
{
#ifdef PLATFORM
	//串口 ISR DMA 
    UART_0_FPC_Start();                      
    UART_0_FPC_SCB_IRQ_StartEx(Fingerprint_recive_isr);

    Fingerprint_Power(true);                //供电 AS608
    CyDelay(100);
    Fingerprint_WriteReg(0x03,REG_LOGIC);   //注册逻辑为1
    CyDelay(100);
    Fingerprint_WriteReg(0x01,REG_NUM);     //注册次数为8
    CyDelay(100);
    Fingerprint_Sleep();
    CyDelay(100);
    Fingerprint_Power(false);               //断电 
    
    Fingerprint.SensorIsr = false; 
    UartBuf.RxNum = UartBuf.RxIndex = 0;
#endif
	return 0;
}

void Fingerprint_Sleep(void)
{
    const uint8_t CMD[]={0xef,0x01,0xff,0xff,0xff,0xff,0x01,0x00,0x03,0x33,0x00,0x37};
	Fingerprint_send((uint8_t *)&CMD[0],sizeof(CMD));   
}

void Fingerprint_Power(bool option)
{
    Fingerprint.PowerStaus = option;
    FPC_POWER(option);
}

//call in TIME_BASIC_MS

int8_t Fingerprint_NormalProcess(void)
{
	static bool f_send = true;
	static uint16_t timeout = 0;
    static uint8_t step=0;
    volatile uint8_t Pageid=0;
    uint8_t ret;
    switch(step)
    {
        case 0://指纹上电
                if((Fingerprint.SensorIsr == false) && (step!=5))
                {
                    step=5;
                    return 0xFF;//已休眠了要跳到等中断状态
                }
        		if(f_send)
        		{
        			Fingerprint_Power(true);
        			f_send = false;
        		}
        		else 
        		{
        			if(Fingerprint_Check_Ack(0x55,&Pageid)==0)
        			{
        				step = 1;
                        f_send = true;
        				timeout = 0;
        			}
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				step = 4;
                        f_send = true;
        				timeout = 0;
        			}			
        		}
                break;
        case 1://获取指纹
        		if(f_send)
        		{
        			Fingerprint_GetImage();
        			f_send = false;
                    Fingerprint.NormalEven = GET_ING;                    
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret !=0xFF)
        			{
                        if(ret==0x00)
                        {step = 2;Fingerprint.NormalEven = GET_OK;}
                        else 
                        {step = 4;Fingerprint.NormalEven = GET_FAIL;}
                        f_send = true;
        				timeout = 0;
        			}                
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				step = 4;
                        f_send = true;
        				timeout = 0;
        			}				
        		}
                break;
        case 2://生成特征
        		if(f_send)
        		{
        			Fingerprint_GenChar(0x01);
        			f_send = false;
        		}	
        		else 
        		{
        			if(Fingerprint_Check_Ack(0,&Pageid)==0)
        			{
        				step = 3;
                        f_send = true;
        				timeout = 0;
        			}else if(++timeout>(500/TIME_BASIC_MS)){
        				step = 4;
                        f_send = true;
        				timeout = 0;
        			}				
        		}   
                break;
        case 3://搜索模板
        		if(f_send)
        		{
        			Fingerprint_Search();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret != 0xFF)
        			{
                        if(ret==0x00)
                            Fingerprint.NormalEven = SEARCH_OK;
                        else
                            Fingerprint.NormalEven = SEARCH_FAIL;                             
                        Fingerprint.PID = Pageid;
        				step = 4;
                        f_send = true;
        				timeout = 0;
        			}
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				step = 4;
                        f_send = true;
        				timeout = 0;
        			}				
        		} 
                break;
        case 4://指纹断电
        		if(f_send)
        		{
        			Fingerprint_Sleep();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret != 0xFF)
        			{
                        if(ret==0x00)
                        {
            				Fingerprint_Power(false);
            				Fingerprint.SensorIsr = false;	
            				step = 5;
                        }
                        f_send = true;
        				timeout = 0;
        			}
        			else if(++timeout>(200/TIME_BASIC_MS))
        			{
        				f_send = true; //多次重发休眠指令
                        timeout = 0;
        			}
        		}
                break;
        case 5://等待中断
                Fingerprint.NormalEven = WAIT_ISR; 
        		if(Fingerprint.SensorIsr == true)
        		{
        			step = 0;
                    f_send = true;
    				timeout = 0;
        		}
                break;
        default:step= 0;break;
    }
    return ret;
}

int8_t Fingerprint_RegProcess(void)
{
	static bool f_send = true;
	static uint16_t timeout = 0;
    
    uint8_t Pageid;
    uint8_t ret;
    uint8_t Fingerprint_ret;
    switch(Fingerprint.step)
    {
        case 0://指纹上电
        		if(f_send)
        		{
        			Fingerprint_Power(true);
        			f_send = false;
        		}
        		else 
        		{
        			if(Fingerprint_Check_Ack(0x55,&Pageid)==0)
        			{
                        Fingerprint.BID = 0;  //初始化
        				Fingerprint.step = 1;
                        f_send = true;
        				timeout = 0;
        			}
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				Fingerprint.step = 6;
                        f_send = true;
        				timeout = 0;
        			}			
        		}
                break;
        case 1://获取指纹
        		if(f_send)
        		{
        			Fingerprint_GetImage();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret !=0xFF)
        			{
                        if(ret==0x00)
                        {Fingerprint.step = 2;Fingerprint.RegEven = GET_OK;}
                        else
                            Fingerprint.RegEven = GET_FAIL;
                        f_send = true;
        				timeout = 0;
        			}                
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				Fingerprint.step = 6;
                        f_send = true;
        				timeout = 0;
        			}				
        		}
                break;
        case 2://生成特征
        		if(f_send)
        		{
        			Fingerprint_GenChar(Fingerprint.BID+1);
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
                    if(ret != 0xFF)
        			{
                        Fingerprint.step = 3; //等待手抬起
                        if(ret ==0x00 || ret==0x28)
                        {
                            if(ret==0x28)
                                Fingerprint.RegEven = GENCHAR_SAME;
                        }
                        f_send = true;
        				timeout = 0;
        			}
                    else if(++timeout>(500/TIME_BASIC_MS)){
        				Fingerprint.step = 6;
                        f_send = true;
        				timeout = 0;
        			}				
        		}   
                break;
        case 3: //等待手抬起
        		if(f_send)
        		{
        			Fingerprint_GetImage();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret !=0xFF)
        			{
                        if(ret==0x02) //手已抬起
                        {
                            Fingerprint.step = 1;//再或取
                            Fingerprint.RegEven = GENCHAR_OK;
                            if(++Fingerprint.BID>(REG_NUM-1))
            				    Fingerprint.step = 4;
                        }
                        f_send = true;
        				timeout = 0;
        			}                			
        		}              
                break;
        case 4: //合成模板
        		if(f_send)
        		{
        			Fingerprint_RegModel();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
                    if(ret!=0xFF)
                    {
        				Fingerprint.step = 5;
                        f_send = true;
        				timeout = 0;                        
        			}
                }
                break;
        case 5: //存储模板
        		if(f_send)
        		{
        			Fingerprint_StorChar(Fingerprint.PID);
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
                    if(ret !=0xFF)
                    {
                        if(ret==0x00)
                            Fingerprint.RegEven = STORE_OK;
                        else 
                            Fingerprint.RegEven = STORE_FAIL;
                        
                        Fingerprint.BID = 0;  //初始化
        				Fingerprint.step = 6; //再或取
                        f_send = true;
        				timeout = 0;
        			}	
                }
                break;      
        case 6://指纹断电
        		if(f_send)
        		{
        			Fingerprint_Sleep();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret != 0xFF)
        			{
                        if(ret==0x00)
                        {
            				Fingerprint_Power(false);
            				Fingerprint.step = 0;
                            Fingerprint.RegEven = SLEEP_OK; //发送休眠成功标记
                        }
                        f_send = true;
        				timeout = 0;
        			}
        			else if(++timeout>(300/TIME_BASIC_MS))
        			{
        				f_send = true; //多次重发休眠指令
                        timeout = 0;
        			}
        		}
                break;  
        default:Fingerprint.step=0;break;        
    }
    return Fingerprint_ret;
}

//Fingerprint_DeletChar(uint8_t pageID);
int8_t Fingerprint_RegDelProcess(uint8_t pid,bool all)
{
	static bool f_send = true;
	static uint16_t timeout = 0;
    uint8_t Pageid;
    uint8_t ret;
    switch(Fingerprint.step)
    {
        case 0://指纹上电
        		if(f_send)
        		{
        			Fingerprint_Power(true);
        			f_send = false;
        		}
        		else 
        		{
        			if(Fingerprint_Check_Ack(0x55,&Pageid)==0)
        			{
                        Fingerprint.BID = 0;  //初始化
        				Fingerprint.step = 1;
                        f_send = true;
        				timeout = 0;
        			}
                    else if(++timeout>(500/TIME_BASIC_MS))
                    {
        				Fingerprint.step = 6;
                        f_send = true;
        				timeout = 0;
        			}			
        		}
                break;
        case 1://清空指纹 或者按 Pid 删除
        		if(f_send)
        		{
                    if(all)
        			    Fingerprint_Empty();                                //清空
                    else 
                        Fingerprint_DeletChar(pid);                         //按pid删除
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
                    if(ret!=0xFF)
                    {
                        Fingerprint.step = 6;
                        f_send = true;
        				timeout = 0;
                        if(ret==0)Fingerprint.RegEven = EMPTY_OK;
                        else Fingerprint.RegEven = EMPTY_FAIL;
                    }   
        		}
                break; 
        case 6://指纹断电
        		if(f_send)
        		{
        			Fingerprint_Sleep();
        			f_send = false;
        		}	
        		else 
        		{
                    ret = Fingerprint_Check_Ack(0,&Pageid);
        			if(ret != 0xFF)
        			{
                        if(ret==0x00)
                        {
            				Fingerprint_Power(false);
            				Fingerprint.step = 0;
                            Fingerprint.RegEven = SLEEP_OK; //发送休眠成功标记
                        }
                        f_send = true;
        				timeout = 0;
        			}
        			else if(++timeout>(300/TIME_BASIC_MS))
        			{
        				f_send = true; //多次重发休眠指令
                        timeout = 0;
        			}
        		}
                break;  
        default:Fingerprint.step=0;break;        
    }
    return 0xFF;
}



