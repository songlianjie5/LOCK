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
#ifndef _MENUTIPS_H_
#define _MENUTIPS_H_

#include "main.h"
    
typedef struct   
{
  char *Num;
  char *MenuChinese;
  char *MenuEnglish;
  uint16_t SpeakAddr;
}item_t;

typedef struct   
{
  char *TipsChinese;
  char *TipsEnglish;
  uint16_t SpeakAddr;
}tips_t;


extern const item_t main_menu[];
extern const item_t user_menu[];
extern const item_t sys_menu[];
extern const item_t record_menu[];
extern const item_t ext_menu[];

extern const item_t Del_menu[];
extern const item_t Mode_menu[];
extern const item_t Voice_menu[];
extern const item_t Language_menu[];

extern const tips_t TIPS_YZSB;
extern const tips_t TIPS_YTCGLMS;
extern const tips_t TIPS_QSRYHMMYJHJJS;
extern const tips_t TIPS_QSRLWBHYJHJJS;
extern const tips_t TIPS_SRCW;
extern const tips_t TIPS_MMGYJD;
extern const tips_t TIPS_SZSB;
extern const tips_t TIPS_SZCG;
extern const tips_t TIPS_QCXSR;
extern const tips_t TIPS_QNKSZZFYC;
extern const tips_t TIPS_QXZWWYDSZ;
extern const tips_t TIPS_QXYWWYDSZ;
extern const tips_t TIPS_QXSWWYDSZ;
extern const tips_t TIPS_QXXWWYDSZ;
extern const tips_t TIPS_TJSB;
extern const tips_t TIPS_TJCG;
extern const tips_t TIPS_SCSB;
extern const tips_t TIPS_SCCG;
extern const tips_t TIPS_ZWKWK;
extern const tips_t TIPS_BHBCZ;
extern const tips_t TIPS_KPBCZ;
extern const tips_t TIPS_YJRGLMS;
extern const tips_t TIPS_YZCG;
extern const tips_t TIPS_DLDQGHDC;
extern const tips_t TIPS_BHYCZ;
extern const tips_t TIPS_ZWYCZ;
extern const tips_t TIPS_QSRSZ;
extern const tips_t TIPS_GYMS;
extern const tips_t TIPS_DYMS;
extern const tips_t TIPS_JYMS;
extern const tips_t TIPS_XTYSD;
extern const tips_t TIPS_SRCWBJ;
extern const tips_t TIPS_QSHZS;
extern const tips_t TIPS_MMYCZ;
extern const tips_t TIPS_MKYCZ;
extern const tips_t TIPS_MMCW;
extern const tips_t TIPS_MMWX;
extern const tips_t TIPS_QZSRYC;
extern const tips_t TIPS_QAZW;
extern const tips_t TIPS_QSRZWHKP;
extern const tips_t TIPS_WTJZW;
extern const tips_t TIPS_WSZMM;
extern const tips_t TIPS_WTJYHK;
extern const tips_t TIPS_QTJGLMM;
extern const tips_t TIPS_QSFWM;
extern const tips_t TIPS_QCXSK;
extern const tips_t TIPS_YHFDCCSZ;
extern const tips_t TIPS_ZZHFQSH;
extern const tips_t TIPS_QXGGLMM;
extern const tips_t TIPS_QAYKQ;
extern const tips_t TIPS_QSK;
extern const tips_t TIPS_QSRZWHSRLWBHQJHJJS;
extern const tips_t TIPS_QSKHSRLWBHYJHJJS;
extern const tips_t TIPS_KPKWK;
extern const tips_t TIPS_KPKYM;
extern const tips_t TIPS_KPYCZ;
extern const tips_t TIPS_YQKSYKP;
extern const tips_t TIPS_ZWKYM;
extern const tips_t TIPS_MMKYM;
extern const tips_t TIPS_MMKWK;
extern const tips_t TIPS_FZCKS;
extern const tips_t TIPS_FQBJ;
extern const tips_t TIPS_MSYC;
extern const tips_t TIPS_BFBJ;
extern const tips_t TIPS_JXYSKM;
extern const tips_t TIPS_QSKHAZW;
extern const tips_t TIPS_QSRMMHKP;
extern const tips_t TIPS_QSRMMHZW;
extern const tips_t TIPS_KMJLWK;
extern const tips_t TIPS_MM;
extern const tips_t TIPS_ZW;
extern const tips_t TIPS_KP;
extern const tips_t TIPS_JXYC;
extern const tips_t TIPS_YKKM;
extern const tips_t TIPS_YJKQ;
extern const tips_t TIPS_APP;
extern const tips_t TIPS_WZLX;
extern const tips_t TIPS_QSH;
extern const tips_t TIPS_WAQMS;
extern const tips_t TIPS_YKYCZ;
extern const tips_t TIPS_YKBCZ;
extern const tips_t TIPS_YKYPM;
extern const tips_t TIPS_ZZSCMM;
extern const tips_t TIPS_ZZSCZW;
extern const tips_t TIPS_ZZSCKP;
extern const tips_t TIPS_ZZSCKMJL;
extern const tips_t TIPS_QKM;
extern const tips_t TIPS_YGM;
extern const tips_t TIPS_LYKQ;
extern const tips_t TIPS_LYGB;
extern const tips_t TIPS_YQDBF;
extern const tips_t TIPS_YFS;
extern const tips_t TIPS_YYMS;
extern const tips_t TIPS_ZDMS;
extern const tips_t TIPS_SDMS;
extern const tips_t TIPS_MWSS;

extern void DisplayTips(const tips_t *tip);
extern void DisplayTipsEx(const tips_t *tip,uint8_t y,bool clr);
#endif
/* [] END OF FILE */
