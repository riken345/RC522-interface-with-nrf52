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
/** \file app_ui.h
 * \brief
 *
 **/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APP_UI_H
#define APP_UI_H

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "json_parser.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define FLASH_UPDATE_TIME                   (150U)         ///< Time in second
#define LIVE_DATA_UPDATE_TIME               (5U)            ///< Time in second // Ideally it should be 5
#define ENERGY_UPDATE_TIME                  (30U)            ///< Time in second
#define ENERGY_CALC_SECONDS                 (3600U)
#define ENERGYFROMMETER 				    1U					///<ENERGY From Power : ENERGYFROMPOWER , Energy From Meter ENERGYFROMMETER
#define ENERGYFROMPOWER                     0U
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
/**
 *  Message Queue Structure
 */
typedef struct uimsgQueueData
{
	uint16_t msg_id;
	uint8_t data[512];
} __attribute__((__packed__)) uimsgQueueData_t;

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
bool app_TimmerInit(void);
/**
 * @brief: This Function initialize UI task
 *
 * @param[in] void
 * @param[out] bool     return on success true either false
 */
 bool app_UITaskInit(void);
/**
 * @brief: This Function return connection status
 *
 * @param[in] void
 * @param[out] bool     return on connection true either false
 */
 bool get_connection_status(void);
 /**
 * @brief: This Function check earth fault condition while charging is going on
 *
 * @param[in] void
 * @param[out] uint8_t 1 or 0
 */
uint8_t get_earthfault_status(void);
/**
 * @brief: This Function check Emergency fault condition
 *
 * @param[in] void
 * @param[out] uint8_t 1 or 0
 */
uint8_t getEmergencyStopSWState(void);
#ifdef __cplusplus
}
#endif
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif