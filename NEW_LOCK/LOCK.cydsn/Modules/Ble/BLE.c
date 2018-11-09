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

#include "BLE.h"

const char *TTMcmd[] = {
    "TTM:CIT-?",
    "TTM:REN-?",
    "TTM:RNI-?",
    "TTM:BPS-?",
    "TTM:MAC-?",
    "TTM:RST-SYSTEMRESET",
    "TTM:ADP-?",
    "TTM:TPL-?",
    "TTM:CDL-?",
    "TTM:PWR-POWERDOWN",
    "TTM:VER-VERSION",
    "TTM:CON-CONSTATE",
    "TTM:DSC-DISCON",
    "TTM:PWD-?",
    "TTM:ENC-?"
};

//enum{
//	CON=0,
//	NOCON,
//}BLE_STATUS;
typedef struct {
    uint8_t status;
    uint8_t option;
} BLE_t;


#define FIFO_SIZE	 100
typedef struct {
    uint8_t Fifo[FIFO_SIZE];
    uint8_t RxIndex;
    uint8_t RxNum;
//    uint8_t Even;
} Uartfifo_t;
volatile Uartfifo_t Uartfifo;


void BLE_ISR(void)
{
    /* Check for "RX fifo not empty interrupt" */
	if(UART_1_BLE_GetRxInterruptSourceMasked()&UART_1_BLE_INTR_RX_NOT_EMPTY)
    {
        /* Get the character from terminal */
        if(Uartfifo.RxIndex >= FIFO_SIZE)Uartfifo.RxIndex = 0;					//循环接收
        Uartfifo.Fifo[Uartfifo.RxIndex++] = UART_1_BLE_UartGetByte();				//接收
        if(++Uartfifo.RxNum > FIFO_SIZE)Uartfifo.RxNum = FIFO_SIZE;			//最大接收字节数
        // Uartfifo.Even = 1;	
        /* Clear UART "RX fifo not empty interrupt" */
        UART_1_BLE_ClearRxInterruptSource(UART_1_BLE_INTR_RX_NOT_EMPTY); //放到最后，防读到后再产生中断  
	}   
}
_Bool ReadUartFifo(uint8_t *pData)
{
    uint8_t OutIndex;
    if(Uartfifo.RxNum > 0)
    {
        UART_1_BLE_SCB_IRQ_Disable();																					//临界保护
        if(Uartfifo.RxNum > Uartfifo.RxIndex)
            OutIndex = FIFO_SIZE - (Uartfifo.RxNum - Uartfifo.RxIndex);
        else
            OutIndex = Uartfifo.RxIndex - Uartfifo.RxNum;
        Uartfifo.RxNum--;
        UART_1_BLE_SCB_IRQ_Enable();																					//临界保护
        *pData = Uartfifo.Fifo[OutIndex];

        return 1;
    }
    //Uartfifo.Even = 0;																									//清标记
    return 0;
}
void Ble_Init()
{
#ifdef PLATFORM
    BLE_POWER(ON);
    
    UART_1_BLE_Start();
    
    UART_1_BLE_SCB_IRQ_StartEx(BLE_ISR);
#endif
}
uint8_t cmdbuf[40], readIndex = 0;

//call in 200ms
_Bool BLE_Decode(char *str,uint8_t *dat)
{
//		char *str;
    _Bool rev= 0;
//    do
//    {
//        if(ReadUartFifo(&cmdbuf[readIndex]))
//        {
//            *dat = cmdbuf[readIndex];
//            if(++readIndex > sizeof(cmdbuf))readIndex = 0;
//            rev = 1;
//        }
//        else break;
//    } while(1);
        if(ReadUartFifo(dat))
            rev = 1;
		//TTM:开头
		//str = strstr(cmdbuf,"TTM:");
//		if()
//			return true;
		//PRINTF(str);
		///r/n结尾





//		{
//			//while(ReadUartFifo(p++));																						//读出来
//			//ReadUartFifo(cmdbuf);
//			//str = strstr(cmdbuf,"TTM:");																							//查找字符串
//			//PRINTF(str);
//			switch(*str)
//			{
//				case 'A':break;
//				case 'B':break;
//				case 'C':break;
//				case 'D':break;
//				case 'E':break;
//				case 'F':break;
//				case 'G':break;
//				case 'H':break;
//				case 'I':break;
//				case 'J':break;
//				case 'K':break;
//				case 'L':break;
//				case 'M':break;
//				case 'N':break;
//				case 'O':break;
//				case 'P':break;
//				case 'Q':break;
//				case 'R':break;
//				case 'S':break;
//				case 'T':break;
//				case 'U':break;
//				case 'V':break;
//				case 'W':break;
//				case 'X':break;
//				case 'Y':break;
//				case 'Z':break;
//				default:break;
//			}
//			return true;
//		}
    return rev;
}


/* [] END OF FILE */
