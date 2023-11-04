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
 * \file     external_rtc.h
 * \brief   
 */
#ifndef EXTERNAL_RTC_H
#define EXTERNAL_RTC_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sys/time.h"
#include <stdbool.h>
#include "components/general/general.h"
/****************************************************************************
* MACROS
****************************************************************************/

//#define RTC_SLAVE_ADDRESS 0xDE       /*!<slave address for read i2c*/
#define RTC_SLAVE_ADDRESS 0x6f       /*!<slave address for read i2c*/
#define RTCSECOND_ADDRESS 0x00       /*!<register address for read seconds*/
#define RTCMIN_ADDRESS    0x01       /*!<register address for read minute*/
#define RTCHOUR_ADDRESS   0x02       /*!<register address for read hour*/
#define RTCDATE_ADDRESS   0x04       /*!<register address for read date*/
#define RTCMONTH_ADDRESS  0x05       /*!<register address for read month*/
#define RTCYEAR_ADDRESS   0x06       /*!<register address for read year*/
#define RTCCONTROL_REG_ADDRESS   0x07       /*!<register address for read controll bits*/
#define RTCWEEKDAY_ADDRESS   0x03       /*!<register address for read week days*/
#define RTCOSC_TRIMMING_REG_ADDRESS 0x08

#if 0
//#define RTC_SLAVE_ADDRESS 0xDE       /*!<slave address for read i2c*/
#define RTC_SLAVE_ADDRESS 0x51         /*!<slave address for read i2c*/
#define RTCSECOND_ADDRESS 0x01         /*!<register address for read seconds*/
#define RTCMIN_ADDRESS    0x02         /*!<register address for read minute*/
#define RTCHOUR_ADDRESS   0x03         /*!<register address for read hour*/
#define RTCDATE_ADDRESS   0x04         /*!<register address for read date*/
#define RTCMONTH_ADDRESS  0x06         /*!<register address for read month*/
#define RTCYEAR_ADDRESS   0x07         /*!<register address for read year*/
#define RTCCONTROL_REG_ADDRESS   0x24  /*!<register address for read controll bits*/
#define RTCWEEKDAY_ADDRESS   0x05      /*!<register address for read week days*/
#define RTCOSC_TRIMMING_REG_ADDRESS 0x08
#endif
// DS3231 Values
#define  MAXVAL_YEAR    (2099)
#define  MAXVAL_MKTIME_YEAR    (2037)
#define  MAXVAL_MONTH   (12)
#define  MAXVAL_DAY     (31)
#define  MAXVAL_HOURS   (23)
#define  MAXVAL_MINUTES (59)
#define  MAXVAL_SECONDS (59)
#define  MAXVAL_WEEK_DAY (7)
#define RTC_DAYLIGHTSAVING_NONE (0)

#define RTC_NONE 0
#define MCP794x 1
#define DS1338 2
#define PCF_85 3

/**
 * ***********************************
 * DS1338 Define start  
 * ***********************************
*/
#define DS1338_ADDR 0x68 /*!< Device address*/

#define DS1338_REG_SECONDS 0x00    /*!<register address for read seconds*/
#define DS1338_REG_MINUTES 0x01    /*!<register address for read minute*/
#define DS1338_REG_HOURS 0x02      /*!<register address for read Hours*/
#define DS1338_REG_DAY 0x03        /*!<register address for read Day*/
#define DS1338_REG_DATE 0x04       /*!<register address for read Date*/
#define DS1338_REG_MONTH 0x05      /*!<register address for read Month*/
#define DS1338_REG_YEAR 0x06       /*!<register address for read Year*/
#define DS1338_REG_CONTROL 0x0E    /*!<register address for read Control*/
#define DS1338_REG_RAM_BEGIN 0x08  /*!<register address for read RAM Begin*/
#define DS1338_REG_RAM_END 0x3F    /*!<register address for read RAM End*/
/**
 * ***********************************
 * PCF85263A Define start  
 * ***********************************
*/
//#define RTC_SLAVE_ADDRESS 0xDE       /*!<slave address for read i2c*/
#define PCF_85_RTC_SLAVE_ADDRESS 0x51         /*!<slave address for read i2c*/
#define PCF_85_RTCSECOND_ADDRESS 0x01         /*!<register address for read seconds*/
#define PCF_85_RTCMIN_ADDRESS    0x02         /*!<register address for read minute*/
#define PCF_85_RTCHOUR_ADDRESS   0x03         /*!<register address for read hour*/
#define PCF_85_RTCDATE_ADDRESS   0x04         /*!<register address for read date*/
#define PCF_85_RTCMONTH_ADDRESS  0x06         /*!<register address for read month*/
#define PCF_85_RTCYEAR_ADDRESS   0x07         /*!<register address for read year*/
#define PCF_85_RTCCONTROL_REG_ADDRESS   0x24  /*!<register address for read controll bits*/
#define PCF_85_RTCWEEKDAY_ADDRESS   0x05      /*!<register address for read week days*/
#define PCF_85_RTCOSC_TRIMMING_REG_ADDRESS 0x08
/**
 * ***********************************
 * PCF85_ Define End  
 * ***********************************
*/
#define RTC_NONE 0
#define MCP794x 1
#define DS1338 2
#define PCF_85 3
/* ********************************** */
/**
If set, in an hour register (DS1338_REG_HOURS, DS1338_REG_A1_HOUR,
DS1338_REG_A2_HOUR, the hour is between 0 and 12, and the
(!AM)/PM bit indicates AM or PM.

If not set, the hour is between 0 and 23.
*/
#define DS1338_HOUR_12 (0x01 << 6)

/**
If DS1338_HOUR_12 is set:
- If set, indicates PM
- If not set, indicates AM
*/
#define DS1338_PM_MASK (0x01 << 5)

// If set, the oscillator has stopped since the last time
// this bit was cleared.
#define DS1338_OSC_STOP_FLAG (0x01 << 5)

#define DS1338_OSC_DISABLE (0x01 << 7) /*!<Set to disable the oscillator*/

/**
These options control the behavior of the SQW/(!INTB) pin. 
*/
#define DS1338_SQWE_FLAG (0x01 << 4)
#define DS1338_SQW_MASK (0x03)
#define DS1338_SQW_32768HZ (0x03)
#define DS1338_SQW_8192HZ (0x02)
#define DS1338_SQW_4096HZ (0x01)
#define DS1338_SQW_1HZ (0x00)

#define decode_bcd(x) ((x >> 4) * 10 + (x & 0x0F))
#define encode_bcd(x) ((((x / 10) & 0x0F) << 4) + (x % 10))
/**
 * ***********************************
 * DS1338 Define End  
 * ***********************************
*/
/****************************************************************************
* GLOBAL VARIABLES
****************************************************************************/
typedef struct rtc_data
{
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} __attribute__((__packed__)) mcp794x_data_t;
extern mcp794x_data_t  rtc_data;

/**
 *  rtc config Structure
 */
typedef struct System_tm
{
    uint64_t sys_epoch;              /*!< system epoch from mobile app */
}__attribute__((packed)) System_tm_t;
/****************************************************************************
* FUNCTION PROTOTYPES
****************************************************************************/

/****************************************************************************
* FUNCTIONS
****************************************************************************/


/**
 * @brief: read hour from i2c
 *
 * @param[in]  hr       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_hour(uint8_t *hr);
/**
 * @brief: read minute from i2c
 *
 * @param[in]  min       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_minute(uint8_t *min);
/**
 * @brief: read seconds from i2c
 *
 * @param[in]  sec       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_second(uint8_t *sec);
/**
 * @brief: read year from i2c
 *
 * @param[in]  year       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_year(uint8_t *year);
/**
 * @brief: read month from i2c
 *
 * @param[in]  month       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_month(uint8_t *month);
/**
 * @brief: read date from i2c
 *
 * @param[in]  dt       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t get_day(uint8_t *dt);
/*
 * Convert uint8_t to binary coded decimal
 */
uint8_t byte_to_bcd(uint8_t val);

/*
 * Convert binary coded decimal to uint8_t
 * @doc 4 bits for each decimal digit.
 */
uint8_t bcd_to_byte(uint8_t val);

/**
 * @brief: set second to i2c
 *
 * @param[in]  hr       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_hour(uint8_t hr);
/**
 * @brief: set second to i2c
 *
 * @param[in]  min       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_minute(uint8_t min);
/**
 * @brief: set second to i2c
 *
 * @param[in]  sec       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_second(uint8_t sec);
/**
 * @brief: set year to i2c
 *
 * @param[in]  year       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_year(uint8_t year);
/**
 * @brief: set month to i2c
 *
 * @param[in]  month       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_month(uint8_t month);
/**
 * @brief: set date to i2c
 *
 * @param[in]  dt       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_day(uint8_t dt);
/**
 * @brief: set control register to i2c
 *
 * @param[in]  dt       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_control_reg(uint8_t dt);
/**
 * @brief: set weekday to i2c
 *
 * @param[in]  dt       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t set_weekday_reg(uint8_t dt);

/**
 * @brief: get date and time from i2c
 *
 * @param[in]  param_ptr_data       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t mcp794x_get_data(mcp794x_data_t* param_ptr_data);
/**
 * @brief: set date and time to i2c
 *
 * @param[in]  param_data       data pointer
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t mcp794x_set_datetime(mcp794x_data_t param_data);

/**
 * @brief: read time form rtc and set to esp32
 *
 * @param[in]  void      
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t mcp794x_apply_rtc_time_to_mcu(void);
/**
 * @brief: read time form internalrtc and set to external rtc
 *
 * @param[in]  void 
 *
 * @param[out]  app_err_t    return STM_OK or STM_FAIL
*/
app_err_t mcp794x_save_mcu_time_to_rtc(void);
/**
 * @brief: This Function read external RTC and update time data into local RTC
 *
 * @param[in] void
 * @param[out] app_err_t return STM_OK or STM_FAIL
*/
app_err_t GetDateAndTimeFromExtRTC(void);

/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
