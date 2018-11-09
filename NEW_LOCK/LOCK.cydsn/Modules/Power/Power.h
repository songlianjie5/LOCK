#ifndef _POWER_H
#define _POWER_H
#include "main.h"

//#define FL3235_IIC_ADDRESS  	0x78//AD 接 GND
#define ON 						    0x01			//工作模式软件开
#define OFF 						0x00			//工作模式软件关
#define I_MAX_1_1					0x00
#define I_MAX_1_2					0x02
#define I_MAX_1_3					0x04
#define I_MAX_1_4					0x06

#define FL3235_REG_SHUTDOWN		    0x00 		//关断寄存器
#define FL3235_REG_PWM_OUT1 	    0x05		//输出PWM寄存器
#define FL3235_REG_PWM_OUT2 	    0x06
#define FL3235_REG_PWM_OUT3 	    0x07
#define FL3235_REG_PWM_OUT4 	    0x08
#define FL3235_REG_PWM_OUT5 	    0x09
#define FL3235_REG_PWM_OUT6 	    0x0A
#define FL3235_REG_PWM_OUT7 	    0x0B
#define FL3235_REG_PWM_OUT8 	    0x0C
#define FL3235_REG_PWM_OUT9 	    0x0D
#define FL3235_REG_PWM_OUT10 	    0x0E
#define FL3235_REG_PWM_OUT11 	    0x0F
#define FL3235_REG_PWM_OUT12 	    0x10
#define FL3235_REG_PWM_OUT13 	    0x11
#define FL3235_REG_PWM_OUT14 	    0x12
#define FL3235_REG_PWM_OUT15 	    0x13
#define FL3235_REG_PWM_OUT16 	    0x14
#define FL3235_REG_PWM_OUT17 	    0x15
#define FL3235_REG_PWM_OUT18 	    0x16
#define FL3235_REG_PWM_OUT19 	    0x17
#define FL3235_REG_PWM_OUT20 	    0x18
#define FL3235_REG_PWM_OUT21 	    0x19
#define FL3235_REG_PWM_OUT22 	    0x1A
#define FL3235_REG_PWM_OUT23 	    0x1B
#define FL3235_REG_PWM_OUT24 	    0x1C
#define FL3235_REG_PWM_OUT25 	    0x1D
#define FL3235_REG_PWM_OUT26 	    0x1E
#define FL3235_REG_PWM_OUT27 	    0x1F
#define FL3235_REG_PWM_OUT28 	    0x20
#define	FL3235_REG_DATA_UPDATE	    0x25 	//PWM和LED控制更新寄存器
#define FL3235_REG_CTRL_OUT1 		0x2A	//LED控制寄存器
#define FL3235_REG_CTRL_OUT2 		0x2B
#define FL3235_REG_CTRL_OUT3 		0x2C
#define FL3235_REG_CTRL_OUT4 		0x2D
#define FL3235_REG_CTRL_OUT5 		0x2E
#define FL3235_REG_CTRL_OUT6 		0x2F
#define FL3235_REG_CTRL_OUT7 		0x30
#define FL3235_REG_CTRL_OUT8 		0x31
#define FL3235_REG_CTRL_OUT9 		0x32
#define FL3235_REG_CTRL_OUT10 	    0x33
#define FL3235_REG_CTRL_OUT11 	    0x34
#define FL3235_REG_CTRL_OUT12 	    0x35
#define FL3235_REG_CTRL_OUT13 	    0x36
#define FL3235_REG_CTRL_OUT14 	    0x37
#define FL3235_REG_CTRL_OUT15 	    0x38
#define FL3235_REG_CTRL_OUT16 	    0x39
#define FL3235_REG_CTRL_OUT17 	    0x3A
#define FL3235_REG_CTRL_OUT18 	    0x3B
#define FL3235_REG_CTRL_OUT19 	    0x3C
#define FL3235_REG_CTRL_OUT20 	    0x3D
#define FL3235_REG_CTRL_OUT21 	    0x3E
#define FL3235_REG_CTRL_OUT22 	    0x3F
#define FL3235_REG_CTRL_OUT23 	    0x40
#define FL3235_REG_CTRL_OUT24 	    0x41
#define FL3235_REG_CTRL_OUT25 	    0x42
#define FL3235_REG_CTRL_OUT26 	    0x43
#define FL3235_REG_CTRL_OUT27 	    0x44
#define FL3235_REG_CTRL_OUT28 	    0x45
#define FL3235_REG_LED_GLOBAL_CTRL  0x4A //全通道开关
#define FL3235_REG_LED_FRE 			0x4B //输出频率寄存器
#define FL3235_REG_RST 			    0x4F //复位寄存器

/********************************************************/
#define	SN_SDB_PIN_High()	I2C_ST_Write(1)     //chip shut down线输出高.
#define	SN_SDB_PIN_Low()	I2C_ST_Write(0)     //chip shut down线输出低.
    
bool I2C_WriteReg(uint8_t reg_address, uint8_t reg_data);
bool I2C_WriteRegSeries(uint8_t reg_address, uint8_t reg_data,uint8_t unit);
bool I2C_WriteRegTab(uint8_t reg_address, uint8_t *tab,uint8_t unit);
void FL3225_Init();
void FL3225_DataUpdate(void);
void OLED_FLASH_POWER(uint8_t onoff);//OLED电源开关
void VOICE_POWER(uint8_t onoff);//语音电源开关
void BLE_POWER(uint8_t onoff);//蓝牙电源开关
void FPC_POWER(uint8_t onoff);//指纹电源开关
void LOCK_POWER(uint8_t onoff);//锁体电源开关
#endif