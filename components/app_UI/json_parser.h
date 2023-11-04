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
/** \file json_parser.h
 * \brief
 *
 **/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "components/general/general.h"
#include "zephyr/kernel.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
/**
 * Common json keys
*/
#define JSON_MSG_ID_KEY                 "msg_id"
#define JSON_DEVICE_ID_KEY              "dev_id"
#define JSON_STATUS_KEY                 "status"
#define JSON_PROPERTIES                 "properties"
#define JSON_ARRAY_KEY                  "array"
#define JSON_FW_VERSION_KEY             "fw_version"

#define JSON_PWD_KEY                    "passpharse"
#define JSON_WIFI_SSID_KEY              "ssid"
#define JSON_APN_KEY                    "apn"
#define JSON_RATED_ENERGY_KEY           "rated-energy"
#define JSON_SERIAL_NO_KEY              "SerialNo"
#define JSON_HARDWARE_VERSION_KEY       "HardwareVersion"
#define JSON_SOFTWARE_VERSION_KEY       "SoftwareVersion"
#define JSON_LATITUDE_KEY               "latitude"
#define JSON_LONGITUDE_KEY              "longitude"
#define JSON_FAULT_CODE_KEY             "fault code"
#define JSON_FAULT_STATUS_KEY           "fault status"
#define JSON_BLE_STATUS                 "BLE_status"
#define JSON_WIFI_STATUS                "WiFi_status"
/**
 * epoch time set JSON key
*/
#define JSON_SYSTEM_EPOCH_KEY           "sys_epoch"
/**
 * OTA JSON key
*/
#define JSON_OTA_STATUS                 "ota_status"
#define JSON_OTA_DISCRIPTION            "data"
#define JSON_NAME_KEY                   "name"
#define JSON_FILE_NAME_KEY              "file_name"
#define JSON_OTA_URL_KEY                 "url"
/**
 * Heartbeat JSON key
*/
#define JSON_HBTIMESTAMP_KEY            "sys_time"
#define JSON_HBTOTALENERGY_KEY          "total_energy"
#define JSON_HBEVCONNECTION_KEY         "ev_connection_status"
#define JSON_HBEVSTATE_KEY              "ev_charging_state"
#define JSON_HB                          "heartbeat"
/**
 * scheduling JSON key
*/
#define JSON_IS_SCHEDULE_KEY            "is_schedule"
#define JSON_WD_START_TM_KEY            "wd_start_tm"
#define JSON_WD_END_TM_KEY              "wd_end_tm"
#define JSON_WE_START_TM_KEY            "we_start_tm"
#define JSON_WE_END_TM_KEY              "we_end_tm"
#define JSON_SCH_CURR_LIMIT_KEY         "sch_max_current"
/**
 * Metering JSON key
*/
#define JSON_VOLTAGE_KEY                "voltage"
#define JSON_CURRENT_KEY                "current"
#define JSON_POWER_KEY                  "power"
#define JSON_SESSION_ENERGY_KEY         "session_energy"
/**
 * currentlimit JSON key
*/
#define JSON_MAX_CURRENT_KEY             "max_current"
/**
 * fault information JSON key
*/
#define JSON_FAULT_KEY                   "err_code"
/**
 * start stop command JSON key
*/
#define JSON_START_STOP_CMD_KEY          "charging_start_stop"

/**
 * read history command JSON key
*/
#define JSON_START_NUMBER_CMD_KEY          "start_number"
#define JSON_END_NUMBER_CMD_KEY            "end_number"
#define JSON_ID_KEY                        "id"
#define JSON_EVENT_KEY                     "event"
#define JSON_TIMESTAMP_KEY                 "time"
#define JSON_TIME_DURATION_KEY             "duration"
#define JSON_CHARGING_MODE                 "mode"
#define JSON_START_USER_NAME               "user_name"
#define JSON_STOP_USER_NAME                "stop_name" 
/**
 * read energy history command JSON key
*/
#define JSON_TYPE_CMD_KEY          "type"
#define JSON_HISTORY_ENERGY_KEY           "energy"

/**
 * available history logs command JSON key
*/
#define JSON_AVAILABLE_LOG                 "log"
/**
 * Automode JSON key
*/
#define JSON_AUTOMODE_KEY                  "automode"
/**
 * start time JSON key
*/
#define JSON_STARTTM_KEY                  "starttm"

#define CHECK_STRUCT_EMPTY				    0
/**
 * Charger type save  command JSON key
*/
#define CHARGER_NAME_JSON_KEY          "charger_name"
#define CHARGER_CONNECTOR_JSON_KEY     "connection_type"   
#define CHARGER_CAPACITY_JSON_KEY      "charger_capacity"
#define CHARGER_PHASE_TYPE_JSON_KEY    "charger_type"
/**
 * OTA JSON key
*/
#define JSON_OCPP_URL_KEY              "url" 
/**
 * Username JSON key
*/
#define JSON_USERNAME_KEY               "user_name"
/**
 * unitprice JSON key
*/
#define JSON_UNITPRICE_KEY               "unit_price"
/**
 * savedprice JSON key
*/
#define JSON_SAVEDPRICE_KEY               "saved_price"
/**
 * Currency JSON key
*/
#define JSON_CURRENCY_KEY               "currency"

#define UI_CHARGING_START_CMD     1
#define UI_CHARGING_STOP_CMD      2


#define WEEK_START_RECORD           1
#define WEEK_END_RECORD             8
#define MONTH_START_RECORD          1
#define MONTH_END_RECORD            31
#define YEAR_START_RECORD           1
#define YEAR_END_RECORD             13


#define USER_NAME_SIZE          11
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
/**
 * Json error code enumeration
 */
typedef enum jsonErrorCode
{
	CMD_SUCCESS_RESP = 0,
    CMD_FAILURE_RESP,
    VALID_JSON_PAYLOAD,
    INVALID_MSG_CODE,
    INVALID_DEVICE_MAC,
    INVALID_JSON_PROPERTIES,
    INVALID_PAYLOAD_DATA,
    JSON_STRING_NULL_PTR,
    JSON_ROOT_ERROR,   
    CMD_BLE_ERROR,
    CMD_WIFI_ERROR,
    CMD_NVS_FLASH_ERROR,
    CMD_DATA_NOT_FOUND,
    CMD_THRESHOLD_ERROR,
}__attribute__((__packed__)) jsonErrorCode_e;

/**
 * Json command enumeration
 */
typedef enum JSON_Msgid
{
    //settable properties command enumeration
    SET_SYSTEM_TIME_CMD = 1,
    SET_SCHEDULE_CONFIG_CMD,
    SET_CHARGING_START_STOP_CMD,
    OTA_UPDATE_CMD,
    OTA_STATUS_NOTIFY_CMD,
    GET_METERING_INFO_CMD,
    GET_FAULT_INFO_CMD,
    GET_HEARTBEAT_CMD,
    FACTORY_RESET_CMD,
    GET_HISTORY_LOG_CMD,
    GET_HISTORY_EVENT_CMD,
    GET_HISTORY_ENERGY_CMD,
    SET_CURRENT_LIMIT_CMD,
    GET_CURRENT_LIMIT_CMD,
    GET_SCHEDULE_CONFIG_CMD,
    GET_AVAILABLE_LOGS_CMD,
    GET_AVAILABLE_EVENT_CMD,
    SET_AUTOMODE_CMD,
    GET_AUTOMODE_CMD,
    GET_ALL_CONFIG_READ_CMD,
    GET_MAC_READ_CMD,
    GET_CHARGING_START_TIME,
    GET_FW_VERSION,
    SET_WIFI_CONFIG_CMD,
    GET_WIFI_CONFIG_CMD,
    SET_CHARGER_CONFIGURATION,
    GET_CHARGER_CONFIGURATION,
    SET_OCPP_CMD,  
    SET_UNIT_PRICE_CMD,
    GET_UNIT_PRICE_CMD,
    SET_HEARTBEAT_CMD,
    //TODO:Extra command start from here
    GET_OCPP_CMD,
    SET_BLE_CONFIG_CMD,
    SET_GPRS_CONFIG_CMD,
    GET_LOCATION_INFO_CMD,
    GET_ALERT_INFO_CMD,
    GET_CONNECTION_STATUS_CMD,
    INVALID_CMD_INDEX = 0xFF,
    //Notification Event command enumeration
}__attribute__((__packed__)) JSON_Msgid_e;

/**
 * Energy Data enumeration
 */
typedef enum energyReadType
{
	LAST_7_DAYS = 1,
    LAST_30_DAYS,
    LAST_YEAR,
}__attribute__((__packed__)) energyReadType_e;

/**
 * Json error code enumeration
 */
typedef enum chargingMode
{
    MANUALMODE_ENABLE = 0, 
    AUTOMODE_ENABLE = 1,
    RFIDMODE_ENABLE = 2,
    OCPPMODE_ENABLE = 3,
}__attribute__((__packed__)) chargingMode_e;


//TODO :Add below Q in BLE file
#define BLE_BUFFER_SIZE 512
/**
 *  Message Queue Structure
 */
typedef struct blemsgQueueData
{
	uint16_t msg_id;
	uint8_t data[BLE_BUFFER_SIZE];
} __attribute__((__packed__)) blemsgQueueData_t;

#define BLE_Q_LEN (5U)
#define BLE_Q_ITEM_SIZE (sizeof(blemsgQueueData_t))
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
/**
 * @brief: JSON string parser.
 *
 * @param[in] json_string       json formatted payload
 *
 * @param[out] jsonErrorCode_e  return json error code
 */
jsonErrorCode_e ParseJSON_String(const uint8_t *json_string);
/**
 * @brief: This function send command status to mobile/cloud
 *
 * @param[in] msgid        msg ID
 * @param[in] status       status
 *
 * @param[out] void
*/
void sendStatus(JSON_Msgid_e msgid, uint8_t status);
/**
 * @brief: send current status of ota in %(1 to 100) in json format
 *
 * @param[in] eStatus           		HTTP Client events data
 * 
 * @param[in] dataDiscription           json string
 *
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t sendLatestOTAstatus(uint8_t eStatus, char *dataDiscription);

/**
 * @brief: send current status of ota in %(1 to 100) in json format
 *
 * @param[in] QHandler           		TCP queue handler
 * 
 * @param[in] msgData1                  message data which send to queue
 *
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t send_ble_Q(struct k_msgq QHandler, blemsgQueueData_t msgData1);

/**
 * @brief: send heartbeat of device in json format
 *
 * @param[in] void          
 * 
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t SendHeartbeatData(void);

/**
 * @brief: send metering information of device in json format
 *
 * @param[in] void          
 * 
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t SendMeteringInfo(void);

/**
 * @brief: send fault information of device in json format
 *
 * @param[in] void          
 * 
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t SendFaultInfo(void);

/**
 * @brief: send current limit information of device in json format
 *
 * @param[in] void          
 * 
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t SendCurrentLimitData(void);

/**
 * @brief: send schedule information of device in json format
 *
 * @param[in] void          
 * 
 * @param[out] app_err_t    			return NRF_OK or NRF_FAIL
 */
app_err_t SendScheduleData(void);
#if 0
 /**
 * @brief: API to Send History Events to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetAllHistoryEnergy(uint16_t start_record,uint16_t end_record);
/**
 * @brief: API to Send History Logs to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetHistoryLogs(uint16_t start_record,uint16_t end_record);
#endif
/**
 * @brief: API to Send start stop command from Mobile and Cloud
 *
 * @param[in] void
 * @param[out] uint8_t    1-->start,2-->stop
 */
uint8_t get_uiStartStop_cmd(void);
/**
 * @brief: API to get maxximum current limit from user side
 *
 * @param[in] void
 * @param[out] uint8_t    values
 */
uint8_t get_max_current_limit(void);
/**
 * @brief: API to will reset start command if charger is not ready
 *
 * @param[in] void
 * @param[out] void
 */
void resetUIStartCmd(void);
/**
 * Validate device mac address
 */
/**
 * @brief: Validate device mac address
 *
 * @param[in] data     device mac address
 * @param[in] err      error status update pointer
 *
 * @param[out] bool         return true or false
 */
bool isValidMacAddress(uint8_t *data, bool *err);
/**
 * @brief: API to Send available History Logs count to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t SendLogCount(void);
/**
 * @brief: This function send automode status to mobile/cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t SendAutModeStatus(void);

/**
* @brief: This function send mac address data to mobile/cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t SendMACAddress(void);
/**
* @brief: This function send Fw Version to mobile/cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t sendfwversion(void);
/**
* @brief: This function send unit price to mobile/cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t sendunitprice(void);
/**
 * @brief: This function send charging start time to mobile/cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t SendChargingStartTime(void);
/**
 * @brief: API to Send last seven days energy to Mobile and Cloud
 *
 * @param[in]  uint16_t Start Record
 * @param[in]  uint16_t End Record
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLast_7DaysEnergy(uint16_t start_record,uint16_t end_record);
/**
 * @brief:  API to Send last 30 days energy to Mobile and Cloud
 *
 * @param[in] uint16_t Start Record
 * @param[in] uint16_t End Record
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLast_30DaysEnergy(uint16_t start_record,uint16_t end_record);
/**
 * @brief:  API to Send last Year energy to Mobile and Cloud
 *
 * @param[in] uint16_t Start Record
 * @param[in] uint16_t End Record
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLastYearEnergy(uint16_t start_record, uint16_t end_record);

app_err_t Heartbeatsend(void);
void set_ocpp_url(char *buff);
/**
 * @brief:  API to Send charger details
 *
 * @param[in]   void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
#if 0
app_err_t send_charger_details(void);
#endif

#ifdef __cplusplus
}
#endif
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif