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
 * \file     volt_curr_monitor.h
 * \brief
 */

#ifndef VOLT_CURR_MONITOR_H
#define VOLT_CURR_MONITOR_H

/**
 * \headerfile ""
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "components/general/general.h"
// #include "components/adc_services/adc_service.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define VOLT_CHANNEL_FOR_RMS  3/*Voltage channel count for calculated RMS value*/
#define VLINE_DATA_INDEX 0
#define COUNT_20MILLISECONDS 20
#define TOTAL_20MS_SAMPLE_COUNT 5
#define VOLT_MILIVOLT_CONV_FACTOR 1000
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/

/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
void VOLTAGE_processADCData(void);
void VOLTAGE_caladcLineGain(int16_t i16TargetVoltage);
uint32_t VOLTAGE_loadRMSValue(void);

void CURRENT_processADCData(void);
uint32_t CURRENT_loadRMSADCValue(void);
uint32_t CURRENT_loadRMSValue(void);
void CURRENT_caladcLineGain(int16_t i16TargetCurrent);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
