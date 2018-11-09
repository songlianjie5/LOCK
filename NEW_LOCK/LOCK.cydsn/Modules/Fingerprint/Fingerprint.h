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
#ifndef _FINGERPRINT_H
#define _FINGERPRINT_H

#include "main.h"  

#define REG_NUM 	8
#define REG_LOGIC	1

typedef enum{
    F_NULL,
    GET_ING,
	GET_OK,
	GET_FAIL,
	GENCHAR_OK,
	SEARCH_OK,
	REGMODEL_OK,
	STORE_OK,
    SLEEP_OK,
    EMPTY_OK,
    EMPTY_FAIL,
	CMD_FAIL,
    SEARCH_FAIL,
    STORE_FAIL,
	GENCHAR_SAME,
    WAIT_ISR,
}CmdEven_t;
typedef struct{
    uint8_t step;
    uint8_t BID;
	uint16_t PID;
	CmdEven_t NormalEven;
	CmdEven_t RegEven;
    bool PowerStaus;
	bool SensorIsr;
}Fingerprint_t;

extern int8_t Fingerprint_init(void);
extern void Fingerprint_Sleep(void);
extern void Fingerprint_Power(bool option);
extern Fingerprint_t *Fingerprint_status(void);
extern int8_t Fingerprint_NormalProcess(void);
extern int8_t Fingerprint_RegProcess(void);
extern int8_t Fingerprint_RegDelProcess(uint8_t pid,bool all);          //all为true时全清空 false时按pid删除

#endif

/* [] END OF FILE */
