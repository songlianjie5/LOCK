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
#ifndef _CARD_H
#define _CARD_H
    
#include "main.h"
#include "nxp520.h"
    
typedef struct{
    uint8_t ID[4];
    bool CardAct;
    uint16_t adc;
    uint16_t Evr;
}Card_t;

/*******************外部函数声明*****************/    
void Card_Init(void);
int8_t Card_ReadId(uint8_t *pTemp);
bool Sleep_SearchCard(void);
Card_t *Card_Status(void);
#endif

/* [] END OF FILE */
