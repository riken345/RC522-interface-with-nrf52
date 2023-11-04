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
/** \file app_schedule.h
 * \brief
 *
 **/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APP_SCHEDULE_H
#define APP_SCHEDULE_H

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <string.h>
#include <stdio.h>
#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
#include "components/general/general.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define SCHEDULE_CHARGING_START     1
#define SCHEDULE_CHARGING_STOP      0
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
typedef struct app_scheduler
{
    uint16_t wd_Starttm;
    uint16_t wd_Endtm;
    uint16_t we_Starttm;
    uint16_t we_Endtm;
    uint8_t is_schedule;
    uint8_t sch_current_limit;
}__attribute__((__packed__)) app_scheduler_t;
//app_scheduler_t app_scheduler;
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
/**
 * @brief: This Function initialize Schedule task
 *
 * @param[in] : void
 * 
 * @param[out] :void
 */
void ScheduleTask_Init(void);
/**
 * @brief: This Function calculate hr and minute ((hr*100)+min)
 *
 * @param[in] : void
 * 
 * @param[out] :uint16_t  calculated time
 */
uint16_t compare_scheduletime(void);
/**
 * @brief: This Function return schedule charging status
 *
 * @param[in] : void
 * 
 * @param[out] :uint8_t  status
 */
uint8_t is_schedule_charging(void);
/**
 * @brief: This Function compare current and previous date
 *
 * @param[in] : void
 * 
 * @param[out] :void
 */
void processDayChangeEvent(void);

app_err_t init_input_button(void);

uint8_t input_button_state(void);
/**
 * @brief: This is Scheduleing task handler.
 *
 * @param[in] arg
 *
 * @param[out] void
*/
void ScheduleTask(void *arg);
#ifdef __cplusplus
}
#endif
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif