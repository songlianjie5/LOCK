#ifndef _GPIO_H
#define _GPIO_H
    
#include "main.h"
    
typedef struct {
    uint32_t gpio0_data;
    uint32_t gpio1_data;
    uint32_t gpio2_data;
    uint32_t gpio3_data;
    uint32_t gpio4_data;
    uint32_t gpio5_data;
    uint32_t gpio6_data;
    uint32_t gpio7_data;
}GPIO_REGDATA,*pGPIO_REGDATA;

typedef enum
{
    GPIO0 = 1,
    GPIO1 = 2,
    GPIO2 = 3,
    GPIO3 = 4,
    GPIO4 = 5,
    GPIO5 = 6,
    GPIO6 = 7,
    GPIO7 = 8,
}GPIO_TYPE;

typedef enum
{
    PIN0 = 0,
    PIN1 = 1,
    PIN2 = 2,
    PIN3 = 3,
    PIN4 = 4,
    PIN5 = 5,
    PIN6 = 6,
    PIN7 = 7,
}PINS;
    
#define DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
#define DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
#define DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
#define DM_RES_DOWN        (0x03u) /**< \brief Resistive Pull Down     */
#define DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
#define DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
#define DM_STRONG          (0x06u) /**< \brief Strong Drive            */
#define DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */ 

#define FPC_WAKE_PIN       (5)
#define BLE_WAKE_PIN       (4) 
#define EXT_WAKE_PIN       (3) 
#define KEY_WAKE_PIN       (2)

#define SLOT_PIN           (3)
#define LOCK_MODE_PIN      (2)
#define LOCK_BNT_PIN       (1)

#define SWD_DIO_PIN        (2)
#define SWD_CLK_PIN        (3)
       
void GPIO_SaveStat();//保存GPIO初始化状态寄存器的值

void GPIO_LoadStat();//恢复GPIO寄存器初始化的值

void GPIO_SleepSet();//睡眠前将所有IO口设置成高阻状态

pGPIO_REGDATA GPIO_GetRegData();

void GPIO_SetPinDriveMode(GPIO_TYPE type,PINS pin,uint8_t mode);

void GPIO_WritePin(GPIO_TYPE type,PINS pin,uint8_t val);

uint8_t GPIO_ReadPin(GPIO_TYPE type,PINS pin);

#endif
    