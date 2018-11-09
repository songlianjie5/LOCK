#include "gpio.h"

#define DRIVE_MODE_BITS        (3)
#define DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - DRIVE_MODE_BITS))

#define GPIO_0_PC   (* (reg32 *) CYREG_GPIO_PRT0_PC)
#define GPIO_1_PC   (* (reg32 *) CYREG_GPIO_PRT1_PC)
#define GPIO_2_PC   (* (reg32 *) CYREG_GPIO_PRT2_PC)
#define GPIO_3_PC   (* (reg32 *) CYREG_GPIO_PRT3_PC)
#define GPIO_4_PC   (* (reg32 *) CYREG_GPIO_PRT4_PC)
#define GPIO_5_PC   (* (reg32 *) CYREG_GPIO_PRT5_PC)
#define GPIO_6_PC   (* (reg32 *) CYREG_GPIO_PRT6_PC)
#define GPIO_7_PC   (* (reg32 *) CYREG_GPIO_PRT7_PC)

#define GPIO_0_DR   (* (reg32 *) CYREG_GPIO_PRT0_DR)
#define GPIO_1_DR   (* (reg32 *) CYREG_GPIO_PRT1_DR)
#define GPIO_2_DR   (* (reg32 *) CYREG_GPIO_PRT2_DR)
#define GPIO_3_DR   (* (reg32 *) CYREG_GPIO_PRT3_DR)
#define GPIO_4_DR   (* (reg32 *) CYREG_GPIO_PRT4_DR)
#define GPIO_5_DR   (* (reg32 *) CYREG_GPIO_PRT5_DR)
#define GPIO_6_DR   (* (reg32 *) CYREG_GPIO_PRT6_DR)
#define GPIO_7_DR   (* (reg32 *) CYREG_GPIO_PRT7_DR)

#define GPIO_0_PS   (* (reg32 *) CYREG_GPIO_PRT0_PS)
#define GPIO_1_PS   (* (reg32 *) CYREG_GPIO_PRT1_PS)
#define GPIO_2_PS   (* (reg32 *) CYREG_GPIO_PRT2_PS)
#define GPIO_3_PS   (* (reg32 *) CYREG_GPIO_PRT3_PS)
#define GPIO_4_PS   (* (reg32 *) CYREG_GPIO_PRT4_PS)
#define GPIO_5_PS   (* (reg32 *) CYREG_GPIO_PRT5_PS)
#define GPIO_6_PS   (* (reg32 *) CYREG_GPIO_PRT6_PS)
#define GPIO_7_PS   (* (reg32 *) CYREG_GPIO_PRT7_PS)

#define GPIO_0_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_0_PC = (GPIO_0_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_1_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_1_PC = (GPIO_1_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_2_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_2_PC = (GPIO_2_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_3_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_3_PC = (GPIO_3_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_4_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_4_PC = (GPIO_4_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_5_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_5_PC = (GPIO_5_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_6_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_6_PC = (GPIO_6_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

#define GPIO_7_SetP4PinDriveMode(shift, mode)  \
                                                    do { \
                                                        GPIO_7_PC = (GPIO_7_PC & \
                                                                    (uint32_t)(~(uint32_t)(DRIVE_MODE_IND_MASK << \
                                                                    (DRIVE_MODE_BITS * (shift))))) | \
                                                                    (uint32)((uint32)(mode) << \
                                                                    (DRIVE_MODE_BITS * (shift))); \
                                                    } while (0)

static GPIO_REGDATA regdata;

void GPIO_SaveStat()//保存初始化后GPIO寄存器值
{
	regdata.gpio0_data = CY_GET_REG32(CYREG_GPIO_PRT0_PC);
	regdata.gpio1_data = CY_GET_REG32(CYREG_GPIO_PRT1_PC);
	regdata.gpio2_data = CY_GET_REG32(CYREG_GPIO_PRT2_PC);
	regdata.gpio3_data = CY_GET_REG32(CYREG_GPIO_PRT3_PC);
	regdata.gpio4_data = CY_GET_REG32(CYREG_GPIO_PRT4_PC);
	regdata.gpio5_data = CY_GET_REG32(CYREG_GPIO_PRT5_PC);
	regdata.gpio6_data = CY_GET_REG32(CYREG_GPIO_PRT6_PC);
	regdata.gpio7_data = CY_GET_REG32(CYREG_GPIO_PRT7_PC);
}

void GPIO_LoadStat()//恢复GPIO睡眠前的值
{
	CY_SET_REG32((void*)CYREG_GPIO_PRT0_PC,regdata.gpio0_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT1_PC,regdata.gpio1_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT2_PC,regdata.gpio2_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT3_PC,regdata.gpio3_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT4_PC,regdata.gpio4_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT5_PC,regdata.gpio5_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT6_PC,regdata.gpio6_data);
	CY_SET_REG32((void*)CYREG_GPIO_PRT7_PC,regdata.gpio7_data);  
}

void GPIO_SleepSet()
{
	//睡眠前将所有IO口设置成高阻状态  
//	GPIO_SetPinDriveMode(GPIO0,PIN0,DM_DIG_HIZ);//OP2
//	GPIO_SetPinDriveMode(GPIO0,PIN1,DM_DIG_HIZ);//一键开关门键  下降沿
//	GPIO_SetPinDriveMode(GPIO0,PIN2,DM_DIG_HIZ);//手动自动开关 下降沿
//	GPIO_SetPinDriveMode(GPIO0,PIN3,DM_DIG_HIZ);//插舌开关SLOT_INT 上升沿
	GPIO_SetPinDriveMode(GPIO0,PIN4,DM_ALG_HIZ);//X1
	GPIO_SetPinDriveMode(GPIO0,PIN5,DM_ALG_HIZ);//X2
	GPIO_SetPinDriveMode(GPIO0,PIN6,DM_ALG_HIZ);//OP1
	GPIO_SetPinDriveMode(GPIO0,PIN7,DM_ALG_HIZ);//空脚 

	GPIO_SetPinDriveMode(GPIO1,PIN0,DM_ALG_HIZ);//UART2_RX
	GPIO_SetPinDriveMode(GPIO1,PIN1,DM_ALG_HIZ);//UART2_TX
//	GPIO_SetPinDriveMode(GPIO1,PIN2,DM_DIG_HIZ);//复位按键   下降沿
	GPIO_SetPinDriveMode(GPIO1,PIN3,DM_ALG_HIZ);//EXT_INT
	GPIO_SetPinDriveMode(GPIO1,PIN4,DM_ALG_HIZ);//BLE_WAKEUP
	GPIO_SetPinDriveMode(GPIO1,PIN5,DM_DIG_HIZ);//指纹模块中断脚 上升沿
	GPIO_SetPinDriveMode(GPIO1,PIN6,DM_ALG_HIZ);//COVER_CHK
	GPIO_SetPinDriveMode(GPIO1,PIN7,DM_ALG_HIZ);//VOICE BUSY

	GPIO_SetPinDriveMode(GPIO2,PIN0,DM_ALG_HIZ);//SPI0_MOSI
	GPIO_SetPinDriveMode(GPIO2,PIN1,DM_ALG_HIZ);//SPI0_MISO
	GPIO_SetPinDriveMode(GPIO2,PIN2,DM_ALG_HIZ);//SPI0_SCK
	GPIO_SetPinDriveMode(GPIO2,PIN3,DM_ALG_HIZ);//SPI0_CS A
	GPIO_SetPinDriveMode(GPIO2,PIN4,DM_ALG_HIZ);//SPI0_CS B
	GPIO_SetPinDriveMode(GPIO2,PIN5,DM_ALG_HIZ);//BAT_ADC
	GPIO_SetPinDriveMode(GPIO2,PIN6,DM_ALG_HIZ);//MOTO_ADC
	GPIO_SetPinDriveMode(GPIO2,PIN7,DM_ALG_HIZ);//RF_ADC

	GPIO_SetPinDriveMode(GPIO3,PIN0,DM_ALG_HIZ);//TSI_CH0
	GPIO_SetPinDriveMode(GPIO3,PIN1,DM_ALG_HIZ);//TSI_CH6
	GPIO_SetPinDriveMode(GPIO3,PIN2,DM_ALG_HIZ);//SWD_DIO
	GPIO_SetPinDriveMode(GPIO3,PIN3,DM_ALG_HIZ);//SWD_CLK
	GPIO_SetPinDriveMode(GPIO3,PIN4,DM_ALG_HIZ);//TSI_CH7
	GPIO_SetPinDriveMode(GPIO3,PIN5,DM_ALG_HIZ);//TSI_CH8
	GPIO_SetPinDriveMode(GPIO3,PIN6,DM_ALG_HIZ);//TSI_CH9
	GPIO_SetPinDriveMode(GPIO3,PIN7,DM_ALG_HIZ);//TSI_CH10

	GPIO_SetPinDriveMode(GPIO4,PIN0,DM_ALG_HIZ);//TSI_CH1
	GPIO_SetPinDriveMode(GPIO4,PIN1,DM_ALG_HIZ);//TSI_CH13
	GPIO_SetPinDriveMode(GPIO4,PIN2,DM_ALG_HIZ);//CMOD
	GPIO_SetPinDriveMode(GPIO4,PIN3,DM_ALG_HIZ);//TSI_CH11
	GPIO_SetPinDriveMode(GPIO4,PIN4,DM_ALG_HIZ);//SPI1_MOSI
	GPIO_SetPinDriveMode(GPIO4,PIN5,DM_ALG_HIZ);//SPI1_MISO
	GPIO_SetPinDriveMode(GPIO4,PIN6,DM_ALG_HIZ);//SPI1_SCK
	GPIO_SetPinDriveMode(GPIO4,PIN7,DM_ALG_HIZ);//SPI1_CS
	
	GPIO_SetPinDriveMode(GPIO5,PIN0,DM_ALG_HIZ);//
	GPIO_SetPinDriveMode(GPIO5,PIN1,DM_ALG_HIZ);//
	GPIO_SetPinDriveMode(GPIO5,PIN2,DM_ALG_HIZ);//
	GPIO_SetPinDriveMode(GPIO5,PIN3,DM_ALG_HIZ);//DC
	GPIO_SetPinDriveMode(GPIO5,PIN4,DM_ALG_HIZ);//
	GPIO_SetPinDriveMode(GPIO5,PIN5,DM_ALG_HIZ);//
	GPIO_SetPinDriveMode(GPIO5,PIN6,DM_ALG_HIZ);//DAC0_OUT
	GPIO_SetPinDriveMode(GPIO5,PIN7,DM_ALG_HIZ);//595_ST 输出低

	GPIO_SetPinDriveMode(GPIO6,PIN0,DM_ALG_HIZ);//UART0_RX
	GPIO_SetPinDriveMode(GPIO6,PIN1,DM_ALG_HIZ);//UART0_TX
	GPIO_SetPinDriveMode(GPIO6,PIN2,DM_ALG_HIZ);//TSI_CH2
	GPIO_SetPinDriveMode(GPIO6,PIN4,DM_ALG_HIZ);//TSI_CH3
	GPIO_SetPinDriveMode(GPIO6,PIN5,DM_ALG_HIZ);//TSI_CH5

	GPIO_SetPinDriveMode(GPIO7,PIN0,DM_ALG_HIZ);//595_DAT
	GPIO_SetPinDriveMode(GPIO7,PIN1,DM_ALG_HIZ);//595_CLK
	GPIO_SetPinDriveMode(GPIO7,PIN2,DM_ALG_HIZ);//
}

pGPIO_REGDATA GPIO_GetRegData()
{
	return &regdata;
}

void GPIO_SetPinDriveMode(GPIO_TYPE type,PINS pin,uint8_t mode)
{
	switch (type)
	{
	case GPIO0:
		GPIO_0_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO1:
		GPIO_1_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO2:
		GPIO_2_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO3:
		GPIO_3_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO4:
		GPIO_4_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO5:
		GPIO_5_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO6:
		GPIO_6_SetP4PinDriveMode(pin,mode);
		break;
	case GPIO7:
		GPIO_7_SetP4PinDriveMode(pin,mode);
		break;
	default:
		break;
	}
}

void GPIO_WritePin(GPIO_TYPE type,PINS pin,uint8_t val)
{
	uint8_t MASK = 1 << pin;
	uint8_t drVal = 0;

	switch (type)
	{
	case GPIO0:
		drVal = (uint8_t)(GPIO_0_DR & (uint8_t)(~MASK));
		GPIO_0_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO1:
		drVal = (uint8_t)(GPIO_1_DR & (uint8_t)(~MASK));
		GPIO_1_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO2:
		drVal = (uint8_t)(GPIO_2_DR & (uint8_t)(~MASK));
		GPIO_2_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO3:
		drVal = (uint8_t)(GPIO_3_DR & (uint8_t)(~MASK));
		GPIO_3_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO4:
		drVal = (uint8_t)(GPIO_4_DR & (uint8_t)(~MASK));
		GPIO_4_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO5:
		drVal = (uint8_t)(GPIO_5_DR & (uint8_t)(~MASK));
		GPIO_5_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO6:
		drVal = (uint8_t)(GPIO_6_DR & (uint8_t)(~MASK));
		GPIO_6_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	case GPIO7:
		drVal = (uint8_t)(GPIO_7_DR & (uint8_t)(~MASK));
		GPIO_7_DR = (uint32)(drVal | ((uint8_t)(val << pin) & MASK));
		break;
	default:
		break;
	}
}

uint8_t GPIO_ReadPin(GPIO_TYPE type,PINS pin)
{
	uint8_t MASK = 1 << pin;
	uint8_t val = 0;

	switch (type)
	{
	case GPIO0:
		val = (uint8_t)((GPIO_0_PS & MASK) >> pin);
		break;
	case GPIO1:
		val = (uint8_t)((GPIO_1_PS & MASK) >> pin);
		break;
	case GPIO2:
		val = (uint8_t)((GPIO_2_PS & MASK) >> pin);
		break;
	case GPIO3:
		val = (uint8_t)((GPIO_3_PS & MASK) >> pin);
		break;
	case GPIO4:
		val = (uint8_t)((GPIO_4_PS & MASK) >> pin);
		break;
	case GPIO5:
		val = (uint8_t)((GPIO_5_PS & MASK) >> pin);
		break;
	case GPIO6:
		val = (uint8_t)((GPIO_6_PS & MASK) >> pin);
		break;
	case GPIO7:
		val = (uint8_t)((GPIO_7_PS & MASK) >> pin);
		break;
	default:
		break;
	}
	return val;
}







