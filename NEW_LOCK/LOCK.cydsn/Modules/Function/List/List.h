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
#ifndef _LIST_H
#define _LIST_H

#include "main.h"
    
typedef struct FunNode {
	void(*Fun)(void);
	char *Describe;
	uint16_t Tick;
	uint16_t Period;
	uint16_t RunCount;
	struct FunNode *Prev;
	struct FunNode *Next;
}FunNode_t;

#define BASE_TICK_MS TIME_BASIC_MS                                                                                                          //基本运行时间单位ms

extern bool RegisterCallback(FunNode_t *pList, void(*callback)(), char *descr, uint16_t TickCount, uint16_t TickTaget, uint16_t RunCount);  //注册函数
extern bool UnRegisterCallback(FunNode_t *pList, void(*callback)());                                                                        //注消函数
extern bool ListRun(FunNode_t *pList);                                                                                                      //运行链表
extern bool ListInit(void);                                                                                                                 //初始化头
extern FunNode_t List;

/*例
void LedTest(void)
{
    printf("Led\r\n");
}
void TimeTest(void)
{
    if(RegisterCallback(&List,LedTest,"Led",0,100,0xFFFF)==false)
        printf("TimeTest Fail: %d\r\n",__LINE__);
    else 
        UnRegisterCallback(&List,TimeTest);             
}

void main(void)
{
    ListInit();
    RegisterCallback(&List,TimeTest,"Time",0,200,0xFFFF); 
    while(1)
    {
        //call in BASE_TICK_MS
        ListRun(&List);
    }
}
*/
#endif
/* [] END OF FILE */
