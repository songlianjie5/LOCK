/***************************************************************************//**
* \file CapSense_SensingCSD_LL.c
* \version 5.0
*
* \brief
*   This file defines the data structure global variables and provides
*   implementation for the low-level APIs of the CSD part of
*   the Sensing module. The file contains the APIs used for the CSD block
*   initialization, calibration, and scanning.
*
* \see CapSense v5.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2017), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/
#include "CapSense_Structure.h"
#include "CapSense_Configuration.h"
#include "CapSense_SensingCSD_LL.h"
#include "cyapicallbacks.h"

#if (CapSense_ENABLE == CapSense_CSD_EN)

/***************************************
* API Constants
***************************************/

#if (CapSense_ENABLE == CapSense_CSDV2)

#if (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN)
    #define CapSense_CSD_AZ_ENABLE_CFG                  (CapSense_CSD_AZ_EN_MASK)
#else
    #define CapSense_CSD_AZ_ENABLE_CFG                  (0uL)
#endif /* (CapSense_ENABLE == CapSense_CSD_AUTO_ZERO_EN) */

#if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
    #define CapSense_HSCMP_SCAN_MASK                    (CapSense_HSCMP_EN_MASK | CapSense_CSD_AZ_ENABLE_CFG |\
                                                                 CapSense_HSCMP_INVERT_MASK)
#else
    #define CapSense_HSCMP_SCAN_MASK                    (CapSense_HSCMP_EN_MASK | CapSense_CSD_AZ_ENABLE_CFG)
#endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */

#define CapSense_HSCMP_INIT_MASK                        (CapSense_HSCMP_EN_MASK | CapSense_CSD_AZ_ENABLE_CFG)

#define CapSense_DEFAULT_SW_SHIELD_SEL                  (0x00000000uL)
#define CapSense_DEFAULT_CSD_SW_DSI_SEL                 (CapSense_SW_DSI_SEL_CMODPAD | CapSense_SW_DSI_SEL_TANKPAD)
#define CapSense_DEFAULT_CSD_INTR_SET                   (0x00000000uL)
#define CapSense_DEFAULT_SW_HS_P_SEL                    (0x00000000uL)
#define CapSense_DEFAULT_SW_HS_N_SEL                    (0x00000000uL)
#define CapSense_DEFAULT_CSD_SW_FW_TANK_SEL             (0x00000000uL)
#define CapSense_DEFAULT_CSD_ADC_CTL                    (0x00000000uL)
#define CapSense_DEFAULT_HSCMP_CFG                      (0x00000000uL)

#else

    /* Set IDAC ranges */
    #if (CapSense_IDAC_GAIN_8X == CapSense_CSD_IDAC_GAIN)
        #define CapSense_DEFAULT_IDAC_MOD_RANGE         (CapSense_IDAC_MOD_RANGE_MASK)
        #define CapSense_DEFAULT_IDAC_COMP_RANGE        (CapSense_IDAC_COMP_RANGE_MASK)
    #else
        #define CapSense_DEFAULT_IDAC_MOD_RANGE         (0u)
        #define CapSense_DEFAULT_IDAC_COMP_RANGE        (0u)
    #endif /* (CapSense_IDAC_GAIN_8X == CapSense_CSD_IDAC_GAIN) */

    /* Defining default IDACs configuration according to settings in customizer. */
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        #define CapSense_DEFAULT_CSD_IDAC_CONFIG        (CapSense_IDAC_MOD_MODE_VARIABLE |\
                                                                 CapSense_IDAC_COMP_MODE_FIXED |\
                                                                 CapSense_DEFAULT_IDAC_MOD_RANGE |\
                                                                 CapSense_DEFAULT_IDAC_COMP_RANGE)
    #else
        #define CapSense_DEFAULT_CSD_IDAC_CONFIG        (CapSense_IDAC_MOD_MODE_VARIABLE |\
                                                                 CapSense_DEFAULT_IDAC_MOD_RANGE)
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

#endif /* (CapSense_ENABLE == CapSense_CSDV2) */

/***************************************
* Variables
***************************************/

#if (CapSense_ENABLE == CapSense_CSDV2)
    uint32 CapSense_configCsd = CapSense_DEFAULT_CSD_CONFIG;
#else
    uint32 CapSense_configCsd = CapSense_DEFAULT_CSD_CONFIG | CapSense_DEFAULT_MODULATION_MODE;
    static uint32 CapSense_counterResolution = CapSense_CNT_RESOLUTION_12_BITS;
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */

/* Flag to indicate electrodes that were connected previously */
static uint8 CapSense_eleCsdDisconnectFlag = 0u;
#if ((CapSense_ENABLE == CapSense_CSDV2) && (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN))
    /* Number of re-samplings when the bad conversion occurs */
    uint8 CapSense_badConversionsNum = CapSense_BAD_CONVERSIONS_NUM;
#endif /* ((CapSense_ENABLE == CapSense_CSDV2) && (CapSense_ENABLE == CapSense_CSD_NOISE_METRIC_EN)) */

#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    /* Stores IDAC and raw count that corresponds to a sensor with maximum Cp within a widget */
    uint8 CapSense_calibratedIdac = 1u;
    uint16 CapSense_calibratedRawcount = 1u;
    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
        uint8 CapSense_calibratedIdacRow = 1u;
        uint16 CapSense_calibratedRawcountRow = 1u;
    #endif /*(CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/

/**
* \cond SECTION_C_INTERNAL
* \addtogroup group_c_internal
* \{
*/

#if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
    static void CapSense_SsSetShieldDelay(uint32 delay);
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
        static void CapSense_SsCSDEnableShieldTank(void);
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
    #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
        static void CapSense_SsCSDEnableShieldElectrodes(void);
    #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */
#endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

#if ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
     (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))
    static cystatus CapSense_SsCSDCalibrateCheck(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt, uint32 rawLevel, uint32 rawData);
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        static void CapSense_SsCSDSetSingleIdacMode(void);
        static void CapSense_SsCSDRestoreIdacMode(void);
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
    static cystatus CapSense_SsCSDCalibrateOneSensor(uint32 rawLevel,
                                                             uint8 *ptrIdac,
                                                             uint8 *iDAC8Max);
#endif /* ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
           (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))  */
#if (CapSense_ENABLE == CapSense_CSDV2)
    static void CapSense_SsCSDSetFilterDelay(void);
#else
    CY_INLINE static void CapSense_SsCSDClockRestart(void);
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */
CY_INLINE static void CapSense_SsCSDCmodPrecharge(void);
CY_INLINE static void CapSense_SsCSDTriggerScan(void);
static void CapSense_SsCSDConfigIDACs(void);
static void CapSense_SsCSDSetModeSnsClockDivider(uint32 snsClkSource, uint32 snsClkDivider);

/** \}
* \endcond */


#if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsSetShieldDelay
    ****************************************************************************//**
    *
    * \brief
    *   This is an internal function that sets a shield delay.
    *
    * \details
    *   The function updates the CSD configuration register bits that define the shield
    *   signal delay relatively to the sense signal.
    *
    * \param delay
    *   Specifies the shield delay value:
    *    PSoC 4100/4200 value interpretation:
    *         0 - no delay
    *         1 - 1 cycle of HFCLK
    *         2 - 2 cycles of HFCLK
    *    Third-generation HW block (except 4100/4200 devices):
    *         0 - no delay
    *         3 - 10ns delay
    *         2 - 50ns delay
    *    Fourth-generation HW block:
    *         0 - no delay
    *         1 - 5ns delay
    *         2 - 10ns delay
    *         3 - 20ns delay
    *
    *******************************************************************************/
    static void CapSense_SsSetShieldDelay(uint32 delay)
    {
        /* Update CSD config register shield delay bits with shield delay value */
        CapSense_configCsd &= (uint32)(~CapSense_CONFIG_SHIELD_DELAY_MASK);
        CapSense_configCsd |= (delay << CapSense_SHIELD_DELAY_SHIFT);
    }

    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDEnableShieldTank
        ****************************************************************************//**
        *
        * \brief
        *   This function configures Csh pin
        *
        * \details
        *   The function updates the CSH_HSIOM register to connect Csh to AMUXBUS-B.
        *   For Third-generation HW block it additionally sets Strong drive mode for Csh
        *   and applies High level to Csh pin when precharge is set to IO buffer.
        *
        *******************************************************************************/
        static void CapSense_SsCSDEnableShieldTank(void)
        {
            uint32 newRegValue;

            /* Update the CSH_HSIOM register to connect Csh to AMUXBUS-B */
            newRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
            newRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
            newRegValue |= (CapSense_CSH_TO_AMUXBUS_B_MASK << CapSense_CSH_HSIOM_SHIFT);
            CY_SET_REG32(CapSense_CSH_HSIOM_PTR, newRegValue);

            #if ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
                 (CapSense_DISABLE == CapSense_CSDV2))
                /* Set Strong drive mode for Csh */
                newRegValue = CY_GET_REG32(CapSense_CSH_PC_PTR);
                newRegValue &= (uint32)(~(CapSense_CSH_PC_MASK << CapSense_CSH_PC_SHIFT));
                newRegValue |= (CapSense_CSH_PC_STRONG_MODE << CapSense_CSH_PC_SHIFT);
                CY_SET_REG32(CapSense_CSH_PC_PTR, newRegValue);

                /* Appliy High level to Csh pin for Csh */
                newRegValue = CY_GET_REG32(CapSense_CSH_DR_PTR);
                newRegValue &= (uint32)(~(CapSense_DR_MASK << CapSense_CSH_SHIFT));
                newRegValue |= (uint32)CapSense_CSH_DR_CONFIG;
                CY_SET_REG32(CapSense_CSH_DR_PTR, newRegValue);
            #endif /* ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
                       (CapSense_DISABLE == CapSense_CSDV2)) */
        }
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */

    #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDEnableShieldElectrodes
        ****************************************************************************//**
        *
        * \brief
        *   This internal function initializes Shield Electrodes.
        *
        * \details
        *   The function sets the bit in the HSIOM register which enables the shield electrode
        *   functionality on the pin. The port and pin configurations are stored in
        *   the CapSense_shieldIoList structure.
        *
        *******************************************************************************/
        static void CapSense_SsCSDEnableShieldElectrodes(void)
        {
            uint32 newRegValue;
            uint32 shieldIndex;

            for (shieldIndex = 0u; shieldIndex < CapSense_CSD_TOTAL_SHIELD_COUNT; shieldIndex++)
            {
                /* Set drive mode to Analog */
                CY_SET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr) &
                           ~(CapSense_GPIO_PC_MASK <<(CapSense_shieldIoList[shieldIndex].shift)));

                /* Set HSIOM register mode to CSD_SHIELD or AMUXBUS B connection */
                newRegValue = CY_GET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr);
                newRegValue &= ~(CapSense_HSIOM_SEL_MASK << (CapSense_shieldIoList[shieldIndex].hsiomShift));

                /* Set HSIOM register mode to CSD_SHIELD */
                newRegValue |= (CapSense_HSIOM_SEL_CSD_SHIELD << (CapSense_shieldIoList[shieldIndex].hsiomShift));

                CY_SET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr, newRegValue);
            }
        }


        /*******************************************************************************
        * Function Name: CapSense_SsCSDDisableShieldElectrodes
        ****************************************************************************//**
        *
        * \brief
        *   This internal function disables Shield Electrodes.
        *
        * \details
        *   The function resets the bit in the HSIOM register which disables the shield
        *   electrode functionality on the pin. The port and pin configurations are
        *   stored in  the CapSense_shieldIoList structure.
        *
        *******************************************************************************/
        void CapSense_SsCSDDisableShieldElectrodes(void)
        {
            uint32 newRegValue;
            uint32 shieldIndex;

            for (shieldIndex = 0u; shieldIndex < CapSense_CSD_TOTAL_SHIELD_COUNT; shieldIndex++)
            {
                /* Reset HSIOM register (to GPIO state) */
                CY_SET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].hsiomPtr) &
                             (uint32)~(uint32)(CapSense_HSIOM_SEL_MASK << CapSense_shieldIoList[shieldIndex].hsiomShift));

                /* Update port configuration register (drive mode to ground)  */
                newRegValue = CY_GET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr);
                newRegValue &= ~(CapSense_GPIO_PC_MASK << CapSense_shieldIoList[shieldIndex].shift);
                newRegValue |= (CapSense_SNS_GROUND_CONNECT << CapSense_shieldIoList[shieldIndex].shift);
                CY_SET_REG32(CapSense_shieldIoList[shieldIndex].pcPtr, newRegValue);

                /* Set logic 0 to port data register */
                CY_SET_REG32(CapSense_shieldIoList[shieldIndex].drPtr, CY_GET_REG32(CapSense_shieldIoList[shieldIndex].drPtr) &
                           (uint32)~(uint32)((uint32)1u << CapSense_shieldIoList[shieldIndex].drShift));
            }
        }
    #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */
#endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */


/*******************************************************************************
* Function Name: CapSense_SsCSDSetModeSnsClockDivider
****************************************************************************//**
*
* \brief
*   Sets sense source and Sense Clock Divider
*
* \details
*   For Fourth-generation HW block: Updates CapSense_SENSE_PERIOD register with
*   sense source and Sense Clock Divider.
*   For Third-generation HW block: Updates CapSense_configCsd variable with
*   sense source and sets sense clock divider.
*
* \param
*   snsClkSource The sense source for the sense clock.
* \param
*   snsClkDivider The divider value for the sense clock.
*
*******************************************************************************/
static void CapSense_SsCSDSetModeSnsClockDivider(uint32 snsClkSource, uint32 snsClkDivider)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Update reg value with divider and configuration */
        CY_SET_REG32(CapSense_SENSE_PERIOD_PTR, snsClkSource | (snsClkDivider - 1u));
    #else
        /* Set configuration */
        CapSense_configCsd &= ~(CapSense_CONFIG_PRS_SELECT_MASK | CapSense_CSD_PRS_12_BIT);
        CapSense_configCsd |= snsClkSource;

        /* Set sense clock divider */
        CapSense_SsSetSnsClockDivider(snsClkDivider);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


#if (CapSense_ENABLE == CapSense_CSDV2)
    /*******************************************************************************
    * Function Name: CapSense_SsCSDSetFilterDelay
    ****************************************************************************//**
    *
    * \brief
    *   Sets the filter delay for Fourth-generation HW block.
    *
    * \details
    *   This function updates Fourth-generation HW block configuration
    *   variable CapSense_configCsd
    *   with the filter delay which depends on the sample clock frequency.
    *   This variable is written into register during enabling
    *   Fourth-generation HW block .
    *
    *******************************************************************************/
    static void CapSense_SsCSDSetFilterDelay(void)
    {
        #if (CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ)
            uint32 sampleClkFreqHz;
        #endif /* (CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ) */

        CapSense_configCsd &= ~CapSense_CONFIG_FILTER_DELAY_MASK;

        #if (CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ)
            sampleClkFreqHz = CYDEV_BCLK__HFCLK__HZ / (uint32)CapSense_dsRam.modCsdClk;
            if(sampleClkFreqHz <= CapSense_MOD_CSD_CLK_12MHZ)
            {
                CapSense_configCsd |= CapSense_CONFIG_FILTER_DELAY_12MHZ;
            }
            else if(sampleClkFreqHz <= CapSense_MOD_CSD_CLK_24MHZ)
            {
                CapSense_configCsd |= CapSense_CONFIG_FILTER_DELAY_24MHZ;
            }
            else
            {
                CapSense_configCsd |= CapSense_CONFIG_FILTER_DELAY_48MHZ;
            }
        #else
            CapSense_configCsd |= CapSense_CONFIG_FILTER_DELAY_12MHZ;
        #endif /* (CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ) */
    }
#endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */


/*******************************************************************************
* Function Name: CapSense_SsCSDConfigIDACs
****************************************************************************//**
*
* \brief
*   Configures the mode for IDAC registers
*
* \details
*   This function configures the IDAC modes depend on Compensation IDAC
*   enabled or disabled.
*
*******************************************************************************/
static void CapSense_SsCSDConfigIDACs(void)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
    CY_SET_REG32(CapSense_IDAC_MOD_PTR, CapSense_IDAC_MOD_DEFAULT_CFG);
        #if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
             (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        CY_SET_REG32(CapSense_IDACB_PTR, CapSense_IDAC_COMP_DEFAULT_CFG);
        #endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                   (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */
    #else
        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            CY_SET_REG32(CapSense_IDAC_PTR, CapSense_DEFAULT_CSD_IDAC_CONFIG);
        #else
            CY_SET_REG32(CapSense_IDAC_PTR, CY_GET_REG32(CapSense_IDAC_PTR) &
                                                                ~(CapSense_IDAC_MOD_MODE_MASK |
                                                                  CapSense_IDAC_MOD_MASK));
            CY_SET_REG32(CapSense_IDAC_PTR, CY_GET_REG32(CapSense_IDAC_PTR) |
                                                                 CapSense_DEFAULT_CSD_IDAC_CONFIG);
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


/*******************************************************************************
* Function Name: CapSense_SsCSDInitialize
****************************************************************************//**
*
* \brief
*   This API initializes the CSD module.
*
* \details
*   The function performs the following steps for Fourth-generation HW block:
*   1) Sets GPIO output to "0" for all sensor pins;
*   2) Connects CMOD to AMUXBUS-A and to CSDBUS-A;
*   3) Connects CMOD to (sense path) to CSDCOMP;
*   4) Connects Csh_tank to AMUXBUS-B and to CSDBUS-B;
*   5) Connects VREF to REFGEN;
*   6) Configures REFGEN and sets the reference voltage;
*   7) Connects VREF to CSDCOMP and HSCOMP;
*   8) Configures IDAC and connect to CSDBUS-A (to drive CMOD);
*   9) Configures ModClk;
*   10) Configure SnsClk source;
*   11) Sets other CSD configurations (Csd Auto Zero time,
*       Sample Init period, interrupts,
*       CMOD and Csh_tank/shield initialization switch resistance).
*
*   The function performs the following steps for Third-generation HW block:
*   1) Sets all the sensors to the inactive state;
*   2) Enables Shield Electrodes;
*   3) Configures the CSD block and IDACs;
*   4) Connects Cmod to AMUXBUS-A;
*   5) Enables the clocks;
*   6) Sets the shield delay;
*   7) Enables the CSD block; connects Vref Buffer to the AMUX bus.
*
*******************************************************************************/
void CapSense_SsCSDInitialize(void)
{
    uint32 newRegValue;

    /* Set all the sensors to inactive state */
    CapSense_SsClearCSDSensors();

    #if (CapSense_ENABLE == CapSense_CSDV2)
        CapSense_BistDischargeExtCapacitors();
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
    
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        /* Connect shields to AMUX-B bus (config HSIOM regs) */
        #if (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)
            CapSense_SsCSDEnableShieldElectrodes();
        #endif /* (0u != CapSense_CSD_TOTAL_SHIELD_COUNT) */

        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)
            /* Configure Csh */
            CapSense_SsCSDEnableShieldTank();
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

    #if (CapSense_ENABLE == CapSense_CSDV2)

        /* Initialize the unused CSD registers to defaut state */
        CY_SET_REG32(CapSense_SENSE_DUTY_PTR,       CapSense_DEFAULT_SENSE_DUTY_SEL);

        /* Configure VREF */
        newRegValue = CY_GET_REG32(CapSense_SW_REFGEN_SEL_PTR);
        newRegValue |= CapSense_DEFAULT_SW_REFGEN_SEL;

        CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, newRegValue);
        CY_SET_REG32(CapSense_INTR_SET_PTR,         CapSense_DEFAULT_CSD_INTR_SET);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR,   CapSense_DEFAULT_CSD_SW_FW_TANK_SEL);
        CY_SET_REG32(CapSense_SW_DSI_SEL_PTR,       CapSense_DEFAULT_CSD_SW_DSI_SEL);
        CY_SET_REG32(CapSense_ADC_CTL_PTR,          CapSense_DEFAULT_CSD_ADC_CTL);
        CY_SET_REG32(CapSense_AMBUF_PTR,            CapSense_AMBUF_PWR_MODE_OFF);
        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,    CapSense_DEFAULT_SW_SHIELD_SEL);
        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR,      CapSense_DEFAULT_SW_HS_P_SEL);
        CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR,      CapSense_DEFAULT_SW_HS_N_SEL);
        CY_SET_REG32(CapSense_HSCMP_PTR,            CapSense_DEFAULT_HSCMP_CFG);

        /* Connect CMOD to AMUXBUS-A */
        newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
        newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CMOD_HSIOM_SHIFT);
        CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);

        /* Connect AMUXBUS-A to CSDBUS-A */
        newRegValue = CY_GET_REG32(CapSense_SW_BYP_SEL_PTR);
        newRegValue |= CapSense_SW_BYP_SEL_SW_BYA_MASK;
        CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, newRegValue);

        /* Connect CMOD to (sense path) to CSDCOMP */
        #if (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION)
            CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPM_STATIC_CLOSE);
        #elif (CapSense_CSD__CSHIELD_PAD == CapSense_CMOD_CONNECTION)
            CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPS_STATIC_CLOSE);
        #else
            CY_SET_REG32(CapSense_SW_CMP_P_SEL_PTR, CapSense_SW_CMP_P_SEL_SW_SFPT_STATIC_CLOSE);
        #endif /* (CapSense_CSD__CMOD_PAD == CapSense_CMOD_CONNECTION) */

        /* Configure shield driving path */
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            /* Connect AMUXBUS-B to CSDBUS-B (and AMUXBUS-A to CSDBUS-A ) */
            CY_SET_REG32(CapSense_SW_BYP_SEL_PTR, CapSense_SW_BYP_SEL_SW_BYA_MASK | CapSense_SW_BYP_SEL_SW_BYB_MASK);

        /* Connect AMUXBUS-B to HSCMP positive input */
        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, 0x00000000uL);
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        /* Configure VREF */
        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)
            /* Connect VREF to REFGEN. Connect IDACB to CSDBUSA */
            newRegValue = CY_GET_REG32(CapSense_SW_REFGEN_SEL_PTR);
            newRegValue |= CapSense_SW_REFGEN_SEL_SW_SGR_MASK | CapSense_SW_REFGEN_SEL_SW_IAIB_MASK;
            CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, newRegValue);
        #else
            /* Connect VREF to REFGEN (IAIB switch is open) */
            newRegValue = CY_GET_REG32(CapSense_SW_REFGEN_SEL_PTR);
            newRegValue |= CapSense_SW_REFGEN_SEL_SW_SGR_MASK;
            CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, newRegValue);
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN */

    #if (CapSense_2000_MV > CapSense_CYDEV_VDDA_MV)

        /* Connect VREFLOW (from AMBUF and RefGen is in bypass mode) to CSDCOMP when Vdda < 2 V */
        CY_SET_REG32(CapSense_SW_CMP_N_SEL_PTR, CapSense_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE);

        /* Configure REFGEN. Set reference voltage when Vdda < 2 V */
        CY_SET_REG32(CapSense_REFGEN_PTR, CapSense_REFGEN_LV);

        /* Connect Vrefhi to AMUBUF positive input when Vdaa < 2V
         *  Connect AMUBUF to SCDCMP negative input when Vdaa < 2V
         */
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE |
                                                                      CapSense_SW_AMUXBUF_SEL_SW_ICB_PHI2);
            #else
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_DEFAULT);
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
            CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_NORM);
        #else
            CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_OFF);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        /* Connect VREFHI to HSCOMP */
        CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR, CapSense_SW_HS_N_SEL_SW_HCRH_STATIC_CLOSE);
    #else

        /* Connect VREFHI (from RefGen) to CSDCOMP when Vdda >= 2 V */
        CY_SET_REG32(CapSense_SW_CMP_N_SEL_PTR, CapSense_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE);

        /* Configure REFGEN. Set reference voltage when Vdda >= 2 V */
        CY_SET_REG32(CapSense_REFGEN_PTR, CapSense_REFGEN_HV);

        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            /* Turn on CSD_AMBUF high power level when Vdaa >= 2V */
            CY_SET_REG32(CapSense_AMBUF_PTR, CapSense_AMBUF_PWR_MODE_HI);

            #if (CapSense_IDAC_SINKING != CapSense_CSD_IDAC_CONFIG)
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE |
                                                                      CapSense_SW_AMUXBUF_SEL_SW_ICB_PHI2);
            #else
                CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_DEFAULT);
            #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */
        #else
            CY_SET_REG32(CapSense_SW_AMUXBUF_SEL_PTR, CapSense_SW_AMUXBUF_SEL_SW_DEFAULT);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        /* Connect VREFHI to HSCOMP */
        CY_SET_REG32(CapSense_SW_HS_N_SEL_PTR, CapSense_SW_HS_N_SEL_SW_HCRH_STATIC_CLOSE);
    #endif /* (CapSense_2000_MV > CapSense_CYDEV_VDDA_MV) */

    /* Configure IDACs mode */
    CapSense_SsCSDConfigIDACs();

        /* Configure ModClk */
        CapSense_SsSetModClkClockDivider((uint32)CapSense_dsRam.modCsdClk);

    /* Set other CSD configurations */
    #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
        CapSense_SsSetShieldDelay(CapSense_CSD_SHIELD_DELAY);
    #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

    /* Configure HW block filter delay */
    CapSense_SsCSDSetFilterDelay();

    #if (CapSense_DISABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
        /* Enable power to sub-blocks */
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd |
                                                  CapSense_CONFIG_SENSE_EN_MASK |
                                                  CapSense_CONFIG_ENABLE_MASK);
    #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */

        /* Set Csd Auto Zero time (set AZ_TIME bitmask) */
        CY_SET_REG32(CapSense_SEQ_TIME_PTR, CapSense_CSD_AUTO_ZERO_TIME);

        /* Select CMOD and Csh_tank/shield initialization switch resistance */
        CY_SET_REG32(CapSense_SW_RES_PTR, 0x00000000);

        /* Set the number of dummy fine initialization cycles */
        CY_SET_REG32(CapSense_SEQ_INIT_CNT_PTR, CapSense_CSD_FINE_INIT_TIME);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

    #if (CapSense_DISABLE == CapSense_CSDV2)

        /* Configure IDACs mode */
        CapSense_SsCSDConfigIDACs();

        /* Connect Cmod to AMUXBUS-A using HSIOM registers */
        newRegValue = CY_GET_REG32(CapSense_CMOD_HSIOM_PTR);
        newRegValue &= (uint32)(~(uint32)CapSense_CMOD_HSIOM_MASK);
        newRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CMOD_HSIOM_SHIFT);
        CY_SET_REG32(CapSense_CMOD_HSIOM_PTR, newRegValue);

        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            CapSense_SsSetShieldDelay(CapSense_CSD_SHIELD_DELAY);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        /* Enable CSD block. Connect Vref Buffer to AMUX bus to make sure that Cmod is charged before scanning in active mode */
        #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd);
        #else
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd | CapSense_CTANK_PRECHARGE_CONFIG_CSD_EN);
        #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */
    #endif /* (CapSense_DISABLE == CapSense_CSDV2) */

    #if ((CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) &&\
         (CapSense_CLK_SOURCE_DIRECT == CapSense_CSD_SNS_CLK_SOURCE))
         /* Set clock dividers and clock source mode */
        CapSense_SsCSDConfigClock();
    #endif /* ((CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) &&\
               (CapSense_CLK_SOURCE_DIRECT == CapSense_CSD_SNS_CLK_SOURCE)) */
}


/*******************************************************************************
* Function Name: CapSense_SsCSDElectrodeCheck
****************************************************************************//**
*
* \brief
*   Checks if electrodes were previously connected using
 * CapSense_CSDSetupWidgetExt() API and if yes disconnects them.
*
* \details
*   This function checks if CapSense_eleCsdDisconnectFlag is set.
*   If it set, the function disconnects the previously connected electrode.
*   The previous IO is contained in CapSense_curSnsIOPtr and
*   CapSense_curFlashSnsPtr contains the previous data for Ganged
*   sensors.
*
*******************************************************************************/
void CapSense_SsCSDElectrodeCheck(void)
{
    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        uint32 tempVal;
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

    if (CapSense_ENABLE == CapSense_eleCsdDisconnectFlag)
    {
        /* Disconnect if electrodes were previous connected by CSDSetupWidgetExt() API */
        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            /* Check ganged sns flag  */
            if (CapSense_GANGED_SNS_MASK == (CapSense_curFlashWdgtPtr->staticConfig & CapSense_GANGED_SNS_MASK))
            {
                /* Get number of ganged pins */
                tempVal = CapSense_curFlashSnsPtr->numPins;

                /* Get IO pointer  */
                CapSense_curSnsIOPtr = &CapSense_ioList[CapSense_curFlashSnsPtr->firstPinId];

                /* Disconnect all ganged sensors */
                do
                {
                    CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
                    CapSense_curSnsIOPtr++;
                    tempVal--;
                } while (0u != tempVal);
            }
            else
            {
                /* Disconnect ganged sensor */
                CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
            }
        #else
            /* Disable sensor */
            CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */

        CapSense_eleCsdDisconnectFlag = 0u;
    }
}


/*******************************************************************************
* Function Name: CapSense_SsCSDSetUpIdacs
****************************************************************************//**
*
* \brief
*  This internal function changes the IDACs values.
*
* \details
*  If Compensation IDAC is enabled, it updates two IDACs.
*  The Modulator IDAC is common for all the sensors of the widget.
*  The Compensation IDAC is updated for the sensor of the widget which
*  is available in the RAM_SNS_STRUCT structure.
*  If the Compensation IDAC is disabled, the function updates the Modulator IDAC
*  value only in the RAM_WD_BASE_STRUCT structure.
*
* \param
*  ptrWdgt The pointer to the RAM_WD_BASE_STRUCT structure.
*
*******************************************************************************/
void CapSense_SsCSDSetUpIdacs(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt)
{

    uint8 interruptState;

#if (CapSense_ENABLE == CapSense_CSDV2)
    uint32 idacARegValue;
    #if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
         (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        uint32 idacBRegValue;
    #endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
               (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

    interruptState = CyEnterCriticalSection();

    /* Get IDACA Value */
    idacARegValue = CY_GET_REG32(CapSense_IDAC_MOD_PTR);

    /* Set IDACA Value mask */
    idacARegValue &= ~(CapSense_IDAC_MOD_VAL_MASK);

    /* Set IDACA value */
    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
        if (CapSense_dsFlash.wdgtArray[(CapSense_widgetIndex)].numCols <= CapSense_sensorIndex)
        {
            idacARegValue |= (uint32)ptrWdgt->rowIdacMod[CapSense_scanFreqIndex];
        }
        else
        {
            idacARegValue |= (uint32)ptrWdgt->idacMod[CapSense_scanFreqIndex];
        }
    #else
        idacARegValue |= (uint32)ptrWdgt->idacMod[CapSense_scanFreqIndex];
    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */

    /* Update IDACA register with new value */
    CY_SET_REG32(CapSense_IDAC_MOD_PTR, idacARegValue);
    CyExitCriticalSection(interruptState);

    #if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
         (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        interruptState = CyEnterCriticalSection();
        /* Get IDACB Value */
        idacBRegValue = CY_GET_REG32(CapSense_IDAC_COMP_PTR);

        /* Set IDACB Value mask */
        idacBRegValue &= ~(CapSense_IDAC_COMP_VAL_MASK);

        /* Set IDACB value */
        idacBRegValue |= (uint32)CapSense_curRamSnsPtr->idacComp[CapSense_scanFreqIndex];

        /* Update IDACA register with new value */
        CY_SET_REG32(CapSense_IDAC_COMP_PTR, idacBRegValue);
        CyExitCriticalSection(interruptState);
    #endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
               (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

#else

    uint32 newRegValue;
    uint8 const *ptrIdacMod = ptrWdgt->idacMod;

    #if (CapSense_CSD_MATRIX_WIDGET_EN  | CapSense_CSD_TOUCHPAD_WIDGET_EN)
        if (CapSense_dsFlash.wdgtArray[(CapSense_widgetIndex)].numCols <= CapSense_sensorIndex)
        {
            ptrIdacMod = &ptrWdgt->rowIdacMod[0u];
        }
    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN  | CapSense_CSD_TOUCHPAD_WIDGET_EN) */

    interruptState = CyEnterCriticalSection();

    /* Get Idac Value */
    newRegValue = CY_GET_REG32(CapSense_IDAC_PTR);

    /* Set Idac Value mask */
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        newRegValue &= ~(CapSense_IDAC_MOD_MASK | CapSense_IDAC_COMP_MASK);
    #else
        newRegValue &= ~(CapSense_IDAC_MOD_MASK);
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

    /* Set Idac value */
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        newRegValue |= (ptrIdacMod[CapSense_scanFreqIndex] |
                       (uint32)((uint32)CapSense_curRamSnsPtr->idacComp[CapSense_scanFreqIndex] <<
                       CapSense_IDAC_COMP_DATA_OFFSET));
    #else
        newRegValue |= ptrIdacMod[CapSense_scanFreqIndex];
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
    CY_SET_REG32(CapSense_IDAC_PTR, newRegValue);

    CyExitCriticalSection(interruptState);

#endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


#if (CapSense_ENABLE == CapSense_CSDV2)
    /*******************************************************************************
    * Function Name: CapSense_SsCSDGetNumberOfConversions
    ****************************************************************************//**
    *
    * \brief
    *  This function gets Number of conversions.
    *
    * \details
    *  This function gets Number of conversions using foll.owing equation:
    *   conversionsNum = (2^resolution - 1) / snsClkDivider.
    *
    * \param
    *  snsClkDivider The divider value for the sense clock.
    *  resolution The widget resolution.
    *  snsClkSrc The current Sense Clock Source.
    *
    * \return Returns the Number of conversions.
    *
    *******************************************************************************/
    uint32 CapSense_SsCSDGetNumberOfConversions(uint32 snsClkDivider, uint32 resolution, uint32 snsClkSrc)
    {
        uint32 conversionsNum;
        #if ((CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ) && (CapSense_ENABLE != CapSense_CSDV2_REF9P6UA_EN))
            uint32 sampleClkFreqHz;
        #endif /* ((CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ) && (CapSense_ENABLE != CapSense_CSDV2_REF9P6UA_EN)) */

        /* Calculate scanning resolution value in register */
        conversionsNum = (uint32)((1uL << resolution) - 1uL);

        #if(CapSense_ENABLE != CapSense_CSDV2_REF9P6UA_EN)
            /* Apply correction to oveflow for 16 bit resolution */
            if (CapSense_RES16BIT == resolution)
            {
                conversionsNum -= ((snsClkDivider + 1u) >> 1u);

                #if (CapSense_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ)
                    sampleClkFreqHz = CYDEV_BCLK__HFCLK__HZ / (uint32)CapSense_dsRam.modCsdClk;

                    if (sampleClkFreqHz <= CapSense_MOD_CSD_CLK_12MHZ)
                    {
                        conversionsNum -= CapSense_CONFIG_FILTER_DELAY_2_CYCLES;
                    }
                    else if (sampleClkFreqHz <= CapSense_MOD_CSD_CLK_24MHZ)
                    {
                        conversionsNum -= CapSense_CONFIG_FILTER_DELAY_3_CYCLES;
                    }
                    else
                    {
                        conversionsNum -= CapSense_CONFIG_FILTER_DELAY_4_CYCLES;
                    }
                #else
                    conversionsNum -= CapSense_CONFIG_FILTER_DELAY_2_CYCLES;
                #endif /* (INSTANCE_NAME`_MOD_CSD_CLK_12MHZ < CYDEV_BCLK__HFCLK__HZ) */
            }
        #endif /* (CapSense_ENABLE != CapSense_CSDV2_REF9P6UA_EN) */

        if (0u < snsClkDivider)
        {
            conversionsNum /= snsClkDivider;
        }

    #if((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
        (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
        (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
        switch (snsClkSrc)
        {
            case CapSense_CLK_SOURCE_PRS8:
            case CapSense_CLK_SOURCE_PRS12:
                /* Divide by 2 for PRS8/PRS12 mode */
                conversionsNum >>= 1u;
                break;

            default:
                break;
        }
    #endif /* ((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
               (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
               (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)) */

    return((conversionsNum > 0uL) ? (conversionsNum) : 1uL);
    }
#endif /* (CapSense_ENABLE == CapSense_CSDV2) */


/*******************************************************************************
* Function Name: CapSense_SsCSDConfigClock
****************************************************************************//**
*
* \brief
*  This function configure sense clock for different modes.
*
* \details
*  Function sets the clock divider and configures the mode based on configuration.
*
*******************************************************************************/
void CapSense_SsCSDConfigClock(void)
{
    uint32 snsClkDivider;
    uint32 snsClkSrc;
    uint32 newRegValue;

    CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
             CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2WdgtRam;

    /* Get sense divider based on configuration */
    #if (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN)
        snsClkDivider = (uint32)CapSense_dsRam.snsCsdClk;
    #else
        #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
            /* Get SnsClck divider for rows or columns */
            if (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].numCols <= CapSense_sensorIndex)
            {
                snsClkDivider = (uint32)(ptrWdgt->rowSnsClk);
            }
            else
            {
                snsClkDivider = (uint32)(ptrWdgt->snsClk);
            }
        #else
            snsClkDivider = (uint32)(ptrWdgt->snsClk);
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

    /* Get sense clk source calculated in CapSense_SsCSDInitialize() before */
    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
        /* Get SnsClc Source for rows or columns */
        if (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].numCols <= CapSense_sensorIndex)
        {
            snsClkSrc = (uint32)ptrWdgt->rowSnsClkSource;
        }
        else
        {
            snsClkSrc = (uint32)ptrWdgt->snsClkSource;
        }
    #else
        snsClkSrc = (uint32)ptrWdgt->snsClkSource;
    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

    #if (CapSense_ENABLE == CapSense_CSDV2)
        newRegValue = (snsClkSrc << CYFLD_CSD_LFSR_SIZE__OFFSET);

        /* Configuring PRS SEL_BIT */
        if ((CapSense_CLK_SOURCE_PRS8 == snsClkSrc) ||
            (CapSense_CLK_SOURCE_PRS12 == snsClkSrc))
        {
            newRegValue |= CapSense_SENSE_PERIOD_SEL_LFSR_MSB_MASK;
        }
    #else
        newRegValue = snsClkSrc;
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */

    #if((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
        (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
        (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
        switch (snsClkSrc)
        {
        case CapSense_CLK_SOURCE_PRS8:
        case CapSense_CLK_SOURCE_PRS12:
            /* Divide by 2 for PRS8/PRS12 mode */
            snsClkDivider >>= 1;
            if (snsClkDivider == 0u)
            {
                snsClkDivider = 1u;
            }
            break;

        default:
            break;
        }
    #endif /* ((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
               (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
               (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)) */
    CapSense_SsCSDSetModeSnsClockDivider(newRegValue, snsClkDivider);
}


/*******************************************************************************
* Function Name: CapSense_SsCSDCalculateScanDuration
****************************************************************************//**
*
* \brief
*   Calculates Scan Duration which is defined by scan resolution
*
* \details
*   For Fourth-generation HW block: The function calculates the number of conversions and updates
*   SEQ_NORM_CNT register. The number of conversions depends on resolution and
*   snsClk divider.
*   For Third-generation HW block: The function recalculate the resolution using following equation:
*   2^resolution - 1. The calculated value is contained in
*   CapSense_counterResolution global variable and used in
*   CapSense_SsCSDStartSample() function to trigger the scan process.
*
* \param wdgtIndex
*  ptrWdgt The pointer to the RAM_WD_BASE_STRUCT structure.
*
*******************************************************************************/
void CapSense_SsCSDCalculateScanDuration(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)
        uint32 conversionsNum;
        uint32 snsClkDivider;

    /* Get Number Of Conversions */
    #if (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN)
        snsClkDivider = CapSense_dsRam.snsCsdClk;
    #else
        #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
            /* Get SnsClck divider for rows or columns */
            if (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].numCols <= CapSense_sensorIndex)
            {
                snsClkDivider = (uint32)(ptrWdgt->rowSnsClk);
            }
            else
            {
                snsClkDivider = (uint32)(ptrWdgt->snsClk);
            }
        #else
            snsClkDivider = (uint32)(ptrWdgt->snsClk);
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
    #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

    conversionsNum = CapSense_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution, (uint32)ptrWdgt->snsClkSource);

        /* Set Number Of Conversions based on scanning resolution */
        CY_SET_REG32(CapSense_SEQ_NORM_CNT_PTR, (conversionsNum & CapSense_SEQ_NORM_CNT_CONV_CNT_MASK));
    #else
        /* Set up scanning resolution  */
        CapSense_counterResolution = (uint32)((0x00000001Lu << ptrWdgt->resolution) - 1u) << CapSense_RESOLUTION_OFFSET;
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


/*******************************************************************************
* Function Name: CapSense_CSDSetupWidget
****************************************************************************//**
*
* \brief
*  Performs hardware and firmware initialization required for scanning sensors
*  in a specific widget using the CSD sensing method. This function requires using
*  the CapSense_CSDScan() function to start scanning.
*
* \details
*  This function initializes the specific widget common parameters to perform
*  the CSD scanning. The initialization includes setting up a Modulator and
*  Sense clock frequency and scanning resolution.
*
*  This function does not connect any specific sensors to the scanning hardware,
*  neither does it start a scanning process. The CapSense_CSDScan()
*  API must be called after initializing the widget to start scanning.
*
*  This function is called when no scanning is in progress. I.e.
*  CapSense_IsBusy() returns a non-busy status.
*
*  This function is called by the CapSense_SetupWidget() API if the
*  given widget uses the CSD sensing method.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases (for faster execution time or pipeline scanning for example).
*
* \param  widgetId
*  Specifies the ID number of the widget to perform hardware and firmware
*  initialization required for scanning sensors in the specific widget.
*  A macro for the widget ID can be found in the CapSense Configuration header
*  file defined as CapSense_<WidgetName>_WDGT_ID.
*
*******************************************************************************/
void CapSense_CSDSetupWidget(uint32 widgetId)
{

    CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                                                    CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

    /* Save widget Id to have assess to it after scanning  */
    CapSense_widgetIndex = (uint8)widgetId;

    CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_CSD_E);

    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
        /* Reset the frequency scan channel if enabled */
        CapSense_scanFreqIndex = CapSense_FREQ_CHANNEL_0;
    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

    /* Disconnect previous electrode if it has been connected */
    CapSense_SsCSDElectrodeCheck();

    /* Initialize IO Sns electrode structure pointer to current widget */
    CapSense_curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                    CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2SnsFlash;

    /* Update status register in Data structure */
    CapSense_dsRam.status &= ~CapSense_STATUS_WDGT0_MASK;
    CapSense_dsRam.status |= CapSense_widgetIndex;

    /* Set up scanning resolution (Number ofconversion for CSDv2) */
    CapSense_SsCSDCalculateScanDuration(ptrWdgt);

    #if ((CapSense_DISABLE == CapSense_CSD_COMMON_SNS_CLK_EN) ||\
         (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE))
        CapSense_SsCSDConfigClock();
    #endif /* ((CapSense_DISABLE == CapSense_CSD_COMMON_SNS_CLK_EN) ||\
               (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE)) */
}


/*******************************************************************************
* Function Name: CapSense_CSDSetupWidgetExt
****************************************************************************//**
*
* \brief
*  Performs extended initialization for the CSD widget and also performs
*  initialization required for a specific sensor in the widget. This function
*  requires using the CapSense_CSDScanExt() function to initiate a scan.
*
* \details
*  This function does the same as CapSense_CSDSetupWidget() and also
*  does the following tasks:
*    1. Connects the first sensor of the widget.
*    2. Configures the IDAC value.
*    3. Initializes an interrupt callback function to initialize a scan of the
*  next sensors in a widget.
*
*  Once this function is called to initialize a widget and a sensor, the
*  CapSense_CSDScanExt() function is called to scan the sensor.
*
*  This function is called when no scanning is in progress. I.e.
*  CapSense_IsBusy() returns a non-busy status.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases (for faster execution time or pipeline scanning for example).
*
* \param widgetId
*  Specifies the ID number of the widget to perform hardware and firmware
*  initialization required for scanning the specific sensor in the specific
*  widget.
*  A macro for the widget ID can be found in the CapSense Configuration header
*  file defined as CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget to perform hardware
*  and firmware initialization required for scanning a specific sensor in a
*  specific widget.
*  A macro for the sensor ID within a specified widget can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID
*
*******************************************************************************/
void CapSense_CSDSetupWidgetExt(uint32 widgetId, uint32 sensorId)
{
    CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                                                    CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

    /* Save widget and sensor Ids to have access to it after scanning  */
    CapSense_sensorIndex = (uint8)sensorId;

    /* Update global pointer to CapSense_RAM_SNS_STRUCT to current sensor  */
    CapSense_curRamSnsPtr = (CapSense_RAM_SNS_STRUCT *)
                                              CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsRam
                                              + CapSense_sensorIndex;

    CapSense_CSDSetupWidget(widgetId);

     /* Setup Idac Value */
    CapSense_SsCSDSetUpIdacs(ptrWdgt);

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        /* initialize access pointers for current pointer to widget configuration in Flash */
        CapSense_curFlashWdgtPtr = &CapSense_dsFlash.wdgtArray[widgetId];
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        CapSense_SsCSDConnectSensorExt(widgetId, sensorId);
    #else
        /* Enable sensor */
        CapSense_curSnsIOPtr += CapSense_sensorIndex;
        CapSense_CSDConnectSns(CapSense_curSnsIOPtr);
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        /* Save sns pointer */
        CapSense_curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)
                                           CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2SnsFlash +
                                           CapSense_sensorIndex;
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

    CapSense_eleCsdDisconnectFlag = CapSense_DISCONNECT_IO_FLAG;

    /* Setup ISR handler to single-sensor scan function */
    CapSense_ISR_StartEx(&CapSense_CSDPostSingleScan);
}

/*******************************************************************************
* Function Name: CapSense_SsCSDStartSample
****************************************************************************//**
*
* \brief
*   Starts the CSD conversion.
*
* \details
*   This function assumes that the CSD block is already set up using
*   the CapSense_CSDSetupWidget API and the sensor port-pin is connected to the CSD
*   block using CapSense_CSDConnectSns.
*   For Third-generation HW block the function performs the following tasks:
*   1. Disconnects the Vref buffer from AMUX;
*   2. Precharges Cmod;
*   3. Makes the PreSettling delay to have a stable Vref voltage;
*   4. Sets the resolution to the Counter register to start scanning.
*
*******************************************************************************/
void CapSense_SsCSDStartSample(void)
{
    #if (CapSense_ENABLE != CapSense_CSDV2)
        uint8 interruptState;
    #endif

    #ifdef CapSense_START_SAMPLE_CALLBACK
        CapSense_StartSampleCallback(CapSense_widgetIndex, CapSense_sensorIndex);
    #endif /* CapSense_START_SAMPLE_CALLBACK */

    #if (CapSense_ENABLE == CapSense_CSDV2)
        /* Fourth-generation HW block section */

        /* Disable CSD interrupt to prevent it during coarse initialization */
        CyIntDisable(CapSense_ISR_NUMBER);

        /* Enable power to sub-blocks */
        CY_SET_REG32(CapSense_HSCMP_PTR, CapSense_HSCMP_INIT_MASK);

        #if (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN)
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd |
                                                      CapSense_CONFIG_SENSE_EN_MASK |
                                                      CapSense_CONFIG_ENABLE_MASK);
            #if(CapSense_CSD_ANALOG_STARTUP_DELAY_US > 0uL)
                CyDelayUs(CapSense_CSD_ANALOG_STARTUP_DELAY_US);
            #endif /* (CapSense_CSD_ANALOG_STARTUP_DELAY_US > 0uL) */
        #endif /* (CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) */

        /* Precharging Cmod and Csh */
        CapSense_SsCSDCmodPrecharge();

        /* Trigger Scan */
        CapSense_SsCSDTriggerScan();

    #else
        /* Third-generation HW block section */
        interruptState = CyEnterCriticalSection();

        /* Enable CSD block. Disconnect Vref Buffer from AMUX */
        #if ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
             (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd | CapSense_CMOD_PRECHARGE_CONFIG_CSD_EN);
        #else
            CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd | CapSense_CTANK_PRECHARGE_CONFIG_CSD_EN);
        #endif /* ((CapSense_CSH_PRECHARGE_IO_BUF == CapSense_CSD_CSH_PRECHARGE_SRC) &&\
                   (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

        /* Restart the clocks. Scan one cycle to reset the flip-flop for Direct clock mode */
        CapSense_SsCSDClockRestart();

        /* Precharging Cmod to Vref */
        CapSense_SsCSDCmodPrecharge();

        #if (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE)
            /* Set PreSettling delay to skip IDAC glitch before scanning */
            CyDelayCycles(CapSense_GLITCH_ELIMINATION_CYCLES);
        #endif /* (CapSense_CSD_SNS_CLK_SOURCE != CapSense_CLK_SOURCE_DIRECT) */

        /* Trigger Scan */
        CapSense_SsCSDTriggerScan();

        CyExitCriticalSection(interruptState);
    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


/*******************************************************************************
* Function Name: CapSense_CSDScanExt
****************************************************************************//**
*
* \brief
*  Starts the CSD conversion on the preconfigured sensor. This function requires
*  using the CapSense_CSDSetupWidgetExt() function to set up the a
*  widget.
*
* \details
*  This function performs single scanning of one sensor in the widget configured
*  by the CapSense_CSDSetupWidgetExt() function. It does the following
*  tasks:
*    1. Sets the busy flag in the CapSense_dsRam structure.
*    2. Performs the clock-phase alignment of the sense and modulator clocks.
*    3. Performs the Cmod pre-charging.
*    4. Starts single scanning.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases (for faster execution time or pipeline scanning for example).
*  This function is called when no scanning is in progress. I.e.
*  CapSense_IsBusy() returns a non-busy status.
*
*  The sensor must be preconfigured by using the
*  CapSense_CSDSetupWidgetExt() API prior to calling this function.
*  The sensor remains ready for a next scan if a previous scan was triggered
*  by using the CapSense_CSDScanExt() function. In this case, calling
*  CapSense_CSDSetupWidgetExt() is not required every time before the
*  CapSense_CSDScanExt() function. If a previous scan was triggered in
*  any other way - CapSense_Scan(), CapSense_ScanAllWidgets() or
*  CapSense_RunTuner() - (see the CapSense_RunTuner() function
*  description for more details), the sensor must be preconfigured again by
*  using the CapSense_CSDSetupWidgetExt() API prior to calling the
*  CapSense_CSDScanExt() function.
*
*  If disconnection of the sensors is required after calling
*  CapSense_CSDScanExt(), the CapSense_CSDDisconnectSns()
*  function can be used.
*
*******************************************************************************/
void CapSense_CSDScanExt(void)
{
    /* Set Start of sensor scan flag */
    CapSense_dsRam.status |= (CapSense_SW_STS_BUSY | CapSense_WDGT_SW_STS_BUSY);

    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
        /* Reset the frequency scan channel if enabled */
        CapSense_scanFreqIndex = CapSense_FREQ_CHANNEL_0;
    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

    #if (CapSense_ENABLE != CapSense_BLOCK_OFF_AFTER_SCAN_EN) &&\
        (CapSense_ENABLE == CapSense_CSDV2)
        #if(CapSense_CSD_ANALOG_STARTUP_DELAY_US > 0uL)
            CyDelayUs(CapSense_CSD_ANALOG_STARTUP_DELAY_US);
        #endif /* (CapSense_CSD_ANALOG_STARTUP_DELAY_US > 0uL) */
    #endif /* (CapSense_ENABLE != CapSense_BLOCK_OFF_AFTER_SCAN_EN) &&\
              (CapSense_ENABLE == CapSense_CSDV2)*/

    CapSense_SsCSDStartSample();
}


/*******************************************************************************
* Function Name: CapSense_CSDScan
****************************************************************************//**
*
* \brief
*  This function initiates a scan for the sensors of the widget initialized by the
*  CapSense_CSDSetupWidget() function.
*
* \details
*  This function performs scanning of all the sensors in the widget configured by
*  the CapSense_CSDSetupWidget() function. It does the following tasks:
*    1. Connects the first sensor of the widget.
*    2. Configures the IDAC value.
*    3. Initializes the interrupt callback function to initialize a scan of
*       the next sensors in a widget.
*    4. Starts scanning for the first sensor in the widget.
*
*  This function is called by the CapSense_Scan() API if the given
*  widget uses the CSD sensing method.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases (for faster execution time or pipeline scanning for example).
*
*  This function is called when no scanning is in progress. I.e.
*  CapSense_IsBusy() returns a non-busy status. The widget must be
*  preconfigured by the CapSense_CSDSetupWidget() function if any other
*  widget was previously scanned or any other type of the scan functions was used.
*
*******************************************************************************/
void CapSense_CSDScan(void)
{
    CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                                                    CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2WdgtRam;

     /*
      * Update CapSense_sensorIndex with the first sensor in
      * widget to use it in ISR handler to configure the next sensor
      */
    CapSense_sensorIndex = 0u;

    /* Update global pointer to CapSense_RAM_SNS_STRUCT to current sensor  */
    CapSense_curRamSnsPtr = (CapSense_RAM_SNS_STRUCT *)
                                              CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2SnsRam
                                              + CapSense_sensorIndex;

    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
        /* Reset the frequency scan channel if enabled */
        CapSense_scanFreqIndex = CapSense_FREQ_CHANNEL_0;
    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

    /* Setup Idac Value */
   CapSense_SsCSDSetUpIdacs(ptrWdgt);

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        /* Check ganged sns flag  */
        if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].staticConfig &
            CapSense_GANGED_SNS_MASK))
        {
            /* Setup ISR handler to multiple-sensor scan function with ganged sensors */
            CapSense_ISR_StartEx(&CapSense_CSDPostMultiScanGanged);
        }
        else
        {
            /* Set up ISR handler to multiple-sensor scan function without ganged sensors */
            CapSense_ISR_StartEx(&CapSense_CSDPostMultiScan);
        }

        CapSense_SsCSDConnectSensorExt((uint32)CapSense_widgetIndex, (uint32)CapSense_sensorIndex);
    #else
        /* initialize ptr to sensor IO structure to appropriate address */
        CapSense_curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                        CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2SnsFlash
                                        + CapSense_sensorIndex;

        /* Enable sensor */
        CapSense_CSDConnectSns(CapSense_curSnsIOPtr);

        /* Set-up ISR handler to multiple-sensor scan function without ganged sensors */
        CapSense_ISR_StartEx(&CapSense_CSDPostMultiScan);
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */

    /* Start scanning */
    CapSense_CSDScanExt();
}


/*******************************************************************************
* Function Name: CapSense_SsCSDConnectSensorExt
****************************************************************************//**
*
* \brief
*  Connects a ganged sensor port-pin to the sensing HW block via the AMUX bus.
*
* \details
*   The function gets the IO configuration registers addresses, their shifts and
*   masks from the FLASH_IO_STRUCT object. Basing on this data, it updates the HSIOM and
*   PC registers.
*
* \param widgetId
*  Specifies ID of the widget.
*
* \param sensorId
*  Specifies ID of the sensor in the widget.
*
*******************************************************************************/
void CapSense_SsCSDConnectSensorExt(uint32 widgetId, uint32 sensorId)
{
    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        uint32 tempVal;
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

    /* initialize ptr to sensor IO structure to appropriate address */
    CapSense_curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                                      CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash
                                                      + sensorId;

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        /* Check ganged sns flag  */
        if (CapSense_GANGED_SNS_MASK ==
           (CapSense_dsFlash.wdgtArray[widgetId].staticConfig &
            CapSense_GANGED_SNS_MASK))
        {
            /* Get sns pointer */
            CapSense_curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)
                                               CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash +
                                               sensorId;

            /* Get number of ganged pins */
            tempVal = CapSense_curFlashSnsPtr->numPins;

            /* Get IO pointer  */
            CapSense_curSnsIOPtr = &CapSense_ioList[CapSense_curFlashSnsPtr->firstPinId];

            /* Connect all ganged sensors  */
            do
            {
                CapSense_CSDConnectSns(CapSense_curSnsIOPtr);
                CapSense_curSnsIOPtr++;
                tempVal--;
            } while (0u != tempVal);
        }
        else
        {
            /* Connect sensor */
            CapSense_CSDConnectSns(CapSense_curSnsIOPtr);
        }
    #else
        /* Connect sensor */
        CapSense_CSDConnectSns(CapSense_curSnsIOPtr);
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
}


/*******************************************************************************
* Function Name: CapSense_SsCSDDisconnectSnsExt
****************************************************************************//**
*
* \brief
*  Disconnects a ganged sensor port-pin from the sensing HW block and AMUX bus.
*  Sets the default state of the un-scanned sensor.
*
* \details
*   The function gets the IO configuration registers addresses, their shifts and
*   masks from the FLASH_IO_STRUCT object. Basing on this data and Inactive sensor
*   connection parameter, it updates the HSIOM, PC and DR registers.
*   The HSIOM register is updated only when the Inactive sensor connection parameter
*   is set to Shield.
*
* \param widgetId
*  Specifies ID of the widget.
*
* \param sensorId
*  Specifies ID of the sensor in the widget.
*
*******************************************************************************/
void CapSense_SsCSDDisconnectSnsExt(uint32 widgetId, uint32 sensorId)
{
    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        uint32 tempVal;
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

    /* initialize ptr to sensor IO structure to appropriate address        */
    CapSense_curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                                      CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash
                                                      + sensorId;

    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
        /* Check ganged sns flag  */
        if ((CapSense_dsFlash.wdgtArray[widgetId].staticConfig &
            CapSense_GANGED_SNS_MASK) == CapSense_GANGED_SNS_MASK)
        {
            /* Get sns pointer */
            CapSense_curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)
                                               CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash +
                                               sensorId;

            /* Get number of ganged pins */
            tempVal = CapSense_curFlashSnsPtr->numPins;

            /* Get IO pointer  */
            CapSense_curSnsIOPtr = &CapSense_ioList[CapSense_curFlashSnsPtr->firstPinId];

            /* Disconnect all ganged sensors */
            do
            {
                CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
                CapSense_curSnsIOPtr++;
                tempVal--;
            } while (0u != tempVal);
        }
        else
        {
            /* Disconnect ganged sensor */
            CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
        }
    #else
        /* Disconnect ganged sensor */
        CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
}


/*******************************************************************************
* Function Name: CapSense_CSDConnectSns
****************************************************************************//**
*
* \brief
*  Connects a port pin used by the sensor to the AMUX bus of the sensing HW block.
*
* \details
*  Connects a port pin used by the sensor to the AMUX bus of the sensing HW block
*  while a sensor is being scanned. The function ignores the fact if
*  the sensor is a ganged sensor and connects only a specified pin.
*
*  Scanning should be completed before calling this API.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases. Functions that perform a setup and scan of a sensor/widget,
*  automatically set the required pin states and perform the sensor connection.
*  They do not take into account changes in the design made by the
*  CapSense_CSDConnectSns() function.
*
* \param  snsAddrPtr
*  Specifies the pointer to the FLASH_IO_STRUCT object belonging to a sensor
*  which to be connected to the sensing HW block.
*
*******************************************************************************/
void CapSense_CSDConnectSns(CapSense_FLASH_IO_STRUCT const *snsAddrPtr)
{
    uint32 newRegisterValue;
    uint8  interruptState;
    uint32 pinModeShift;
    uint32 pinHSIOMShift;

    /* Get offsets for sensor */
    pinModeShift = (uint32)snsAddrPtr->shift;
    pinHSIOMShift = (uint32)snsAddrPtr->hsiomShift;

    interruptState = CyEnterCriticalSection();

    /* Use temporary variable to update registers without multiple writings to them */
    newRegisterValue = CY_GET_REG32(snsAddrPtr->hsiomPtr);
    newRegisterValue &= ~(CapSense_HSIOM_SEL_MASK << pinHSIOMShift);
    newRegisterValue |= (uint32)((uint32)CapSense_HSIOM_SEL_CSD_SENSE << pinHSIOMShift);

    /* Update port configuration register (drive mode) for sensor */
    CY_SET_REG32(snsAddrPtr->pcPtr, CY_GET_REG32(snsAddrPtr->pcPtr) & (uint32)~((uint32)CapSense_GPIO_PC_MASK << pinModeShift));

    /* Update HSIOM register for sensor */
    CY_SET_REG32(snsAddrPtr->hsiomPtr, newRegisterValue);

    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: CapSense_CSDDisconnectSns
****************************************************************************//**
*
* \brief
*  Disconnects a sensor port pin from the sensing HW block and the AMUX bus. Sets
*  the default state of the un-scanned sensor.
*
* \details
*  This function works identically to CapSense_CSDConnectSns() except
*  it disconnects the specified port-pin used by the sensor.
*
*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the user's specific
*  use cases. Functions that perform a setup and scan of sensor/widget
*  automatically set the required pin states and perform the sensor connection.
*  They ignore changes in the design made by the
*  CapSense_CSDDisconnectSns() function.
*
* \param  snsAddrPtr
*  Specifies the pointer to the FLASH_IO_STRUCT object belonging to a sensor
*  which should be disconnected from the sensing HW block.
*
*******************************************************************************/
void CapSense_CSDDisconnectSns(CapSense_FLASH_IO_STRUCT const *snsAddrPtr)
{
    uint32 pinHSIOMShift;
    uint32 newRegisterValue;
    uint32 pinModeShift;

    #if ((CapSense_DISABLE == CapSense_CSD_SHIELD_EN) || \
        (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION))
        uint8  interruptState;
    #endif /* ((CapSense_DISABLE == CapSense_CSD_SHIELD_EN) && \
                (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION))) */

    pinHSIOMShift = (uint32)snsAddrPtr->hsiomShift;
    pinModeShift = (uint32)snsAddrPtr->shift;

    #if ((CapSense_DISABLE == CapSense_CSD_SHIELD_EN) ||  \
        (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION))
        /* Update HSIOM register to disconnect sensor */
        CY_SET_REG32(snsAddrPtr->hsiomPtr, CY_GET_REG32(snsAddrPtr->hsiomPtr) & ~(CapSense_HSIOM_SEL_MASK << pinHSIOMShift));

        interruptState = CyEnterCriticalSection();

        /* Update port configuration register (drive mode) for sensor */
        newRegisterValue = CY_GET_REG32(snsAddrPtr->pcPtr);
        newRegisterValue &= ~(CapSense_GPIO_PC_MASK << pinModeShift);
        newRegisterValue |= CapSense_CSD_INACTIVE_SNS_CONNECTION << pinModeShift;
        CY_SET_REG32(snsAddrPtr->pcPtr, newRegisterValue);

        CyExitCriticalSection(interruptState);

        /* Set logic 0 to port data register */
        CY_SET_REG32(snsAddrPtr->drPtr, CY_GET_REG32(snsAddrPtr->drPtr) & (uint32)~(uint32)((uint32)1u << snsAddrPtr->drShift));
    #else
        /* Update port configuration register (drive mode to HiZ Analog) for sensor */
        CY_SET_REG32(snsAddrPtr->pcPtr, CY_GET_REG32(snsAddrPtr->pcPtr) & (uint32)~((uint32)CapSense_GPIO_PC_MASK << pinModeShift));

        /* Connect to Shield */
        newRegisterValue = CY_GET_REG32(snsAddrPtr->hsiomPtr) & ~(CapSense_HSIOM_SEL_MASK << pinHSIOMShift);
        newRegisterValue |= (CapSense_HSIOM_SEL_CSD_SHIELD << pinHSIOMShift);
        CY_SET_REG32(snsAddrPtr->hsiomPtr, newRegisterValue);
    #endif /* ((CapSense_DISABLE == CapSense_CSD_SHIELD_EN) && \
                (CapSense_SNS_CONNECTION_SHIELD != CapSense_CSD_INACTIVE_SNS_CONNECTION)) */
}

#if ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
     (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDSetSingleIdacMode
        ****************************************************************************//**
        *
        * \brief
        *  This function set single IDAC mode.
        *
        * \details
        *   For Third-generation HW block: The function enables Modulator IDAC only in variable mode.
        *   For Fourth-generation HW block: The function enables IDACA LEG1 in CSD mode, IDACA LEG2 in GP mode.
        *
        *******************************************************************************/
        static void CapSense_SsCSDSetSingleIdacMode(void)
        {
            uint32 newRegValue;

            #if (CapSense_ENABLE == CapSense_CSDV2)
                /* Enable IDACA LEG1 in CSD mode, IDACA LEG2 in GP mode */
                newRegValue = CY_GET_REG32(CapSense_IDAC_MOD_PTR) &
                              CapSense_IDAC_MOD_VAL_MASK;
                newRegValue |= CapSense_IDAC_MOD_CALIBRATION_CFG;
                CY_SET_REG32(CapSense_IDAC_MOD_PTR, newRegValue);
                #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                    (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)
                    newRegValue = CY_GET_REG32(CapSense_IDAC_COMP_PTR) & CapSense_IDAC_COMP_VAL_MASK;
                    newRegValue |= CapSense_IDAC_COMP_CALIBRATION_CFG;
                    CY_SET_REG32(CapSense_IDAC_COMP_PTR, newRegValue);
                #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                          (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN */
            #else
                /* Enable Modulator IDAC only in variable mode */
                newRegValue = CY_GET_REG32(CapSense_IDAC_PTR) &
                              ~(CapSense_IDAC_MOD_MODE_MASK | CapSense_IDAC_COMP_MODE_MASK);
                newRegValue |= CapSense_IDAC_MOD_MODE_VARIABLE;
                CY_SET_REG32(CapSense_IDAC_PTR, newRegValue);
            #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
        }
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
#endif /* ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
           (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)) */

#if ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) || \
     (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN))
    #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDRestoreIdacMode
        ****************************************************************************//**
        *
        * \brief
        *  This function restores IDAC configuration.
        *
        * \details
        *   The function restores Compensation IDAC if it is enabled.
        *
        *******************************************************************************/
        static void CapSense_SsCSDRestoreIdacMode(void)
        {
            /* Restore IDAC configuration */
            #if (CapSense_ENABLE == CapSense_CSDV2)
                uint32 newRegValue;

                newRegValue = CY_GET_REG32(CapSense_SW_REFGEN_SEL_PTR);
                newRegValue |= CapSense_DEFAULT_SW_REFGEN_SEL;
                CY_SET_REG32(CapSense_SW_REFGEN_SEL_PTR, newRegValue);
                CY_SET_REG32(CapSense_IDAC_MOD_PTR, CapSense_IDAC_MOD_DEFAULT_CFG);
                #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                    (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)
                CY_SET_REG32(CapSense_IDAC_COMP_PTR, CapSense_IDAC_COMP_DEFAULT_CFG);
                #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                          (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN */
            #else
                #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
                    CY_SET_REG32(CapSense_IDAC_PTR, CapSense_DEFAULT_CSD_IDAC_CONFIG);
                #else
                    CY_SET_REG32(CapSense_IDAC_PTR, CY_GET_REG32(CapSense_IDAC_PTR) &
                                                                        ~(CapSense_IDAC_MOD_MODE_MASK | CapSense_IDAC_MOD_MASK));
                    CY_SET_REG32(CapSense_IDAC_PTR, CY_GET_REG32(CapSense_IDAC_PTR) | CapSense_DEFAULT_CSD_IDAC_CONFIG);
                #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
            #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
        }
    #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

    /*******************************************************************************
    * Function Name: CapSense_SsCSDCalibrateCheck
    ****************************************************************************//**
    *
    * \brief
    *  This internal function checks if the calibration is performed successfully.
    *
    * \details
    *   The function verifies calibration error based on maximum raw count level.
    *
    * \param rawLevel The raw data level which should be reached during the
    *   calibration procedure.
    * \param rawData  Calibrated raw counts.
    *
    * \return Returns the status of the operation:
    *   - Zero     - All the sensors in the widget are calibrated successfully.
    *   - Non-Zero - Calibration failed for any sensor in the widget.
    *
    *******************************************************************************/
    static cystatus CapSense_SsCSDCalibrateCheck(CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt, uint32 rawLevel, uint32 rawData)
    {
        cystatus calibrateStatus;
        uint32 rawCountsError;
        uint32 minRawLevel = 0u;
        uint32 maxRawLevel = (0x00000001Lu << ptrWdgt->resolution) - 1u;

        /* Calculate the error in rawcounts for given resolution (round to floor) */
        rawCountsError = ((maxRawLevel * CapSense_IDAC_MOD_MAX_CALIB_ERROR) /
                          CapSense_PERCENTAGE_100) + 1u;

        /* Get minimum and maximum range values */
        if ((rawLevel + rawCountsError) <= maxRawLevel)
        {
            maxRawLevel = rawLevel + rawCountsError;
        }
        if (rawLevel >= rawCountsError)
        {
            minRawLevel = rawLevel - rawCountsError;
        }

        /* Check if calibration is in valid range */
        if ((rawData < minRawLevel) || (rawData > maxRawLevel))
        {
            calibrateStatus = CYRET_BAD_DATA;
        }
        else
        {
            calibrateStatus = CYRET_SUCCESS;
        }

        return (calibrateStatus);
    }

    /*******************************************************************************
    * Function Name: CapSense_SsCSDCalibrateOneSensor
    ****************************************************************************//**
    *
    * \brief
    *  Computes the Modulator IDAC value for one sensor that provides raw signal at
    *  the specified level.
    *
    * \details
    *   The function performs the following tasks:
    *   1. initializes The Compensation IDAC to zero
    *   2. Enables the calibrated sensor
    *   3. Performs eight iteration steps to get the target = rawLevel
    *      setting and resetting the IDAC value bits beginning from the
    *      MSB bit to an MSB bit.
    *   4. Disables the calibrated sensor
    *   5. Checks if the IDAC is in the correct range
    *   6. Updates the maximum IDAC value in the widget
    *   7. Stores the temporary ModIDAC8 value (in the single-IDAC mode).
    *
    *  The CSD block should be initialized with the CapSense_SsCSDInitialize()
    *  API before using this function.
    *
    * \param  rawLevel
    *   The raw data level which should be reached during the calibration procedure.
    * \param  ptrIdac  The pointer to IDAC.
    * \param  iDAC8Max The pointer to the maximum IDAC value in the widget.
    *
    * \return Returns the status of the operation:
    *  - Zero     - All the sensors in the widget are calibrated successfully.
    *  - Non-Zero - Calibration failed for any sensor in the widget.
    *
    *******************************************************************************/
    static cystatus CapSense_SsCSDCalibrateOneSensor(uint32 rawLevel,
                                                           uint8 *ptrIdac,
                                                           uint8 *iDAC8Max)
    {
        cystatus calibrateStatus;
        uint32 calMask = ((uint32)CapSense_CAL_MIDDLE_BIT << 1uL);
        uint32 watchdogCounter;

        uint16 rawData0 = 0u;
        #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
            uint16 rawData1 = 0u;
            uint16 rawData2 = 0u;
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

        /* Declare and initialize ptr to widget and sensor structures to appropriate address */
        CapSense_RAM_WD_BASE_STRUCT *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                                                        CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2WdgtRam;

        ptrIdac[0u] = 0u;
        #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
            ptrIdac[CapSense_FREQ_CHANNEL_1] = 0u;
            ptrIdac[CapSense_FREQ_CHANNEL_2] = 0u;
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

        /* Setup the calibrated sensor of the widget (Set IDAC = calMask and connect the sensor) */
        CapSense_CSDSetupWidgetExt((uint32)CapSense_widgetIndex, (uint32)CapSense_sensorIndex);

        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            /* Set single IDAC mode */
            CapSense_SsCSDSetSingleIdacMode();
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

        /*
         * Perform the iteration steps to get target = rawLevel.
         * The number of iteration steps is defined by calMask and depend on IDAC resolution.
         */
        do
        {
            /* Decrease IDAC until rawData reaches rawLevel */
            if (rawData0 < rawLevel)
            {
                /* Reset bit for null channel (decrease IDAC -> will increase rawData) */
                ptrIdac[0u] &= (uint8)(~calMask);
            }
            #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                if (rawData1 < rawLevel)
                {
                    ptrIdac[CapSense_FREQ_CHANNEL_1] &= (uint8)(~calMask);
                }
                if (rawData2 < rawLevel)
                {
                    ptrIdac[CapSense_FREQ_CHANNEL_2] &= (uint8)(~calMask);
                }
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

            /*
            * Shift calMask to set or reset next bit.
            * Perform scan even if callibration mask is equal to zero.
            * It gives raw counts for the last changed IDAC.
            */
            calMask >>= 1u;

            /* Set bit for null channel */
            ptrIdac[0u] |= (uint8)calMask;
            #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                ptrIdac[CapSense_FREQ_CHANNEL_1] |= (uint8)calMask;
                ptrIdac[CapSense_FREQ_CHANNEL_2] |= (uint8)calMask;
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

            /* Update IDAC */
            CapSense_SsCSDSetUpIdacs(ptrWdgt);

            /* Scan sensor */
            CapSense_CSDScanExt();

            /* Initialize Watchdog Counter with time interval which is enough to charge 100 nF capacitor */
            watchdogCounter = CapSense_CSD_CALIBR_WATCHDOG_CYCLES_NUM;
            while (((CapSense_dsRam.status & CapSense_SW_STS_BUSY) != 0u) &&
                   (0u != watchdogCounter))
            {
                /* Wait until scan complete and decrement Watchdog Counter to prevent unending loop */
                watchdogCounter--;
            }

            /* Read rawData */
            rawData0 = CapSense_curRamSnsPtr->raw[0u];
            #if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                rawData1 = CapSense_curRamSnsPtr->raw[CapSense_FREQ_CHANNEL_1];
                rawData2 = CapSense_curRamSnsPtr->raw[CapSense_FREQ_CHANNEL_2];
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
        }
        while (calMask > 0u);

        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            CapSense_SsCSDDisconnectSnsExt((uint32)CapSense_widgetIndex, (uint32)CapSense_sensorIndex);
        #else
            /* initialize pointer to sensor IO structure */
            CapSense_curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                            CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].ptr2SnsFlash
                                            + CapSense_sensorIndex;

            /* Disable sensor */
            CapSense_CSDDisconnectSns(CapSense_curSnsIOPtr);
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

        /* Set flag about disconnecting the electrode */
        CapSense_eleCsdDisconnectFlag = 0u;

        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            /* Restore IDAC configuration to default state */
            CapSense_SsCSDRestoreIdacMode();
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

        /* Check if IDAC is in correct range */
        calibrateStatus = CapSense_SsCSDCalibrateCheck(ptrWdgt, rawLevel, (uint32)rawData0);
        #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
            calibrateStatus |= CapSense_SsCSDCalibrateCheck(ptrWdgt, rawLevel, (uint32)rawData1);
            calibrateStatus |= CapSense_SsCSDCalibrateCheck(ptrWdgt, rawLevel, (uint32)rawData2);
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

        /*
         * Update maximum IDAC value in widget. iDAC8Max variable will contain
         * maximum IDAC value after all the sensors in a widget have been scanned.
         */
        if (*iDAC8Max < ptrIdac[0u])
        {
            *iDAC8Max = ptrIdac[0u];
            #if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
                /* Stores IDAC and raw count that corresponds to a sensor with maximum Cp within a widget */
                CapSense_calibratedIdac = *iDAC8Max;
                CapSense_calibratedRawcount = rawData0;
            #endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */
        }
        #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
            if (iDAC8Max[CapSense_FREQ_CHANNEL_1] < ptrIdac[CapSense_FREQ_CHANNEL_1])
            {
                iDAC8Max[CapSense_FREQ_CHANNEL_1] = ptrIdac[CapSense_FREQ_CHANNEL_1];
            }
            if (iDAC8Max[CapSense_FREQ_CHANNEL_2] < ptrIdac[CapSense_FREQ_CHANNEL_2])
            {
                iDAC8Max[CapSense_FREQ_CHANNEL_2] = ptrIdac[CapSense_FREQ_CHANNEL_2];
            }
        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            /*
             * Store temporary ModIDAC8 value (in single-IDAC mode) in idacComp variable.
             * We need this value to calculate idacMod value the in the dual-IDAC mode.
             */
            CapSense_curRamSnsPtr->idacComp[0u] = ptrIdac[0u];
            #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] = ptrIdac[CapSense_FREQ_CHANNEL_1];
                CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] = ptrIdac[CapSense_FREQ_CHANNEL_2];
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

        return (calibrateStatus);
    }


    /*******************************************************************************
    * Function Name: CapSense_CSDCalibrateWidget
    ****************************************************************************//**
    *
    * \brief
    *  Executes the IDAC calibration for all the sensors in the widget specified in
    *  the input.
    *
    * \details
    *  Performs a successive approximation search algorithm to find appropriate
    *  IDAC values for sensors in the specified widget that provides the raw
    *  count to the level specified by the target parameter.
    *
    *  Calibration is always performed in the single IDAC mode and if the dual
    *  IDAC mode (Enable compensation IDAC is enabled) is configured, the IDAC
    *  values are re-calculated to match the raw count target. If a widget
    *  consists of two or more elements (buttons, slider segments, etc.), then
    *  calibration is performed by the element with the highest sensor
    *  capacitance.
    *
    *  Calibration fails if the achieved raw count is outside of the +/-10%
    *  range of the target.
    *
    *  This function is available when the CSD Enable IDAC auto-calibration
    *  parameter is enabled or the  SmartSense auto-tuning mode is configured.
    *
    * \param widgetId
    *  Specifies the ID number of the CSD widget to calibrate its raw count.
    *  A macro for the widget ID can be found in the
    *  CapSense Configuration header file defined as
    *  CapSense_<WidgetName>_WDGT_ID.
    *
    * \param target
    *  Specifies the calibration target in percentages of the maximum raw count.
    *
    * \return
    *  Returns the status of the specified widget calibration:
    *    - CYRET_SUCCESS - The operation is successfully completed.
    *    - CYRET_BAD_PARAM - The input parameter is invalid.
    *    - CYRET_BAD_DATA - The calibration failed and the Component may not operate as expected.
    *
    *******************************************************************************/
    cystatus CapSense_CSDCalibrateWidget(uint32 widgetId, uint32 target)
    {
        uint32 rawLevel;
        cystatus calibrateStatus = CYRET_SUCCESS;

        #if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
            #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                uint8 maxIdac = 1u;
                uint16 maxRaw = 1u;
            #endif /*(CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
        #endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

        /* Declare and initialize ptr to widget and sensor structures to appropriate address */
        CapSense_RAM_WD_BASE_STRUCT *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                                              CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

        #if (1u == CapSense_NUM_SCAN_FREQS)
            uint8 iDAC8Max[CapSense_NUM_SCAN_FREQS] = {1u};
        #else
            uint8 iDAC8Max[CapSense_NUM_SCAN_FREQS] = {1u, 1u, 1u};
        #endif /* (1u == CapSense_NUM_SCAN_FREQS) */

        uint8 *ptrIdac;

        /* Calibrate maximum level for given resolution */
        rawLevel = (0x00000001Lu << ptrWdgt->resolution) - 1u;

        /* Calculate percentage of maximum value */
        rawLevel = (rawLevel * target) / CapSense_PERCENTAGE_100;

        /* Update CapSense_widgetIndex */
        CapSense_widgetIndex = (uint8)widgetId;

        /* Setup ISR handler to single-sensor scan function */
        CapSense_ISR_StartEx(&CapSense_CSDPostSingleScan);

        /* Init pointer to Modulator IDAC */
        ptrIdac = &ptrWdgt->idacMod[0u];

        #if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
            CapSense_calibratedIdac = 1u;
            CapSense_calibratedRawcount = 1u;
        #endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

        /* Go through all the sensors (or columns of touchpad) of widget to calibrate them */
        for (CapSense_sensorIndex = 0u;
             CapSense_sensorIndex < CapSense_dsFlash.wdgtArray[widgetId].numCols;
             CapSense_sensorIndex++)
        {
            /* Calibrate current sensor in widget */
            calibrateStatus |= CapSense_SsCSDCalibrateOneSensor(rawLevel, ptrIdac, iDAC8Max);
        }

        /* Normalize idacMod and idacComp values */
        #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
            /* Calculate Modulator value: idacMod = (iDAC8Max + 1)/2 */
            ptrIdac[0u] = (iDAC8Max[0u] + 1u) >> 1u;
            #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                ptrIdac[CapSense_FREQ_CHANNEL_1] =  (iDAC8Max[CapSense_FREQ_CHANNEL_1] + 1u) >> 1u;
                ptrIdac[CapSense_FREQ_CHANNEL_2] = (iDAC8Max[CapSense_FREQ_CHANNEL_2] + 1u) >> 1u;
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

            /* Go through all columns of touchpad (or all the sensors in button) to normalize them */
            for (CapSense_sensorIndex = 0u;
                 CapSense_sensorIndex < CapSense_dsFlash.wdgtArray[widgetId].numCols;
                 CapSense_sensorIndex++)
            {
                CapSense_curRamSnsPtr = (CapSense_RAM_SNS_STRUCT *)
                         CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsRam + CapSense_sensorIndex;
                /* Calculate Compensation IDAC value: idacComp(i) = (iDAC8(i) - idacMod) * Target */
                if (CapSense_curRamSnsPtr->idacComp[0u] >= ptrIdac[0u])
                {
                    CapSense_curRamSnsPtr->idacComp[0u] -= ptrIdac[0u];
                    #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                         (CapSense_DISABLE == CapSense_CSDV2))
                        CapSense_curRamSnsPtr->idacComp[0u] =
                            (uint8)((((target * CapSense_curRamSnsPtr->idacComp[0u]) + CapSense_PERCENTAGE_100) - 1u) /
                            CapSense_PERCENTAGE_100);
                    #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                               (CapSense_DISABLE == CapSense_CSDV2)) */
                }
                else
                {
                    CapSense_curRamSnsPtr->idacComp[0u] = 0u;
                }
                #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                    /* Do same for first frequency channel */
                    if (CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] >= ptrIdac[CapSense_FREQ_CHANNEL_1])
                    {
                        CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] -= ptrIdac[CapSense_FREQ_CHANNEL_1];
                        #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                             (CapSense_DISABLE == CapSense_CSDV2))
                            CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] =
                                (uint8)((((target * CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1]) +
                                CapSense_PERCENTAGE_100) - 1u) / CapSense_PERCENTAGE_100);
                        #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                   (CapSense_DISABLE == CapSense_CSDV2)) */
                    }
                    else
                    {
                       CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] = 0u;
                    }
                    /* Do same for second frequency channel */
                    if (CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] >= ptrIdac[CapSense_FREQ_CHANNEL_2])
                    {
                        CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] -= ptrIdac[CapSense_FREQ_CHANNEL_2];
                        #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                             (CapSense_DISABLE == CapSense_CSDV2))
                            CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] =
                                (uint8)((((target * CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2]) +
                                CapSense_PERCENTAGE_100) - 1u) / CapSense_PERCENTAGE_100);
                        #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                   (CapSense_DISABLE == CapSense_CSDV2)) */
                    }
                    else
                    {
                        CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] = 0u;
                    }
                #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
            }
        #else
            /* Normalize widget for single-IDAC mode*/
            ptrIdac[0u] = iDAC8Max[0u];
            #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                ptrIdac[CapSense_FREQ_CHANNEL_1] = iDAC8Max[CapSense_FREQ_CHANNEL_1];
                ptrIdac[CapSense_FREQ_CHANNEL_2] = iDAC8Max[CapSense_FREQ_CHANNEL_2];
            #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
        #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */

        #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
            if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType) ||
                (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[widgetId].wdgtType))
            {
                /* Init pointer to Modulator IDAC for columns */
                ptrIdac = &ptrWdgt->rowIdacMod[0u];

                /* Reset the maximum value for columns */
                iDAC8Max[0u] = 1u;
                #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                    iDAC8Max[CapSense_FREQ_CHANNEL_1] = 1u;
                    iDAC8Max[CapSense_FREQ_CHANNEL_2] = 1u;
                #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

                #if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
                    maxIdac = CapSense_calibratedIdac;
                    maxRaw = CapSense_calibratedRawcount;
                    CapSense_calibratedIdac = 1u;
                    CapSense_calibratedRawcount = 1u;
                #endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

                /* Go through all columns of touchpad to calibrate them */
                for (CapSense_sensorIndex = CapSense_dsFlash.wdgtArray[widgetId].numCols;
                     CapSense_sensorIndex < (uint8)CapSense_dsFlash.wdgtArray[widgetId].totalNumSns;
                     CapSense_sensorIndex++)
                {
                    /* Calibrate current sensor in widget */
                    calibrateStatus |= CapSense_SsCSDCalibrateOneSensor(rawLevel, ptrIdac, iDAC8Max);
                }

                #if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
                    CapSense_calibratedIdacRow = CapSense_calibratedIdac;
                    CapSense_calibratedRawcountRow = CapSense_calibratedRawcount;
                    CapSense_calibratedIdac = maxIdac;
                    CapSense_calibratedRawcount = maxRaw;
                #endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

                /* Normalize idacMod and idacComp values */
                #if (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN)
                    /* Calculate Modulator value: idacMod = (iDAC8Max + 1)/2 */
                    ptrIdac[0u] = (iDAC8Max[0u] + 1u) >> 1u;
                    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                        ptrIdac[CapSense_FREQ_CHANNEL_1] =  (iDAC8Max[CapSense_FREQ_CHANNEL_1] + 1u) >> 1u;
                        ptrIdac[CapSense_FREQ_CHANNEL_2] = (iDAC8Max[CapSense_FREQ_CHANNEL_2] + 1u) >> 1u;
                    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

                    /* Go through all columns of touchpad to normalize them */
                    for (CapSense_sensorIndex = CapSense_dsFlash.wdgtArray[widgetId].numCols;
                         CapSense_sensorIndex < (uint8)CapSense_dsFlash.wdgtArray[widgetId].totalNumSns;
                         CapSense_sensorIndex++)
                    {
                        CapSense_curRamSnsPtr = (CapSense_RAM_SNS_STRUCT *)
                                 CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsRam + CapSense_sensorIndex;

                        /* Calculate Compensation IDAC value: idacComp(i) = (iDAC8(i) - idacMod) * Target */
                        if (CapSense_curRamSnsPtr->idacComp[0u] >= ptrIdac[0u])
                        {
                            CapSense_curRamSnsPtr->idacComp[0u] -= ptrIdac[0u];
                            #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                 (CapSense_DISABLE == CapSense_CSDV2))
                                CapSense_curRamSnsPtr->idacComp[0u] =
                                    (uint8)((((target * CapSense_curRamSnsPtr->idacComp[0u]) +
                                        CapSense_PERCENTAGE_100) - 1u) / CapSense_PERCENTAGE_100);
                            #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                       (CapSense_DISABLE == CapSense_CSDV2)) */
                        }
                        #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                            /* Do same for first frequency channel */
                            if (CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] >= ptrIdac[CapSense_FREQ_CHANNEL_1])
                            {
                                CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] -= ptrIdac[CapSense_FREQ_CHANNEL_1];
                                #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                     (CapSense_DISABLE == CapSense_CSDV2))
                                    CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1] =
                                        (uint8)((((target * CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_1]) +
                                        CapSense_PERCENTAGE_100) - 1u) / CapSense_PERCENTAGE_100);
                                #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                           (CapSense_DISABLE == CapSense_CSDV2)) */
                            }
                            /* Do same for second frequency channel */
                            if (CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] >= ptrIdac[CapSense_FREQ_CHANNEL_2])
                            {
                                CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] -= ptrIdac[CapSense_FREQ_CHANNEL_2];
                                #if ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                     (CapSense_DISABLE == CapSense_CSDV2))
                                    CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2] =
                                        (uint8)((((target * CapSense_curRamSnsPtr->idacComp[CapSense_FREQ_CHANNEL_2]) +
                                        CapSense_PERCENTAGE_100) - 1u) / CapSense_PERCENTAGE_100);
                                #endif /* ((CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN) ||\
                                           (CapSense_DISABLE == CapSense_CSDV2)) */
                            }
                        #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
                    }
                #else
                    /* Normalize widget for single-IDAC mode*/
                    ptrIdac[0u] = iDAC8Max[0u];
                    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
                        ptrIdac[CapSense_FREQ_CHANNEL_1] = iDAC8Max[CapSense_FREQ_CHANNEL_1];
                        ptrIdac[CapSense_FREQ_CHANNEL_2] = iDAC8Max[CapSense_FREQ_CHANNEL_2];
                    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */
                #endif /* (CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) */
            }
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

        CapSense_sensorIndex = 0u;

        return calibrateStatus;
    }
#endif /* ((CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)) || \
            (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)) */


#if (CapSense_DISABLE == CapSense_CSDV2)
    /*******************************************************************************
    * Function Name: CapSense_SsCSDClockRestart
    ****************************************************************************//**
    *
    * \brief
    *  This function restarts the clocks.
    *
    * \details
    *  The function performs following:
    *  1) Stops, sets dividers and starts ModClk clock;
    *  2) Stops and starts SnsClk clock;
    *  3) Scan one cycle to reset the flip-flop for Direct clock mode.
    *
    *******************************************************************************/
    CY_INLINE static void CapSense_SsCSDClockRestart(void)
    {
        /* Stop clocks, set dividers, and start clock to align clock phase */
        CapSense_SsSetModClkClockDivider((uint32)CapSense_dsRam.modCsdClk);

        #if (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK)
            /* Stop sense clock clock   */
            CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                         ((uint32)CapSense_SnsClk__DIV_ID <<
                         CapSense_SNSCLK_CMD_DIV_SHIFT)|
                         ((uint32)CapSense_SNSCLK_CMD_DISABLE_MASK));

            /* Check whether previous sense clock start command has finished. */
            while(0u != (CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) & CapSense_SNSCLK_CMD_ENABLE_MASK))
            {
                /* Wait until previous sense clock start command has finished. */
            }

            /* Start sense clock aligned to previously started modulator clock. */
            CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                         (uint32)(((uint32)CapSense_SnsClk__DIV_ID << CapSense_SNSCLK_CMD_DIV_SHIFT) |
                          ((uint32)CapSense_ModClk__PA_DIV_ID << CapSense_SNSCLK_CMD_PA_DIV_SHIFT) |
                          CapSense_SNSCLK_CMD_ENABLE_MASK));
        #else
            /* Clear bit to disable SnsClk clock. */
            CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                         CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) &
                         (uint32)(~(uint32)CapSense_SnsClk__ENABLE_MASK));

            /* Set bit to enable SnsClk clock. */
            CY_SET_REG32(CapSense_SNSCLK_CMD_PTR,
                         CY_GET_REG32(CapSense_SNSCLK_CMD_PTR) |
                        CapSense_SnsClk__ENABLE_MASK);
        #endif /* (CapSense_ENABLE == CapSense_IS_M0S8PERI_BLOCK) */

        #if (CapSense_CLK_SOURCE_DIRECT == CapSense_CSD_SNS_CLK_SOURCE)
            /* Scan one cycle to reset the flip-flop for Direct clock mode */
            CyIntDisable(CapSense_ISR_NUMBER);
            CY_SET_REG32(CapSense_COUNTER_PTR, CapSense_ONE_CYCLE);

            while(0u != (CY_GET_REG32(CapSense_COUNTER_PTR) & CapSense_RESOLUTION_16_BITS))
            {
                /* Wait until scanning is complete */
            }
            CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_SET_MASK);
            CyIntClearPending(CapSense_ISR_NUMBER);
        #endif /* (CapSense_CLK_SOURCE_DIRECT == CapSense_CSD_SNS_CLK_SOURCE) */
    }
#endif /* (CapSense_DISABLE == CapSense_CSDV2) */


/*******************************************************************************
* Function Name: CapSense_SsCSDCmodPrecharge
****************************************************************************//**
*
* \brief
*  This function initializes the Cmod charging to Vref.
*
* \details
*  For Third-generation HW block:
*  The function initializes the Cmod charging to Vref.
*  Then it waits the even when Cmod is completely charged
*  to Vref to have stable raw counts. Software Watchdog Counter is implemented to
*  prevent the project hanging.
*
*  For Fourth-generation HW block:
*  Coarse initialization for CMOD and Cch.
*  The coarse initialization is performed by HSCOMP.
*  The HSCOMP monitors the Cmod voltage via Cmod sense path
*  and charges the Cmod using HCAV switch via CSDBUS-A, AMUXBUS-A
*  and static connection of Cmod to AMUXBUS-A.
*
*******************************************************************************/
CY_INLINE static void CapSense_SsCSDCmodPrecharge(void)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)

        /* Fourth-generation HW block section */

        uint32 watchdogCounter;
        #if((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) &&\
            (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
            uint32 tmpRegValue;
        #endif

        CY_SET_REG32(CapSense_CSDCMP_PTR, CapSense_CSD_CSDCMP_INIT);
        CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR,  CapSense_CSD_SW_FW_MOD_SEL_INIT);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR, CapSense_CSD_SW_FW_TANK_SEL_INIT);
        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,  CapSense_CSD_SW_SHIELD_SEL_INIT);
        #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
            /*
             * Connect CMOD to (sense path) to HSCOMP: HMPM or HMPS or HMPT switches depend on Cmod connects to certain pad
             * Connect AMUXBUS-B to HSCMP positive input
             */
            CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_SW_HS_P_SEL_COARSE);

            #if(0u != CapSense_CSD_SHIELD_TANK_EN)
                /* Connect CTANK to AMUXBUS-A */
                tmpRegValue = CY_GET_REG32(CapSense_CSH_HSIOM_PTR);
                tmpRegValue &= (uint32)(~(uint32)CapSense_CSH_HSIOM_MASK);
                tmpRegValue |= (CapSense_HSIOM_SEL_AMUXA << CapSense_CSH_HSIOM_SHIFT);
                CY_SET_REG32(CapSense_CSH_HSIOM_PTR, tmpRegValue);
            #endif /* (0u != CapSense_CSD_SHIELD_TANK_EN) */
        #else
            /* Connect CMOD to (sense path) to HSCOMP: HMPM or HMPS or HMPT switches depend on Cmod connects to certain pad */
            CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_SW_HS_P_SEL_COARSE);
        #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */

        CY_SET_REG32(CapSense_SW_RES_PTR, CapSense_CSD_SW_RES_INIT);

        /* Clear all interrupt pending requests */
        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_ALL_MASK);

        CY_SET_REG32(CapSense_INTR_MASK_PTR, CapSense_CLEAR_MASK);

        /* Start SEQUENCER for coarse initialization for Cmod */
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_SEQ_MODE_MASK |
                                                         CapSense_SEQ_START_START_MASK);

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = CapSense_CSD_PRECHARGE_WATCHDOG_CYCLES_NUM;

        /* Wait for INTR.INIT goes high */
        while((0u == (CY_GET_REG32(CapSense_INTR_PTR) & CapSense_INTR_INIT_MASK)) && (0u != watchdogCounter))
        {
            watchdogCounter--;
        }

        if (0u == watchdogCounter)
        {
            /* Set sequencer to idle state if coarse initialization fails */
            CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_ABORT_MASK);
        }

    #else

        uint32 tmpRegValue;
        uint32 newRegValue;
        uint32 watchdogCounter;

        /* Save the current IDAC configuration */
        tmpRegValue = CY_GET_REG32(CapSense_IDAC_PTR);

        /* Wait until Cmod discharges below Vref */
        CyDelayCycles(CapSense_CMOD_DISCHARGE_CYCLES);

        newRegValue = CapSense_CSD_IDAC_PRECHARGE_CONFIG | (tmpRegValue & ~CapSense_IDAC_MOD_CFG_MASK);

        /* Set the IDAC configuration for fast Cmod precharge to Vref */
        CY_SET_REG32(CapSense_IDAC_PTR, newRegValue);

        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            CY_SET_REG32(CapSense_CONFIG_PTR, CY_GET_REG32(CapSense_CONFIG_PTR) & ~CapSense_CONFIG_POLARITY_MASK);
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = CapSense_CSD_PRECHARGE_WATCHDOG_CYCLES_NUM;

        /* Wait until Cmod reaches to Vref */
        while((0u == (CY_GET_REG32(CapSense_STATUS_PTR) & CapSense_STATUS_SAMPLE)) && (0u != watchdogCounter))
        {
            watchdogCounter--;
        }

        #if (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG)
            CY_SET_REG32(CapSense_CONFIG_PTR, CY_GET_REG32(CapSense_CONFIG_PTR) | CapSense_CONFIG_POLARITY_MASK);
        #endif /* (CapSense_IDAC_SINKING == CapSense_CSD_IDAC_CONFIG) */

        /* Restore the current IDAC configuration */
        CY_SET_REG32(CapSense_IDAC_PTR, tmpRegValue);

        /* Enable the sense modulator output */
        CY_SET_REG32(CapSense_CONFIG_PTR, CapSense_configCsd | CapSense_CTANK_PRECHARGE_CONFIG_CSD_EN | CapSense_CONFIG_SENSE_EN_MASK);

        CyIntEnable(CapSense_ISR_NUMBER);

    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}


/*******************************************************************************
* Function Name: CapSense_SsCSDTriggerScan
****************************************************************************//**
*
* \brief
*  This function triggers the scanning.
*
* \details
*  For Third-generation HW block:
*  Writes resolution to start the scanning.
*
*  For Fourth-generation HW block:
*  Trigger the fine initialization (scan some dummy cycles) and start sampling.
*  Fine initialization for CMOD and Start scan.
*  For the fine initialization and sampling, Cmod is static connected to AMUXBUS-A
*  and in every conversion (one cycle of SnsClk), the sensor capacitance is charged
*  from Cmod and discharged to ground using the switches in GPIO cell.
*  The CSDCOMP monitors voltage on Cmod using the sense path and charges Cmod back
*  to Vref using IDACs by connecting IDAC to CSDBUS-A and then the AMUXBUS-A.
*
*******************************************************************************/
CY_INLINE static void CapSense_SsCSDTriggerScan(void)
{
    #if (CapSense_ENABLE == CapSense_CSDV2)

        /* Fourth-generation HW block section */

        uint32 watchdogCounter;

        /* Clear previous interrupts */
        CY_SET_REG32(CapSense_INTR_PTR, CapSense_INTR_ALL_MASK);

        /* Clear pending interrupts  */
        CyIntClearPending(CapSense_ISR_NUMBER);

        /* Enable CSD interrupt */
        CyIntEnable(CapSense_ISR_NUMBER);


        CY_SET_REG32(CapSense_SW_HS_P_SEL_PTR, CapSense_CSD_SW_HS_P_SEL_SCAN);

        /* Set scanning configuration for switches */
        CY_SET_REG32(CapSense_SW_FW_MOD_SEL_PTR, CapSense_CSD_SW_FW_MOD_SEL_SCAN);
        CY_SET_REG32(CapSense_SW_FW_TANK_SEL_PTR,CapSense_CSD_SW_FW_TANK_SEL_SCAN);

        CY_SET_REG32(CapSense_SW_SHIELD_SEL_PTR,  CapSense_CSD_SW_SHIELD_SEL_SCAN);

        #if((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
            /* Connect CTANK to AMUXBUS-B */
            CapSense_SsCSDEnableShieldTank();
        #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

        CY_SET_REG32(CapSense_SW_RES_PTR, CapSense_CSD_SW_RES_SCAN);
        CY_SET_REG32(CapSense_CSDCMP_PTR, CapSense_CSD_CSDCMP_SCAN);

        #if (CapSense_DISABLE == CapSense_CSD_SHIELD_EN)
            /* Disable HSCOMP during the sampling phase when shield is disabled */
            CY_SET_REG32(CapSense_HSCMP_PTR, 0u);
        #else
            CY_SET_REG32(CapSense_HSCMP_PTR, CapSense_HSCMP_SCAN_MASK);
        #endif /* (CapSense_DISABLE == CapSense_CSD_SHIELD_EN) */

        #if (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE)
            /* Force the LFSR to it's initial state (all ones) */
            CY_SET_REG32(CapSense_SENSE_PERIOD_PTR, CY_GET_REG32(CapSense_SENSE_PERIOD_PTR) |
                                                           CapSense_SENSE_PERIOD_LFSR_CLEAR_MASK);
        #endif /* (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE) */

        /* Enable SAMPLE interrupt */
        CY_SET_REG32(CapSense_INTR_MASK_PTR, CapSense_INTR_MASK_SAMPLE_MASK);

        /* Init Watchdog Counter to prevent a hang */
        watchdogCounter = CapSense_CSD_PRECHARGE_WATCHDOG_CYCLES_NUM;

        /* Wait for IDLE state of the HW sequencer */
        while((0u != (CapSense_STAT_SEQ_REG & CapSense_STAT_SEQ_SEQ_STATE_MASK)) && (0u != watchdogCounter))
        {
            watchdogCounter--;
        }

        /* Start SEQUENCER for fine initialization scan for Cmod and then for normal scan */
        CY_SET_REG32(CapSense_SEQ_START_PTR, CapSense_SEQ_START_AZ0_SKIP_MASK |
                                                     CapSense_SEQ_START_AZ1_SKIP_MASK |
                                                     CapSense_SEQ_START_START_MASK);

    #else

        /* Third-generation HW block section */

        /* Set resolution to Counter register to start scanning */
        CY_SET_REG32(CapSense_COUNTER_PTR, CapSense_counterResolution);

    #endif /* (CapSense_ENABLE == CapSense_CSDV2) */
}

#endif  /* (CapSense_ENABLE == CapSense_CSD_EN) */


/* [] END OF FILE */
