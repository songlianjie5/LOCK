/**
  ******************************************************************************
  * @file         adc_hal.c
  * @brief        adc底层驱动程序，提供给电池模块、检卡模块、电机模块使用
  * @author       Fli
  * @date         2018-07-10
  * @version      v001
  * @copyright    kaadas
  * 
  * @note         凯迪仕·智能锁
  *
  ******************************************************************************
  */
#include "adc_hal.h"


#define SINCONVER()       {ADC_SAR_START_CTRL_REG = ADC_FW_TRIGGER;} //单次转换
#define CONTCONVER()      {ADC_SAR_SAMPLE_CTRL_REG |= ADC_CONTINUOUS_EN;}//连续转换
#define CONTCONVERSTOP()  {if (ADC_SAR_SAMPLE_CTRL_REG&(uint32)ADC_CONTINUOUS_EN)) ADC_SAR_SAMPLE_CTRL_REG &= (uint32)(~ADC_CONTINUOUS_EN);}//连续转换停止
  

__IO static FlagStatus adcMutexFlag; //ADC互斥标志
//__IO static void (*pAdcDogCB)(void); //模拟看门狗回调
//static void ADC_IRQHandler(void);


/**
  * @brief  初始化ADC配置一个通道
  * @note   
  *         
  * @param  ADC_Channel: ADC通道
  * @return ADC已占用返回ERROR，正常返回SUCCESS
  */
ErrorStatus AdcHal_InitChange(uint8_t ADC_Channel)
{
	uint8_t chan = (1 << ADC_Channel);
    
    if (adcMutexFlag == SET)
	{
		return ERROR;//ADC已占用
	}
	adcMutexFlag = SET;
    
    ADC_SetChanMask(chan);
	
	return SUCCESS;
}

/**
  * @brief  软件启动一次ADC转换并读取转换结果
  * @note   
  *
  * @return 返回结果
  */
uint16_t AdcHal_ReadResult(uint8_t chan)
{
	uint16_t value = 0;
    
    uint16_t i = 0;
		
	adcMutexFlag = RESET;
    
    SINCONVER();
    
    while(ADC_IsEndConversion(ADC_WAIT_FOR_RESULT)==0 && (++i)<10000);
    
    value = ADC_GetResult16(chan);
	
	return value;
}

/**
  * @brief  初始化一个通道并配置模拟看门狗监控该通道
  * @note   
  *         
  * @param  ADC_Channel：ADC通道
  * @param  TrigConv：ADC触发源
  * @param  HighThreshold：模拟看门狗高阀值
  * @param  pCB：模拟看门狗回调
  * @return ADC已占用返回ERROR，正常返回SUCCESS
  */
//ErrorStatus AdcHal_InitChangeAndDog(uint8_t ADC_Channel,void (*pCB)(void))
//{	
//	/* 回调 */
//    uint8_t chan = (1 << ADC_Channel);
//    
//    if (adcMutexFlag == SET)
//	{
//		return ERROR;//ADC已占用
//	}
//    adcMutexFlag = SET;
//    
//    ADC_SetChanMask(chan);
//    
//    ADC_IRQ_StartEx(ADC_IRQHandler); 
//    
//    ADC_IRQ_Enable();
//    
//    CONTCONVER();
//    
//	pAdcDogCB = (__IO void (*)(void))pCB; 
//	
//	return SUCCESS;
//}

/**
  * @brief  关闭模拟看门狗
  *         
  * @note   
  */
void AdcHal_StopAdcDog(void)
{
    if (ADC_SAR_SAMPLE_CTRL_REG&(uint32)ADC_CONTINUOUS_EN)
	{
        ADC_SAR_SAMPLE_CTRL_REG &= (uint32)(~ADC_CONTINUOUS_EN);
		
		adcMutexFlag = RESET;
	}
}

/**
  * @brief  ADC中断服务函数
  *         
  * @note   
  */
//static void ADC_IRQHandler(void)
//{
//    uint32 intr_status;
//    /* Read interrupt status register */
//    intr_status = ADC_SAR_INTR_REG;
//	if (pAdcDogCB != NULL)
//	{
//		pAdcDogCB();
//	}
///* Clear handled interrupt */
//    ADC_SAR_INTR_REG = intr_status;
//    
//    ADC_IRQ_Disable();
//}

/**
  * @brief  初始化
  * @note    
  */
void AdcHal_Init(void)
{
	adcMutexFlag = RESET;
    
    ADC_Start();
    
//    ADC_IRQ_Disable();  
}

