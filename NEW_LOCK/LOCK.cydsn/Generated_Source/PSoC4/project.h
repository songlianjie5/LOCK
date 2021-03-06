/*******************************************************************************
* File Name: project.h
* 
* PSoC Creator  4.2
*
* Description:
* It contains references to all generated header files and should not be modified.
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2018 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "cyfitter_cfg.h"
#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cydisabledsheets.h"
#include "UART_1_BLE.h"
#include "UART_1_BLE_SPI_UART.h"
#include "UART_1_BLE_PINS.h"
#include "UART_1_BLE_SPI_UART_PVT.h"
#include "UART_1_BLE_PVT.h"
#include "UART_1_BLE_BOOT.h"
#include "UART_0_FPC.h"
#include "UART_0_FPC_SPI_UART.h"
#include "UART_0_FPC_PINS.h"
#include "UART_0_FPC_SPI_UART_PVT.h"
#include "UART_0_FPC_PVT.h"
#include "UART_0_FPC_BOOT.h"
#include "UART_2_EXT.h"
#include "UART_2_EXT_SPI_UART.h"
#include "UART_2_EXT_PINS.h"
#include "UART_2_EXT_SPI_UART_PVT.h"
#include "UART_2_EXT_PVT.h"
#include "UART_2_EXT_BOOT.h"
#include "SPI_0_OLED_FLASH.h"
#include "SPI_0_OLED_FLASH_SPI_UART.h"
#include "SPI_0_OLED_FLASH_PINS.h"
#include "SPI_0_OLED_FLASH_SPI_UART_PVT.h"
#include "SPI_0_OLED_FLASH_PVT.h"
#include "SPI_0_OLED_FLASH_BOOT.h"
#include "SPI_1_CARD.h"
#include "SPI_1_CARD_SPI_UART.h"
#include "SPI_1_CARD_PINS.h"
#include "SPI_1_CARD_SPI_UART_PVT.h"
#include "SPI_1_CARD_PVT.h"
#include "SPI_1_CARD_BOOT.h"
#include "CARD_RESET.h"
#include "CARD_RESET_aliases.h"
#include "OLED_DC.h"
#include "OLED_DC_aliases.h"
#include "I2C_SCL.h"
#include "I2C_SCL_aliases.h"
#include "I2C_SDA.h"
#include "I2C_SDA_aliases.h"
#include "I2C_ST.h"
#include "I2C_ST_aliases.h"
#include "CapSense.h"
#include "CapSense_Control.h"
#include "CapSense_Filter.h"
#include "CapSense_Processing.h"
#include "CapSense_Sensing.h"
#include "CapSense_Structure.h"
#include "CapSense_Tuner.h"
#include "CapSense_Configuration.h"
#include "CapSense_SensingCSD_LL.h"
#include "CapSense_RegisterMap.h"
#include "BAT_ADC.h"
#include "BAT_ADC_aliases.h"
#include "ADC.h"
#include "MOTO_ADC.h"
#include "MOTO_ADC_aliases.h"
#include "RF_ADC.h"
#include "RF_ADC_aliases.h"
#include "VOICE_BUSY.h"
#include "VOICE_BUSY_aliases.h"
#include "VOICE_DATA.h"
#include "VOICE_DATA_aliases.h"
#include "isr_1.h"
#include "LOCK_INT.h"
#include "LOCK_INT_aliases.h"
#include "GPIO_ISR1.h"
#include "GPIO_ISR1_aliases.h"
#include "isr_2.h"
#include "OP3_INT.h"
#include "OP3_INT_aliases.h"
#include "OP1_INT.h"
#include "OP1_INT_aliases.h"
#include "COVER_CHK_INT.h"
#include "COVER_CHK_INT_aliases.h"
#include "isr_wdt.h"
#include "RTC_1.h"
#include "Em_EEPROM.h"
#include "Opamp_Rf.h"
#include "Opamp_Bat.h"
#include "TEST.h"
#include "TEST_aliases.h"
#include "UART_1_BLE_SCBCLK.h"
#include "UART_1_BLE_tx.h"
#include "UART_1_BLE_tx_aliases.h"
#include "UART_1_BLE_SCB_IRQ.h"
#include "UART_1_BLE_rx.h"
#include "UART_1_BLE_rx_aliases.h"
#include "UART_0_FPC_SCBCLK.h"
#include "UART_0_FPC_tx.h"
#include "UART_0_FPC_tx_aliases.h"
#include "UART_0_FPC_SCB_IRQ.h"
#include "UART_0_FPC_rx.h"
#include "UART_0_FPC_rx_aliases.h"
#include "UART_2_EXT_SCBCLK.h"
#include "UART_2_EXT_tx.h"
#include "UART_2_EXT_tx_aliases.h"
#include "UART_2_EXT_SCB_IRQ.h"
#include "UART_2_EXT_rx.h"
#include "UART_2_EXT_rx_aliases.h"
#include "SPI_0_OLED_FLASH_SCBCLK.h"
#include "SPI_0_OLED_FLASH_ss1_m.h"
#include "SPI_0_OLED_FLASH_ss1_m_aliases.h"
#include "SPI_0_OLED_FLASH_sclk_m.h"
#include "SPI_0_OLED_FLASH_sclk_m_aliases.h"
#include "SPI_0_OLED_FLASH_miso_m.h"
#include "SPI_0_OLED_FLASH_miso_m_aliases.h"
#include "SPI_0_OLED_FLASH_mosi_m.h"
#include "SPI_0_OLED_FLASH_mosi_m_aliases.h"
#include "SPI_0_OLED_FLASH_ss0_m.h"
#include "SPI_0_OLED_FLASH_ss0_m_aliases.h"
#include "SPI_1_CARD_SCBCLK.h"
#include "SPI_1_CARD_sclk_m.h"
#include "SPI_1_CARD_sclk_m_aliases.h"
#include "SPI_1_CARD_miso_m.h"
#include "SPI_1_CARD_miso_m_aliases.h"
#include "SPI_1_CARD_mosi_m.h"
#include "SPI_1_CARD_mosi_m_aliases.h"
#include "SPI_1_CARD_ss0_m.h"
#include "SPI_1_CARD_ss0_m_aliases.h"
#include "CapSense_ISR.h"
#include "ADC_IRQ.h"
#include "ADC_intClock.h"
#include "CyFlash.h"
#include "CyLib.h"
#include "cyPm.h"
#include "cytypes.h"
#include "cypins.h"
#include "core_cm0plus_psoc4.h"
#include "CyDMA.h"
#include "CyLFClk.h"
#include "cy_em_eeprom.h"

/*[]*/

