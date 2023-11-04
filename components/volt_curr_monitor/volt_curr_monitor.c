/*
 *      Copyright (c) 2023
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *		@author: Jaimit Shah
 *
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of Private Company.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise
 *      duplicated and the information herein may not be used,
 *      disseminated or otherwise disclosed, except with the prior
 *      written consent of Private Company.
 *
 */
/*!
 * \file     volt_curr_monitor.c
 * \brief
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <zephyr/kernel.h>
#include "volt_curr_monitor.h"
#include "zephyr/logging/log.h"
#include "components/gpio_config/gpio_config.h"
#include <zephyr/drivers/spi.h>
#include "components/adc_services/adc_service.h"
/************************************************************************
 * Define Macros
 ************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/
uint32_t gui32VoltLineRMSVal = 0;
static uint32_t gui32VoltageBufferRMS[VOLT_CHANNEL_FOR_RMS] = {0};
static uint32_t gui32VoltageForCal = 0;
/************************************************************************
 * Define Global Variables
 ************************************************************************/
uint16_t gui16LineVoltageSampleBUffer[6] = {0};
uint8_t gui8VoltageBufferIndex = 0;
uint32_t gui32CURRENTCalibRMS = 0;
uint32_t gui32CurrentRMSValue = 0;
extern int32_t gi32ReadADCValue[ADC_MAX_CHANNEL];
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/
static uint32_t ADC_getSumOfSquareOfADCData(eADCChannels_e eADCChannels);
/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
/**
 * @brief Process line voltage data
 * @note Call this API at 1ms
 */
void VOLTAGE_processADCData(void)
{
    /*To Trigger RMS value calculation at every 100ms*/
    static uint8_t ui8ExecutionCount = 0;
    /*Save sum of square of ADC value of line into buffer till 100ms*/
    static uint64_t ui32SumOfSquareADCValue[VOLT_CHANNEL_FOR_RMS] = {0};
    static uint8_t ui8AverageVoltageSamples = 0;
    uint8_t ui8LoopIndex = 0;
    static uint16_t counter = 0;

    /*Reset buffer after calculate value*/
    if (0 == ui8ExecutionCount)
    {
        memset(ui32SumOfSquareADCValue, 0x00, sizeof(ui32SumOfSquareADCValue));
    }
    /*Increment tick of 100ms*/
    ui8ExecutionCount++;
    ui32SumOfSquareADCValue[VLINE_DATA_INDEX] += (ADC_getSumOfSquareOfADCData(ADC_VLINE_CHANNEL));
    // printk("gui32VoltLineRMSVal = %d \n\r", gui32VoltLineRMSVal);
    if (COUNT_20MILLISECONDS <= ui8ExecutionCount)
    {
        ui8ExecutionCount = 0;
        /* Save value in Buffer */
        gui16LineVoltageSampleBUffer[gui8VoltageBufferIndex] = sqrt((ui32SumOfSquareADCValue[VLINE_DATA_INDEX] / COUNT_20MILLISECONDS));
        gui8VoltageBufferIndex++;
        /*Convert line ADC count to Actual line voltage*/
        //  printk("ui8AverageVoltageSamplesui8AverageVoltageSamples = %d gui8VoltageBufferIndex %d \n", ui8AverageVoltageSamples,gui8VoltageBufferIndex);
        if (true == ui8AverageVoltageSamples || TOTAL_20MS_SAMPLE_COUNT <= gui8VoltageBufferIndex)
        {
            if (TOTAL_20MS_SAMPLE_COUNT <= gui8VoltageBufferIndex)
            {
                ui8AverageVoltageSamples = true;
                gui8VoltageBufferIndex = 0;
            }
            for (ui8LoopIndex = 0; TOTAL_20MS_SAMPLE_COUNT > ui8LoopIndex; ui8LoopIndex++)
            {
                gui32VoltageBufferRMS[VLINE_DATA_INDEX] += gui16LineVoltageSampleBUffer[ui8LoopIndex];
            }
            gui32VoltageBufferRMS[VLINE_DATA_INDEX] = (gui32VoltageBufferRMS[VLINE_DATA_INDEX] / TOTAL_20MS_SAMPLE_COUNT);
            gui32VoltageBufferRMS[VLINE_DATA_INDEX] *= VOLT_MILIVOLT_CONV_FACTOR;
            gui32VoltageForCal = gui32VoltageBufferRMS[VLINE_DATA_INDEX];

            /*Convert line ADC count to Actual line voltage*/
            gui32VoltLineRMSVal = (uint32_t)((gui32VoltageBufferRMS[VLINE_DATA_INDEX] * 1627 ) >> FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL);

            if (counter++ > 1000)
            {
                counter = 0;
                printk("gui32VoltLineRMSVal = %d \n\r", gui32VoltLineRMSVal);
            }

            /*Reset sum of square of va;lue for line voltage*/
            gui32VoltageBufferRMS[VLINE_DATA_INDEX] = 0;
        }
        /*Reset sum of square of va;lue for line voltage*/
        ui32SumOfSquareADCValue[VLINE_DATA_INDEX] = 0;
    }
}
/**
 * @brief Calculate ADC line voltage gain
 * @param i16TargetVoltage Targer Line voltage(230)
 */
void VOLTAGE_caladcLineGain(int16_t i16TargetVoltage)
{
    uint16_t ui16ADCLineVolGain = 0;
    float fVoltageADCLineGain = 0.0f;

    /*Calculate gain value for line voltage*/
    fVoltageADCLineGain = ((float)(i16TargetVoltage * VOLT_MILIVOLT_CONV_FACTOR) /
                           (float)gui32VoltageForCal);
    /*convert float to fix*/
    ui16ADCLineVolGain = float_to_fixed(FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL, fVoltageADCLineGain);
    printk("voltage after Gain = %d \r\n", (int)(ui16ADCLineVolGain));
    printk("voltage after Gain = %u \r\n", (gui32VoltageForCal));
    printk("voltage after Gain = %u \r\n", (uint32_t)(ui16ADCLineVolGain * gui32VoltageForCal) >> FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL);
    /*TODO: Write API to save gain in FRAM*/
}
/**
 * @brief Calculate ADC line current gain
 * @param i16TargetCurrent Targer Line voltage(230)
 */
void CURRENT_caladcLineGain(int16_t i16TargetCurrent)
{
    uint16_t ui16ADCCurrentGain = 0;
    float fCurrentADCGain = 0.0f;
    uint32_t ui32CurrentADCValue = 0;

    ui32CurrentADCValue = CURRENT_loadRMSADCValue();
    /*Calculate gain value for load current*/
    fCurrentADCGain = (float)i16TargetCurrent / (float)ui32CurrentADCValue;
    /*convert float to fix*/
    ui16ADCCurrentGain = float_to_fixed(FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL, fCurrentADCGain);
    // printk("voltage after Gain = %d", (int)(ui16ADCCurrentGain * ui32CurrentADCValue) >> FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL);
    /*TODO: Write API to save gain in FRAM*/
}

/**
 * @brief get sum of square ofADC value
 * @param eADCChannels :adc channel number
 * @retval sum of square of adc channel
 */
static uint32_t ADC_getSumOfSquareOfADCData(eADCChannels_e eADCChannels)
{
    uint32_t ui32SumofSquareValue = 0;
    int16_t i16ADCSquareValue = 0;
    // printk("%d - %d \n\r", gi32ReadADCValue[eADCChannels],gi32ReadADCValue[ADC_VOLT_OFFSET_CHANNEL]);
    if (ADC_CURR_LOAD_CHANNEL == eADCChannels)
    {
        i16ADCSquareValue = (gi32ReadADCValue[eADCChannels] - gi32ReadADCValue[ADC_CURR_OFFSET_CHANNEL]);
    }
    else if (ADC_VLINE_CHANNEL == eADCChannels)
    {
        i16ADCSquareValue = (gi32ReadADCValue[eADCChannels] - gi32ReadADCValue[ADC_VOLT_OFFSET_CHANNEL]);
    }
    ui32SumofSquareValue = (uint32_t)(i16ADCSquareValue * i16ADCSquareValue);
    return ui32SumofSquareValue;
}
/**
 * @brief Process load current
 */
void CURRENT_processADCData(void)
{
    static uint8_t ui8ExecutionCount = 0;
    static uint64_t ui64SumofSquareADCValue = 0;

    if (0 == ui8ExecutionCount)
    {
        ui64SumofSquareADCValue = 0;
    }
    ui8ExecutionCount++;
    ui64SumofSquareADCValue += ADC_getSumOfSquareOfADCData(ADC_CURR_LOAD_CHANNEL);
    if (COUNT_20MILLISECONDS <= ui8ExecutionCount)
    {
        ui8ExecutionCount = 0;
        gui32CurrentRMSValue = sqrt(ui64SumofSquareADCValue / COUNT_20MILLISECONDS);
        gui32CurrentRMSValue *= VOLT_MILIVOLT_CONV_FACTOR;
        gui32CURRENTCalibRMS = (uint32_t)((gui32CurrentRMSValue * 1234) >> FIXED_POINT_BIT_RESOLUTION_ADC_CHANNEL);
        ui64SumofSquareADCValue = 0;
    }
}
/**
 * @brief return ADC count of load current
 * @retval RMS ADC value of load current
 */
uint32_t CURRENT_loadRMSADCValue(void)
{
    return gui32CurrentRMSValue;
}
/**
 * @brief return RMS value of load current
 * @retval RMS value of load current
 */
uint32_t CURRENT_loadRMSValue(void)
{
    return gui32CURRENTCalibRMS;
}
/**
 * @brief return RMS value of load voltage
 * @retval RMS value of load voltage
 */
uint32_t VOLTAGE_loadRMSValue(void)
{
    return gui32VoltLineRMSVal;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/