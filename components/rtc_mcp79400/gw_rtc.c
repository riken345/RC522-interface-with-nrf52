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
 * \file     gw_rtc.c
 * \brief   
 */
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "gw_rtc.h"

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
void set_get_system_time_from_cli(uint8_t op_type, time_t time)
{
	if(op_type) {
		struct timeval tv = { .tv_sec = time, .tv_usec = 0 };
		set_system_time(&tv);
	}
}

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
void set_system_time(struct timeval *tv)
{
	// LOG_ERR(UART, "set_system_time : %ld", tv->tv_sec);
	gw_rtc_set_time(tv);
}

/////////////////////////////
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
time_t get_system_real_time(void)
{
	time_t sys_time = 0;
	struct timeval now;
	memset(&now, 0, sizeof(now));
	gettimeofday(&now, NULL);
	sys_time = now.tv_sec;
	return sys_time;
}

void gw_rtc_set_time(struct timeval *tv)
{
	
}

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
time_t set_get_system_init_time(uint8_t op_type, time_t init_time)
{
	static time_t init_sys_time = 0;
	if (1 == op_type) {
		init_sys_time = init_time;
		// LOG_ERR(UART, "INIT time : %d", init_time);
	} else {
		return init_sys_time;
	}

	return (time_t) 0;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
