#include "Extend.h"

#define  STD_PRINTF 0

void EXTEND_ISR()
{
#ifdef PLATFORM   
    /* Check for "RX fifo not empty interrupt" */
	if(UART_2_EXT_GetRxInterruptSourceMasked()&UART_2_EXT_INTR_RX_NOT_EMPTY)
    {
        /* Get the character from terminal */
//        if(Uartfifo.RxIndex >= FIFO_SIZE)Uartfifo.RxIndex = 0;					//循环接收
//        Uartfifo.Fifo[Uartfifo.RxIndex++] = UART_2_EXT_INTR_UartGetByte();		//接收
//        if(++Uartfifo.RxNum > FIFO_SIZE)Uartfifo.RxNum = FIFO_SIZE;			//最大接收字节数
        UART_2_EXT_UartGetByte();
        /* Clear UART "RX fifo not empty interrupt" */
        UART_2_EXT_ClearRxInterruptSource(UART_2_EXT_INTR_RX_NOT_EMPTY); //放到最后，防读到后再产生中断  
	}   
    
#endif
}

#if  STD_PRINTF
//#pragma weak fputc
int fputc(int ch, FILE *f)
{
    UART_2_EXT_UartPutChar(ch);
    return 1;
}

//#pragma weak fgetc
int fgetc(FILE *f)
{
    char ch;
	ch  = UART_2_EXT_UartGetByte();
    return ch;
}
#endif

void Extend_Init()
{
#ifdef PLATFORM
    UART_2_EXT_Start(); //启用串口2
    
    UART_2_EXT_SCB_IRQ_StartEx(EXTEND_ISR);//开启串口2中断
#endif
}





