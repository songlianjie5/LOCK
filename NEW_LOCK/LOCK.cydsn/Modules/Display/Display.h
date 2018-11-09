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
#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "main.h"

#define _CMD                 		0
#define _DAT                 		1

typedef struct {
    uint16_t GBcode;
    uint8_t dat[32];
} GB2313_t;

#define CN_NUM	140
extern const GB2313_t CN16X16[];														//中文字库
extern const uint8_t F8X16[];															//英文、数字 字库
extern const char Bterry20x10[];														//电池图片
extern const char Bterry1x10[];															//电量图片

extern int8_t Display_init(void);
extern void Display_clear_screen(void);
extern void Display_picture(uint8_t x, uint8_t y, uint8_t w, uint8_t h, char *pic);
extern void Display_char(uint8_t x, uint8_t y, uint8_t chr,uint8_t N);                  //N为是否正反显示
extern void Display_english_str(uint8_t x, uint8_t y, char *str, uint8_t N);            //N为是否正反显示
extern void Display_chinese_str(uint8_t x, uint8_t y, char *str, uint8_t N);            //N为是否正反显示

#endif

/* [] END OF FILE */
