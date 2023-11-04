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
 * \file     rtc_utility.h
 * \brief   
 */

#ifndef RTC_UTILITY_H
#define RTC_UTILITY_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sys/time.h"
#include "components/general/general.h"
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
 * @name    get_system_min
 * @brief   get current minute from epoch
 *
 * This function will get minute from system.
 *
 * @param [in] void 
 *
 * @retval [out] minute
 **/
void get_system_min(uint8_t* min);
/**
 * @name    get_system_hr
 * @brief   get current hour from epoch
 *
 * This function will get hour from system.
 *
 * @param [in] void 
 *
 * @retval [out] hour
 **/
void get_system_hr(uint8_t* hr);
/**
 * @name    set_timezone_utc
 * @brief   set system time to indian standard time
 *
 * This function will set timezone to system.
 *
 * @param [in] void 
 *
 * @retval [out] void
 **/
void set_timezone_utc(void);
/**
 * @name    set_timezone
 * @brief   set system time to indian standard time
 *
 * This function will set timezone to system.
 *
 * @param [in] void 
 *
 * @retval [out] void
 * 
 **/
void set_timezone(void);
/**
 * @name    get_current_time_yyyymmddhhmmss
 * @brief   get system time in yyyymmddhhss format
 *
 *  This function will convert epoch time to yyyymmddhhss format
 * 
 * @example char current_time[20];
 * get_current_time_yyyymmddhhmmss(current_time);
 * 
 * @param [in] datapointer 
 *
 * @retval [out] void
 */
void get_current_time_yyyymmddhhmmss(char *ptr_buffer);
/**
 * @name    get_YYMMDD
 * @brief   get time in formet
 *
 * This function will get time in YYMMDD format.
 *
 * @param [in] uint32_t* data pointer 
 *
 * @retval [out] void
 * 
 **/
void get_YYMMDD(uint32_t *dt);
void epochtimetodate_yyyymmdd(time_t e,char *ptr_buff);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
