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
 * \file     external_rtc.c
 * \brief
 */

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "external_rtc.h"
#include "components/rtc_mcp79400/rtc_utility.h"
#include "components/i2c_services/i2c_service.h"
#include "zephyr/logging/log.h"
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/timeutil.h>
#include <time.h>
/****************************************************************************
 * MACROS
 ****************************************************************************/
#define TAG "RTC_Over_i2c"
LOG_MODULE_REGISTER(rtc, LOG_LEVEL_DBG);
#define RTC_NODE DT_NODELABEL(rtc0)
struct device *rtc = DEVICE_DT_GET(RTC_NODE);
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
uint8_t rtc_addrs_type;
/****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS
 ****************************************************************************/

app_err_t get_OscTrim_reg(uint8_t *trimming)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCOSC_TRIMMING_REG_ADDRESS, trimming, 1);
	return err;
}
app_err_t set_OscTrim_reg(uint8_t trimming)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCOSC_TRIMMING_REG_ADDRESS, trimming, 0);
	return err;
}

app_err_t get_control_reg(uint8_t *dt)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCCONTROL_REG_ADDRESS, dt, 1);
	return err;
}
app_err_t set_control_reg(uint8_t dt)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCCONTROL_REG_ADDRESS, dt, 0);
	return err;
}

app_err_t get_weekday_reg(uint8_t *dt)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCWEEKDAY_ADDRESS, dt, 1);
	return err;
}
app_err_t set_weekday_reg(uint8_t dt)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCWEEKDAY_ADDRESS, dt, 0);
	return err;
}
/**
 * bit 7-6 Unimplemented: Read as ‘0’
 * bit 5-4 DATETEN<1:0>: Binary-Coded Decimal Value of Date’s Tens Digit Contains a value from 0 to 3
 * bit 3-0 DATEONE<3:0>: Binary-Coded Decimal Value of Date’s Ones Digit Contains a value from 0 to 9
 */
app_err_t get_day(uint8_t *dt)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCDATE_ADDRESS, dt, 1);
	return err;
}
app_err_t set_day(uint8_t dt)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCDATE_ADDRESS, dt, 0);
	return err;
}
/**
 * bit 7-6 Unimplemented: Read as ‘0’
 * bit 5 LPYR: Leap Year bit 1 = Year is a leap year 0 = Year is not a leap year
 * bit 4 MTHTEN0: Binary-Coded Decimal Value of Month’s Tens Digit Contains a value of 0 or 1
 * bit 3-0 MTHONE<3:0>: Binary-Coded Decimal Value of Month’s Ones Digit Contains a value from 0 to 9
 */
app_err_t get_month(uint8_t *month)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCMONTH_ADDRESS, month, 1);
	return err;
}
app_err_t set_month(uint8_t month)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCMONTH_ADDRESS, month, 0);
	return err;
}
/**
 * bit 7-4 YRTEN<3:0>: Binary-Coded Decimal Value of Year’s Tens Digit Contains a value from 0 to 9
   bit 3-0 YRONE<3:0>: Binary-Coded Decimal Value of Year’s Ones Digit Contains a value from 0 to 9
*/
app_err_t get_year(uint8_t *year)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCYEAR_ADDRESS, year, 1);
	return err;
}
app_err_t set_year(uint8_t year)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCYEAR_ADDRESS, year, 0);
	return err;
}
/**
 * bit 7 ST: Start Oscillator bit, 1 = Oscillator enabled  0 = Oscillator disabled
 * bit 6-4 SECTEN<2:0>: Binary-Coded Decimal Value of Second’s Tens Digit Contains a value from 0 to 5
 * bit 3-0 SECONE<3:0>: Binary-Coded Decimal Value of Second’s Ones Digit Contains a value from 0 to 9
 */
app_err_t get_second(uint8_t *sec)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCSECOND_ADDRESS, sec, 1);
	return err;
}
app_err_t set_second(uint8_t sec)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCSECOND_ADDRESS, sec, 0);
	return err;
}
/**
 *  bit 7 Unimplemented: Read as  ‘0’
 * bit 6-4 MINTEN<2:0>: Binary-Coded Decimal Value of Minute’s Tens Digit Contains a value from 0 to 5
 * bit 3-0 MINONE<3:0>: Binary-Coded Decimal Value of Minute’s Ones Digit Contains a value from 0 to 9
 */
app_err_t get_minute(uint8_t *min)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCMIN_ADDRESS, min, 1);
	return err;
}
app_err_t set_minute(uint8_t min)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCMIN_ADDRESS, min, 0);
	return err;
}
/**
 * If 12/24 = 1 (12-hour format):
 *
 * bit 7 Unimplemented: Read as ‘0’
 * bit 6 12/24: 12 or 24 Hour Time Format bit , 1 = 12-hour format 0 = 24-hour format
 * bit 5 AM/PM: AM/PM Indicator bit,1 = PM  0 = AM
 * bit 4 HRTEN0: Binary-Coded Decimal Value of Hour’s Tens Digit Contains a value from 0 to 1
 * bit 3-0 HRONE<3:0>: Binary-Coded Decimal Value of Hour’s Ones Digit Contains a value from 0 to 9
 */
app_err_t get_hour(uint8_t *hr)
{
	app_err_t err = NRF_OK;
	I2C_u8readByte(I2C1, RTC_SLAVE_ADDRESS, RTCHOUR_ADDRESS, hr, 1);
	return err;
}
app_err_t set_hour(uint8_t hr)
{
	app_err_t err = NRF_OK;
	I2C_u8writeByte(I2C1, RTC_SLAVE_ADDRESS, RTCHOUR_ADDRESS, hr, 0);
	return err;
}

app_err_t mcp794x_apply_rtc_time_to_mcu(void)
{
	app_err_t err = NRF_OK;

	/*
	 * Read RTC
	 */
	mcp794x_data_t data = {0};

	err = mcp794x_get_data(&data);

	if (err != NRF_OK)
	{
		err = NRF_FAIL;
	}

	if (data.year > MAXVAL_MKTIME_YEAR)
	{
		err = NRF_ERR_INVALID_ARG;
	}

	/* Set RTC time */
	struct rtc_time time_set;
	int ret;

	time_set.tm_hour = data.hours;	/**< Hours [0, 23] */
	time_set.tm_min = data.minutes; /**< Minutes [0, 59] */
	time_set.tm_sec = data.seconds; /**< Seconds [0, 59] */

	time_set.tm_year = data.year - 1900;		 /**< Year - 1900 */
	time_set.tm_mon = data.month - 1;			 /**< Month [0, 11] */
	time_set.tm_mday = data.day;				 /**< Day of the month [1, 31] */
	time_set.tm_isdst = RTC_DAYLIGHTSAVING_NONE; /**< Daylight saving time flag [-1] (Unknown = -1) */

	ret = rtc_set_time(rtc, &time_set);
	//printf("Error %d\r\n",ret);
	return err;
}
app_err_t mcp794x_save_mcu_time_to_rtc(void)
{
	app_err_t err = NRF_OK;
	/*
	 * Get MCU TimeOfDay
	 */
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	struct tm *the_tm_ptr;
	the_tm_ptr = localtime(&tv.tv_sec);
	/*
	 * Save to RTC
	 */

	mcp794x_data_t data = {0};

	data.year = the_tm_ptr->tm_year + 1900; // @important
	data.month = the_tm_ptr->tm_mon + 1;	// @important
	data.day = the_tm_ptr->tm_mday;
	data.hours = the_tm_ptr->tm_hour+16;
	data.minutes = the_tm_ptr->tm_min+20;
	data.seconds = the_tm_ptr->tm_sec+00;
	LOG_ERR("data.year %d data.month %d data.day %d  data.hours %d data.minutes %d data.seconds %d", data.year, data.month, data.day, data.hours, data.minutes, data.seconds);
	err = mcp794x_set_datetime(data);
	if (err != NRF_OK)
	{
		// GOTO
		return NRF_FAIL;
	}

	return err;
}

app_err_t mcp794x_get_data(mcp794x_data_t *param_ptr_data)
{
	app_err_t err = NRF_OK;
	uint8_t year_raw = 0;

	err = get_second(&param_ptr_data->seconds);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get seconds");
	}

	err = get_minute(&param_ptr_data->minutes);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get minute");
	}

	err = get_hour(&param_ptr_data->hours);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get hour");
	}

	err = get_year(&year_raw);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get year");
	}

	err = get_month(&param_ptr_data->month);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get month");
	}

	err = get_day(&param_ptr_data->day);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to get gate");
	}
	/**
	 * addded for configuration
	 */
	uint8_t reg = 0;
	get_control_reg(&reg);
	//LOG_ERR("reg %02x", reg);
	/**
	 * addded for configuration
	 */
	uint8_t wd = 0;
	get_weekday_reg(&wd);
	//LOG_ERR("wd : %02x", wd);
	/**
	 * addded for configuration
	 */
	uint8_t trimming = 0;
	get_OscTrim_reg(&trimming);
	//LOG_ERR("trimming : %02x", trimming);

	param_ptr_data->seconds &= 0b01111111; // Keep 7 LSB
	param_ptr_data->minutes &= 0b01111111; // Keep 7 LSB
	param_ptr_data->hours &= 0b00111111;   // Keep 6 LSB
	param_ptr_data->day &= 0b00111111;	   // Keep 6 LSB
	param_ptr_data->month &= 0b00011111;   // Keep 5 LSB
	// param_ptr_data->year = 2000 + bcd_to_byte(year_raw);
	param_ptr_data->year = 2000 + year_raw;

	param_ptr_data->seconds = bcd_to_byte(param_ptr_data->seconds);
	param_ptr_data->minutes = bcd_to_byte(param_ptr_data->minutes);
	param_ptr_data->hours = bcd_to_byte(param_ptr_data->hours);
	param_ptr_data->day = bcd_to_byte(param_ptr_data->day);
	param_ptr_data->month = bcd_to_byte(param_ptr_data->month);

	// LOG_ERR("get_second %d", param_ptr_data->seconds);
	// LOG_ERR("get_minute %d", param_ptr_data->minutes);
	// LOG_ERR("get_hour %d", param_ptr_data->hours);
	// LOG_ERR("get_year %d", param_ptr_data->year);
	// LOG_ERR("get_month %d", param_ptr_data->month);
	// LOG_ERR("get_day %d", param_ptr_data->day);
	return err;
}

app_err_t mcp794x_set_datetime(mcp794x_data_t param_data)
{
	app_err_t err = NRF_OK;

	// Validation
	bool validation_passed = true;

	if (param_data.year > MAXVAL_YEAR)
	{
		validation_passed = false;
	}
	if (param_data.month > MAXVAL_MONTH)
	{
		validation_passed = false;
	}
	if (param_data.day > MAXVAL_DAY)
	{
		validation_passed = false;
	}
	if (param_data.hours > MAXVAL_HOURS)
	{
		validation_passed = false;
	}
	if (param_data.minutes > MAXVAL_MINUTES)
	{
		validation_passed = false;
	}
	if (param_data.seconds > MAXVAL_SECONDS)
	{
		validation_passed = false;
	}
	if (validation_passed == false)
	{
		err = NRF_ERR_INVALID_ARG;
		goto cleanup;
	}

	// uint8_t seconds = byte_to_bcd(param_data.seconds);
	uint8_t minutes = byte_to_bcd(param_data.minutes);
	uint8_t hours = byte_to_bcd(param_data.hours);
	uint8_t day = byte_to_bcd(param_data.day);
	uint8_t month = byte_to_bcd(param_data.month);
	// uint8_t year = byte_to_bcd(param_data.year - 2000);
	uint8_t year = param_data.year - 2000;

	// param_data.seconds &= 0b01111111; // Keep 7 LSB
	param_data.seconds |= 0b10000000; // Keep 7 LSB
	// err = set_second(param_data.seconds);
	err = set_second(param_data.seconds);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set seconds");
	}
	param_data.minutes &= 0b01111111; // Keep 7 LSB
	// err = set_minute(param_data.minutes);
	err = set_minute(minutes);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set minute");
	}

	param_data.hours &= 0b00111111; // Keep 6 LSB
	// err = set_hour(param_data.hours);
	err = set_hour(hours);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set hour");
	}

	// err = set_year(param_data.year);
	err = set_year(year);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set year");
	}
	param_data.month &= 0b00011111; // Keep 5 LSB
	// err = set_month(param_data.month);
	err = set_month(month);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set month");
	}
	param_data.day &= 0b0011111; // Keep 6 LSB
	// err = set_day(param_data.day);
	err = set_day(day);
	if (NRF_OK != err)
	{
		LOG_ERR("fails to set gate");
	}
	uint8_t reg = 0xDF;
	reg = 0b11010111;
	set_control_reg(reg);

	uint8_t trim = 0;
	set_OscTrim_reg(trim);

	uint8_t wd = 0;
	wd |= 0b00101000;
	set_weekday_reg(wd);
// LABEL
cleanup:;
	return err;
}

app_err_t GetDateAndTimeFromExtRTC(void)
{
	app_err_t ret;
	mcp794x_data_t curr_time = {0};

	set_timezone();

	ret = mcp794x_get_data(&curr_time);
	if (NRF_OK != ret)
	{
		 LOG_ERR( "Failed to read external RTC data, Error: %d",ret);
	}
	else
	{
		LOG_INF("****** EXTERNAL RTC TIMESTAMP *******");
		LOG_INF("year: %d", curr_time.year);
		LOG_INF("month: %d", curr_time.month);
		LOG_INF("date: %d", curr_time.day);
		LOG_INF("hour: %d", curr_time.hours);
		LOG_INF("min: %d", curr_time.minutes);
		LOG_INF("sec: %d", curr_time.seconds);
		LOG_INF("******************************");
	}

	ret = mcp794x_apply_rtc_time_to_mcu();
	if (NRF_OK != ret)
	{
		LOG_ERR("Failed to read external RTC data, Error: %d", ret);
	}

	struct timeval tv = {0};
	struct timezone tz = {0};
	gettimeofday(&tv, &tz);
	LOG_INF("****** Internal RTC TIMESTAMP *******");
	LOG_INF("year: %d", tv.tv_sec);
	LOG_INF("******************************");
	return ret;
}

uint8_t byte_to_bcd(uint8_t val)
{
	return ((val / 10 * 16) + (val % 10));
}

uint8_t bcd_to_byte(uint8_t val)
{
	return ((val / 16 * 10) + (val % 16));
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
