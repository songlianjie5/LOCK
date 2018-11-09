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
#include "List.h"

FunNode_t List;

bool ListInit(void)
{
    memset(&List, 0, sizeof(FunNode_t));				                //初始化Head固定地址
}
bool RegisterCallback(FunNode_t *pList, void(*callback)(), char *descr, uint16_t TickCount, uint16_t TickTaget, uint16_t RunCount)
{
	FunNode_t *Node;
	Node = (FunNode_t *)malloc(sizeof(FunNode_t));						//动态分配内存
	if (callback != NULL)
	{
		Node->Fun = callback;
		Node->Describe = descr;
		Node->Tick = (TickCount / BASE_TICK_MS);
		Node->Period = (TickTaget / BASE_TICK_MS);
		Node->RunCount = RunCount;

		for (; pList->Next; pList = pList->Next)
		{
			if (pList->Fun == Node->Fun)break;
		}																//跳到最尾部  //查找是否已有注册过的 有的话注册失败
		if (pList->Fun == Node->Fun)									//最后一个
		{
			free(Node);
			return false;												//返回失败
		}
		Node->Prev = pList;
		Node->Next = NULL;
		pList->Next = Node;												//尾插

		return true;
	}
	return false;
}

bool UnRegisterCallback(FunNode_t *pList, void(*callback)())
{
	if (pList == NULL)return false;
	for (; pList; pList = pList->Next)				
	{
		if (pList->Fun == callback)
		{
			if (pList->Prev != NULL)
				pList->Prev->Next = pList->Next;
			if(pList->Next != NULL)
				pList->Next->Prev = pList->Prev;
			free(pList);							                    //稀放地址
			return true; 							                    //删除成功
		}
	}	
	return false;
}

bool ListRun(FunNode_t *pList)
{
	FunNode_t note;
	if (pList == NULL)
		return false;
	while (pList)
	{
		if (pList->Fun)								                    //防空函数执行
		{
			pList->Tick++;
			if (pList->Tick > pList->Period)
			{
				pList->Tick = 0;
				if (pList->RunCount)				
				{
					if (pList->RunCount != 0xFFFF)pList->RunCount--;
					note = *pList;					                    //先备份
					pList->Fun();					                    //自毁后不能再运行 pList已为空了
					if (pList->Fun != note.Fun)		                    //对比是否本函数已销毁
						return true;
				}
				else
				{
					UnRegisterCallback(&List, pList->Fun);              //自毁
					return true;
				} 
			}
		}
		pList = pList->Next;
	}
	return true;
}
//打印List
bool ListView(FunNode_t *pList)
{
	if (pList == NULL)
		return false;
	while (pList)
	{
		if (pList->Fun)								                    //防空函数执行
			printf("List---------------:%s\r\n", pList->Describe);
		else 
			printf("List-----空NULL-:\r\n");
		pList = pList->Next;
	}
	return true;
}

/* [] END OF FILE */
