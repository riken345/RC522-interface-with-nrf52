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
 * \file     rtc_utility.c
 * \brief
 */
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "rtc_utility.h"

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

void get_system_min(uint8_t *min)
{
#if 0
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	mktime(&timeinfo);
	*min = timeinfo.tm_min;
#endif
}

void get_system_hr(uint8_t *hr)
{
#if 0
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	mktime(&timeinfo);
	*hr = timeinfo.tm_hour;
#endif
}

void get_YYMMDD(uint32_t *dt)
{
#if 0
	struct tm timeinfo;
	uint8_t dd,mm,yy;
	time(&now);
	localtime_r(&now, &timeinfo);
	mktime(&timeinfo);
	
	dd = timeinfo.tm_mday;
	mm = (timeinfo.tm_mon + 1);
	yy = ((timeinfo.tm_year + 1900) % 100);
	*dt = (yy * 10000) + (mm * 100) + dd;
#endif
}

void set_timezone_utc()
{
	// @special "" means timezone UTC (@bug Specifying "UTC" does not work!)
}

void set_timezone(void)
{
	/* Local time zone set GMT+5.30 */
}

void get_current_time_yyyymmddhhmmss(char *ptr_buffer)
{
	// @dep buffer 14+1
	// @example I (3550) myapp:   19700101000003
	time_t current_time;
	struct tm *ptr_local_time;

	time(&current_time);
	ptr_local_time = localtime(&current_time);
	strftime(ptr_buffer, 14 + 1, "%Y%m%d%H%M%S", ptr_local_time);
}
void epochtimetodate_yyyymmdd(time_t e, char *ptr_buff)
{
	// time_t epoch;
	struct tm *ptr_local_t;

	ptr_local_t = localtime(&e);
	strftime(ptr_buff, 14 + 1, "%Y%m%d", ptr_local_t);
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
