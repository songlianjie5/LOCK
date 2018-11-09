/**
  ******************************************************************************
  * @file         adc_hal.h
  * @brief        与adc_hal.c对应的头文件
  * @author       Fli
  * @date         2018-07-10
  * @version      v001
  * @copyright    kaadas
  * 
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#ifndef __ADC_HAL_H__
#define __ADC_HAL_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


void AdcHal_Init(void);

//ErrorStatus AdcHal_InitChangeAndDog(uint8_t ADC_Channel,void (*pCB)(void));
//void AdcHal_StopAdcDog(void);

extern ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel);
extern uint16_t AdcHal_ReadResult(uint8_t chan);


#ifdef __cplusplus
}
#endif

#endif /*__ADC_HAL_H__ */
