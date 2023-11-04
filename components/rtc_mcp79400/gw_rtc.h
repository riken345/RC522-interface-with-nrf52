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
 * \file     gw_rtc.h
 * \brief   
 */

#ifndef GW_RTC_H
#define GW_RTC_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sys/time.h"


/****************************************************************************
* MACROS
****************************************************************************/

/****************************************************************************
* GLOBAL VARIABLES
****************************************************************************/

/****************************************************************************
* FUNCTION PROTOTYPES
****************************************************************************/

/****************************************************************************
* FUNCTIONS
****************************************************************************/
/**
 * @name    set_get_system_time_from_cli
 * @brief   Set/Get time based on argument
 * @ingroup component
 *
 * This function will Set/get time in/from system.
 *
 * @param [in] op_type : Operation Type
						 1 : SET
						 0 : GET
 * 				time   : Time to set
 *
 * @ret [out] Time
 **/
void set_get_system_time_from_cli(uint8_t op_type, time_t time);

/**
 * @name    set_system_time
 * @brief   Set time based on argument
 * @ingroup component
 *
 * This function will Set time in system.
 *
 * @param [in] tv : Time
 *
 * @ret [out] None
 **/
void set_system_time(struct timeval *tv);

/**
 * @name    get_system_real_time
 * @brief   Get time from system
 * @ingroup component
 *
 * This function will calculate time based on Init time and system tick time.
 *
 * @param [in] None
 *
 * @ret [out] time_t : Calculated Current Time
 **/
time_t get_system_real_time(void);
/**
 * \fn gw_rtc_set_time(struct timeval *tv)
 * \brief  set the RTC peripheral with time.
 * \param  time value using timeval structure
 * time.h
 *
 * \return
 *     None
 */
void gw_rtc_set_time(struct timeval *tv);

/**
 * @name    set_get_system_init_time
 * @brief   Set/Get time based on argument
 * @ingroup component
 *
 * This function will Set/get time in/from system.
 * It will fill init time based on argument.
 *
 * @param [in] op_type : Operation Type
						 1 : SET
						 0 : GET
 * 				time   : Time to set
 *
 * @ret [out] time_t : Current Time
 **/
time_t set_get_system_init_time(uint8_t op_type, time_t init_time);


void gw_rtc_set_time(struct timeval *tv);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
