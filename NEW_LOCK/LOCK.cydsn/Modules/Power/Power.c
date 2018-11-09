#include "Power.h"

/*******************IIC 通讯底层***********************/

#define SN_IIC_DELAY_SW			OPEN  	        //IIC通讯延时控制开关  OPEN 开   CLOSE 关
#define SN3236_IIC_ADDRESS      0x78            //AD 接 GND  

#ifdef PLATFORM
    
#define SN_SDA_PIN_High()	I2C_SDA_Write(1)   	//SDA=1
#define SN_SDA_PIN_Low()	I2C_SDA_Write(0)  	//SDA=0
#define SN_SDA_PIN          I2C_SDA_Read()

#define SN_SCL_PIN_High()	I2C_SCL_Write(1)    //SCL=1
#define SN_SCL_PIN_Low()	I2C_SCL_Write(0)  	//SCL=0

//#define	SN_SDB_PIN_High()	I2C_ST_Write(1)     //chip shut down线输出高.
//#define	SN_SDB_PIN_Low()	I2C_ST_Write(0)     //chip shut down线输出低.

#define Delay_IIC()         {__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}
    
#endif

static void SN_IIC_Start(void);//启动总线

static void SN_IIC_Stop(void);//停止总线

static uint8_t SN_IIC_TestAck(void);//测试应答信号

static void SN_IIC_Write_Byte(uint8_t inputdata);//写入8个二进制位

static void SN_IIC_Start(void)//启动总线
{
#ifdef PLATFORM
    
    SN_SDA_PIN_High();

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
	
	SN_SCL_PIN_High();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
	
	SN_SDA_PIN_Low();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
	
	SN_SCL_PIN_Low();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
    
#endif
}

static void SN_IIC_Stop(void)//停止总线
{
#ifdef PLATFORM	
    
    SN_SCL_PIN_Low();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	SN_SDA_PIN_Low();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	SN_SCL_PIN_High();
	
	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	SN_SDA_PIN_High();

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
    
#endif
}
		 

static uint8_t SN_IIC_TestAck(void)//测试应答信号
{
	uint8_t bit_ack;

#ifdef PLATFORM	
	
    SN_SDA_PIN_High();

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	SN_SCL_PIN_High();

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	bit_ack=SN_SDA_PIN;

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif

	SN_SCL_PIN_Low();

	#if SN_IIC_DELAY_SW
	Delay_IIC();
	#endif
    
#endif

	return(bit_ack);
}

static void SN_IIC_Write_Byte(uint8_t inputdata)//写入8个二进制位
{
	uint8_t temp,bMask=0X80;

#ifdef PLATFORM

	for(temp=0;temp<8;temp++)
	{
		if(inputdata&bMask)
		{
			SN_SDA_PIN_High();
		}
		else
		{
			SN_SDA_PIN_Low();
		}
		#if SN_IIC_DELAY_SW
		Delay_IIC();
		#endif

		SN_SCL_PIN_High();

		#if SN_IIC_DELAY_SW
		Delay_IIC();
		#endif

		SN_SCL_PIN_Low();

		#if SN_IIC_DELAY_SW
		Delay_IIC();
		#endif

		bMask=bMask>>1;
	}
    
#endif
}

bool I2C_WriteReg(uint8_t reg_address, uint8_t reg_data)
{
	SN_IIC_Start();									//开始信号
	SN_IIC_Write_Byte(SN3236_IIC_ADDRESS);			//写IIC地址
	SN_IIC_TestAck();								//测试应答
	SN_IIC_Write_Byte(reg_address);					//写入寄存器地址
	SN_IIC_TestAck();								//测试应答
	SN_IIC_Write_Byte(reg_data);					//写入数据
	SN_IIC_TestAck();								//测试应答
	SN_IIC_Stop();									//结束信号
    return true;
}
    
//连续写入N个同样数据
bool I2C_WriteRegSeries(uint8_t reg_address, uint8_t reg_data,uint8_t unit)
{
	SN_IIC_Start();												//开始信号
	SN_IIC_Write_Byte(SN3236_IIC_ADDRESS);//写IIC地址
	SN_IIC_TestAck();											//测试应答
	SN_IIC_Write_Byte(reg_address);				//写入寄存器地址
	SN_IIC_TestAck();											//测试应答
	for(;unit!=0;unit--)
	{
		SN_IIC_Write_Byte(reg_data);				//写入数据
		SN_IIC_TestAck();										//测试应答
	}
	SN_IIC_Stop();												//结束信号
    return true;
}
//连续写入N个同样数据
bool I2C_WriteRegTab(uint8_t reg_address, uint8_t *tab,uint8_t unit)
{
    uint8_t i;
	SN_IIC_Start();												//开始信号
	SN_IIC_Write_Byte(SN3236_IIC_ADDRESS);//写IIC地址
	SN_IIC_TestAck();											//测试应答
	SN_IIC_Write_Byte(reg_address);				//写入寄存器地址
	SN_IIC_TestAck();											//测试应答
	for(i=0;i<unit;i++)
	{
		SN_IIC_Write_Byte(tab[i]);				//写入数据
		SN_IIC_TestAck();										//测试应答
	}
	SN_IIC_Stop();												//结束信号
    return true;
}

void FL3225_Init()
{
#ifdef PLATFORM
    SN_SDB_PIN_High();//开硬件开关
    CyDelay(100);
#endif
    I2C_WriteReg(FL3235_REG_RST,0x00);
	I2C_WriteReg(FL3235_REG_SHUTDOWN, 1);//开软件开关
    I2C_WriteReg(FL3235_REG_LED_FRE, 0x01);                    //22Khz	
}

void FL3225_DataUpdate(void)
{
    I2C_WriteReg(FL3235_REG_DATA_UPDATE, 0x00);
}

void OLED_FLASH_POWER(uint8_t onoff)
{
    I2C_WriteReg(FL3235_REG_PWM_OUT2, 0xFF);
    I2C_WriteReg(FL3235_REG_CTRL_OUT2, I_MAX_1_4|onoff);         //供电 FLASH OLED 
    FL3225_DataUpdate();
}

void VOICE_POWER(uint8_t onoff)
{
    I2C_WriteReg(FL3235_REG_PWM_OUT21, 0xFF);
    I2C_WriteReg(FL3235_REG_CTRL_OUT21, I_MAX_1_4|onoff);        //供电 Voice
    FL3225_DataUpdate();
}

void BLE_POWER(uint8_t onoff)
{
	I2C_WriteReg(FL3235_REG_PWM_OUT12, 0xFF);
	I2C_WriteReg(FL3235_REG_CTRL_OUT12, I_MAX_1_4|onoff);        //供电 BLE   
    FL3225_DataUpdate();
}

void FPC_POWER(uint8_t onoff)
{
    I2C_WriteReg(FL3235_REG_PWM_OUT4, 0xFF);
	I2C_WriteReg(FL3235_REG_CTRL_OUT4, I_MAX_1_4|onoff);         //供电 AS608
    FL3225_DataUpdate();
}

void LOCK_POWER(uint8_t onoff)
{
    I2C_WriteReg(FL3235_REG_PWM_OUT6, 0xFF);\
    I2C_WriteReg(FL3235_REG_CTRL_OUT6, I_MAX_1_4|onoff);
	FL3225_DataUpdate();	
}

/* [] END OF FILE */
