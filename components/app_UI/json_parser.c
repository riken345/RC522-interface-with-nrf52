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
/** \file json_parser.c
 * \brief
 *
 **/

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <stdlib.h>
#include "json_parser.h"
#include "app_ui.h"
#include "components/rtc_mcp79400/gw_rtc.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/rtc_mcp79400/rtc_utility.h"
#include "components/app_schedule/app_schedule.h"
#include "components/app_db/app_db.h"
#include "zephyr/logging/log.h"

// #include "cp_pwm.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define TAG "JSON_PARSER"
#define MINIMUM_CURRENT_SUPPORT 6U
#define MAXIMUM_CURRENT_SUPPORT 30U
LOG_MODULE_REGISTER(jsonparser, LOG_LEVEL_DBG);
/****************************************************************************
 * TYPES
 ****************************************************************************/

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
/** Device Mac Address */
// uint8_t dev_mac[MAC_ADDR_SIZE] = {0}; /*!< Device mac address */
uint8_t UI_Start_Stop_cmd;
uint8_t StopCmdRcvd;
uint8_t start_user_name[USER_NAME_SIZE];
uint8_t stop_user_name[USER_NAME_SIZE];
uint8_t ocpp_link_get;
extern struct k_msgq uiMsgQueue;
extern struct k_msgq bleMsgQueue;
blemsgQueueData_t blemsgData;
extern uint16_t Max_Current_Limit;
/****************************************************************************
 * PRIVATE FUNCTIONS
 ****************************************************************************/
/**
 * @brief: Decode firmware version from the string
 *
 * @param[in] fileName      firmware version string
 * @param[in] version       decode firmware version from string and convert
 *                          into integer format
 *
 * @param[out] bool  return true or false
 */
bool DecodeOTAfirmwareVer(uint8_t *fileName, uint8_t *version);

/**
 * @brief: Validate message identifier
 *
 * @param[in] messageID     message identifier
 * @param[in] err           error status update pointer
 *
 * @param[out] bool         return true or false
 */
static bool isValidMessageID(int32_t messageID, bool *err);
/****************************************************************************
 * EXPORTED FUNCTIONS
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief: JSON string parser
 */
jsonErrorCode_e ParseJSON_String(const uint8_t *json_string)
{
    jsonErrorCode_e status = JSON_STRING_NULL_PTR;
    cJSON *root = NULL;
    uimsgQueueData_t json_msgData = {0U};
    uint16_t messageID = INVALID_CMD_INDEX;

    if (NULL != json_string)
    {
        root = cJSON_Parse((char *)json_string);
        if (NULL != root)
        {
            bool error = false;

            /** Debugging purpose print json payload */
            LOG_INF("[JSON]: %s", json_string);

            cJSON *msgId = cJSON_GetObjectItem(root, JSON_MSG_ID_KEY);
            if ((NULL != msgId) && cJSON_IsNumber(msgId))
            {
                messageID = msgId->valueint;
                LOG_INF(" *************** messageID: %u ", messageID);

                if (!isValidMessageID(messageID, &error))
                {
                    status = INVALID_MSG_CODE;
                }
                else
                {
#if 0
                    if (!error)
                    {
                        cJSON *devId = cJSON_GetObjectItem(root, JSON_DEVICE_ID_KEY);
                        if ((NULL != devId) && cJSON_IsString(devId))
                        {
                            LOG_INF( "indevice id");
                            uint8_t mac_addr[13U] = {0U};
                            (void)strncpy((char *)mac_addr, devId->valuestring, MAC_ADDR_SIZE);
                            if (!isValidMacAddress(mac_addr, &error))
                            {
                                status = INVALID_DEVICE_MAC;
                            }
                        }
                        else
                        {
                            error = true;
                            status = INVALID_JSON_PROPERTIES;
                        }
                    }
#endif

                    if (!error)
                    {
                        switch (messageID)
                        {
                        case SET_SYSTEM_TIME_CMD:
                        {
                            LOG_INF(" ********** SET_SYSTEM_TIME_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_SYSTEM_TIME_CMD;
                                cJSON *sys_epoch = cJSON_GetObjectItem(properties, JSON_SYSTEM_EPOCH_KEY);

                                if (sys_epoch && cJSON_IsNumber(sys_epoch))
                                {
                                    System_tm_t sys_tm = {0U};
                                    sys_tm.sys_epoch = sys_epoch->valuedouble;
                                    (void)memcpy(json_msgData.data, &sys_tm, sizeof(System_tm_t));
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case SET_SCHEDULE_CONFIG_CMD:
                        {
                            LOG_INF(" ********** SET_SCHEDULE_CONFIG_CMD **********");

                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_SCHEDULE_CONFIG_CMD;

                                cJSON *is_schedule = cJSON_GetObjectItem(properties, JSON_IS_SCHEDULE_KEY);
                                cJSON *wd_start_tm = cJSON_GetObjectItem(properties, JSON_WD_START_TM_KEY);
                                cJSON *wd_end_tm = cJSON_GetObjectItem(properties, JSON_WD_END_TM_KEY);
                                cJSON *we_start_tm = cJSON_GetObjectItem(properties, JSON_WE_START_TM_KEY);
                                cJSON *we_end_tm = cJSON_GetObjectItem(properties, JSON_WE_END_TM_KEY);
                                cJSON *sch_curr_limit = cJSON_GetObjectItem(properties, JSON_SCH_CURR_LIMIT_KEY);

                                if ((is_schedule && cJSON_IsNumber(is_schedule)) &&
                                    (wd_start_tm && cJSON_IsNumber(wd_start_tm)) &&
                                    (wd_end_tm && cJSON_IsNumber(wd_end_tm)) &&
                                    (we_start_tm && cJSON_IsNumber(we_start_tm)) &&
                                    (we_end_tm && cJSON_IsNumber(we_end_tm)) &&
                                    sch_curr_limit && cJSON_IsNumber(sch_curr_limit))
                                {
                                    app_scheduler_t app_scheduler = {0U};

                                    app_scheduler.is_schedule = is_schedule->valueint;
                                    app_scheduler.wd_Starttm = wd_start_tm->valueint;
                                    app_scheduler.wd_Endtm = wd_end_tm->valueint;
                                    app_scheduler.we_Starttm = we_start_tm->valueint;
                                    app_scheduler.we_Endtm = we_end_tm->valueint;
                                    app_scheduler.sch_current_limit = sch_curr_limit->valueint;

                                    if (app_scheduler.sch_current_limit >= MINIMUM_CURRENT_SUPPORT && app_scheduler.sch_current_limit <= MAXIMUM_CURRENT_SUPPORT)
                                    {
                                        LOG_INF("=================%d  %d  %d  %d  %d  %d",
                                                app_scheduler.is_schedule,
                                                app_scheduler.wd_Starttm,
                                                app_scheduler.wd_Endtm,
                                                app_scheduler.we_Starttm,
                                                app_scheduler.we_Endtm,
                                                app_scheduler.sch_current_limit);

                                        (void)memcpy(json_msgData.data, &app_scheduler, sizeof(app_scheduler_t));
                                        LOG_INF("------------- %s", (char *)json_msgData.data);
                                        status = VALID_JSON_PAYLOAD;
                                    }
                                    else
                                    {
                                        status = INVALID_PAYLOAD_DATA;
                                    }
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;
                        case SET_BLE_CONFIG_CMD:
                        {
                            LOG_INF(" ********** SET_BLE_CONFIG_CMD **********");
                        }
                        break;
#if 0
//TODO:Enable after OTA command add
                        case OTA_UPDATE_CMD:
                        {
                            LOG_INF( " ********** OTA_UPDATE_CMD **********");

                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = OTA_UPDATE_CMD;
                                cJSON *file_name = cJSON_GetObjectItem(properties, JSON_FILE_NAME_KEY);
                                cJSON *downloadURL = cJSON_GetObjectItem(properties, JSON_OTA_URL_KEY);

                                if ((file_name && cJSON_IsString(file_name)) &&
                                    (downloadURL && cJSON_IsString(downloadURL)))
                                {
                                    OTACmdPayload_t otaData = {0U};
                                    uint16_t len;
                                    status = INVALID_PAYLOAD_DATA;

                                    len = strlen(file_name->valuestring);
                                    if ((len > 0) && (len <= FILE_NAME_LEN))
                                    {
                                        (void)strncpy((char *)otaData.file_name, file_name->valuestring, len);
                                        len = strlen(downloadURL->valuestring);
                                        if ((len > 0) && (len <= MAX_URL_LEN))
                                        {
                                            (void)strncpy((char *)otaData.url, downloadURL->valuestring, len);
                                            // if (true == DecodeOTAfirmwareVer(otaData.file_name, otaData.fwVersion))
                                            // {
                                            //     status = VALID_JSON_PAYLOAD;
                                            // }
                                            strncpy((char *)otaData.file_name, "OCPP_Proto.bin", 16);
                                            otaData.otaFileSize = 1061152;
                                            app_vOTAinit(otaData);
                                        }
                                    }
                                }
                            }
                        }

                        break;
#endif
                        case GET_LOCATION_INFO_CMD:
                        {
                            LOG_INF(" ********** GET_LOCATION_INFO_CMD **********");
                        }
                        break;

                        case GET_FAULT_INFO_CMD:
                        {
                            LOG_INF(" ********** GET_FAULT_INFO_CMD **********");
                        }
                        break;

                        case GET_ALERT_INFO_CMD:
                        {
                            LOG_INF(" ********** GET_ALERT_INFO_CMD **********");
                        }
                        break;

                        case GET_CONNECTION_STATUS_CMD:
                        {
                            LOG_INF(" ********** GET_CONNECTION_STATUS_CMD **********");
                        }
                        break;

                        case FACTORY_RESET_CMD:
                        {
                            LOG_INF(" ********** FACTORY_RESET_CMD **********");
                        }
                        break;

                        case GET_HEARTBEAT_CMD:
                        {
                            LOG_INF(" ********** GET_HEARTBEAT_CMD **********");
                            json_msgData.msg_id = GET_HEARTBEAT_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case SET_CURRENT_LIMIT_CMD:
                        {
                            LOG_INF(" ********** SET_CURRENT_LIMIT_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_CURRENT_LIMIT_CMD;
                                cJSON *max_current = cJSON_GetObjectItem(properties, JSON_MAX_CURRENT_KEY);

                                if (max_current && cJSON_IsNumber(max_current))
                                {
                                    Max_Current_Limit = max_current->valueint;
                                    if (Max_Current_Limit >= MINIMUM_CURRENT_SUPPORT && Max_Current_Limit <= MAXIMUM_CURRENT_SUPPORT)
                                    {
                                        status = VALID_JSON_PAYLOAD;
                                    }
                                    else
                                    {
                                        status = INVALID_PAYLOAD_DATA;
                                    }
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case SET_CHARGING_START_STOP_CMD:
                        {
                            // LOG_INF(" ********** SET_CHARGING_START_STOP_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_CHARGING_START_STOP_CMD;
                                cJSON *start_stop_cmd = cJSON_GetObjectItem(properties, JSON_START_STOP_CMD_KEY);
                                cJSON *user = cJSON_GetObjectItem(properties, JSON_USERNAME_KEY);

                                if (start_stop_cmd && cJSON_IsNumber(start_stop_cmd))
                                {
                                    UI_Start_Stop_cmd = start_stop_cmd->valueint;
                                    if (UI_CHARGING_START_CMD == UI_Start_Stop_cmd || UI_CHARGING_STOP_CMD == UI_Start_Stop_cmd)
                                    {
                                        if (UI_CHARGING_STOP_CMD == UI_Start_Stop_cmd)
                                        {
                                            StopCmdRcvd = 1;
                                            memset(stop_user_name, 0, strlen((char *)stop_user_name));
                                            strncpy((char *)stop_user_name, user->valuestring, strlen(user->valuestring));
                                            LOG_INF("StopCmdRcvd %d", StopCmdRcvd);
                                        }
                                        else if (UI_CHARGING_START_CMD == UI_Start_Stop_cmd)
                                        {
                                            strncpy((char *)start_user_name, user->valuestring, strlen(user->valuestring));
                                            StopCmdRcvd = 0;
                                        }
                                        status = VALID_JSON_PAYLOAD;
                                    }
                                    else
                                    {
                                        status = INVALID_PAYLOAD_DATA;
                                    }
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case GET_HISTORY_LOG_CMD:
                        {
                            LOG_INF(" ********** GET_HISTORY_LOG_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = GET_HISTORY_LOG_CMD;
                                cJSON *start_record = cJSON_GetObjectItem(properties, JSON_START_NUMBER_CMD_KEY);
                                cJSON *end_record = cJSON_GetObjectItem(properties, JSON_END_NUMBER_CMD_KEY);

                                if (start_record && cJSON_IsNumber(start_record) &&
                                    end_record && cJSON_IsNumber(end_record))
                                {
                                    ReadHistoryEntry_t record = {0U};
                                    record.start_record = start_record->valuedouble;
                                    record.end_record = end_record->valuedouble;
                                    (void)memcpy(json_msgData.data, &record, sizeof(ReadHistoryEntry_t));
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case GET_HISTORY_EVENT_CMD:
                        {
                            LOG_INF(" ********** GET_HISTORY_EVENT_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = GET_HISTORY_EVENT_CMD;
                                cJSON *start_record = cJSON_GetObjectItem(properties, JSON_START_NUMBER_CMD_KEY);
                                cJSON *end_record = cJSON_GetObjectItem(properties, JSON_END_NUMBER_CMD_KEY);

                                if (start_record && cJSON_IsNumber(start_record) &&
                                    end_record && cJSON_IsNumber(end_record))
                                {
                                    ReadHistoryEntry_t record = {0U};
                                    record.start_record = start_record->valuedouble;
                                    record.end_record = end_record->valuedouble;
                                    (void)memcpy(json_msgData.data, &record, sizeof(ReadHistoryEntry_t));
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case GET_HISTORY_ENERGY_CMD:
                        {
                            LOG_INF(" ********** GET_HISTORY_ENERGY_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;
                            if (NULL != properties)
                            {
                                json_msgData.msg_id = GET_HISTORY_ENERGY_CMD;
                                cJSON *type = cJSON_GetObjectItem(properties, JSON_TYPE_CMD_KEY);

                                if (type && cJSON_IsNumber(type))
                                {
                                    ReadHistoryEntry_t record = {0U};
                                    record.type = type->valueint;
                                    (void)memcpy(json_msgData.data, &record, sizeof(ReadHistoryEntry_t));
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case GET_CURRENT_LIMIT_CMD:
                        {
                            LOG_INF(" ********** GET_CURRENT_LIMIT_CMD **********");
                            json_msgData.msg_id = GET_CURRENT_LIMIT_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case GET_SCHEDULE_CONFIG_CMD:
                        {
                            LOG_INF(" ********** GET_SCHEDULE_CONFIG_CMD **********");
                            json_msgData.msg_id = GET_SCHEDULE_CONFIG_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case GET_AVAILABLE_LOGS_CMD:
                        {
                            LOG_INF(" ********** GET_AVAILABLE_LOGS_CMD **********");
                            json_msgData.msg_id = GET_AVAILABLE_LOGS_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case SET_AUTOMODE_CMD:
                        {
                            LOG_INF(" ********** SET_AUTOMODE_CMD **********");
                            uint8_t AutoModeStatus = 0;
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;

                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_AUTOMODE_CMD;
                                cJSON *automode = cJSON_GetObjectItem(properties, JSON_AUTOMODE_KEY);

                                if (automode && cJSON_IsNumber(automode))
                                {
                                    AutoModeStatus = automode->valueint;
                                    if (AutoModeStatus <= OCPPMODE_ENABLE)
                                    {
                                        (void)memcpy(json_msgData.data, &AutoModeStatus, sizeof(AutoModeStatus));
                                        status = VALID_JSON_PAYLOAD;
                                    }
                                    else
                                    {
                                        status = INVALID_PAYLOAD_DATA;
                                    }
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;

                        case GET_AUTOMODE_CMD:
                        {
                            LOG_INF(" ********** GET_AUTOMODE_CMD **********");
                            json_msgData.msg_id = GET_AUTOMODE_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case GET_ALL_CONFIG_READ_CMD:
                        {
                            LOG_INF(" ********** GET_ALL_CONFIG_READ_CMD **********");
                            json_msgData.msg_id = GET_ALL_CONFIG_READ_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case GET_MAC_READ_CMD:
                        {
                            LOG_INF(" ********** GET_MAC_READ_CMD **********");
                            json_msgData.msg_id = GET_MAC_READ_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;

                        case GET_CHARGING_START_TIME:
                        {
                            LOG_INF(" ********** GET_CHARGING_START_TIME **********");
                            json_msgData.msg_id = GET_CHARGING_START_TIME;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;
                        case GET_FW_VERSION:
                        {
                            LOG_INF(" ********** GET_FW_VERSION **********");
                            json_msgData.msg_id = GET_FW_VERSION;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;
                        case SET_CHARGER_CONFIGURATION:
                        {
                            uint8_t *data;
                            LOG_INF(" ********** SET_CHARGER_CONFIGURATION **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            if (NULL != properties)
                            {
                                json_msgData.msg_id = SET_CHARGER_CONFIGURATION;
                                cJSON *charger_name = cJSON_GetObjectItem(properties, CHARGER_NAME_JSON_KEY);
                                cJSON *charger_conn = cJSON_GetObjectItem(properties, CHARGER_CONNECTOR_JSON_KEY);
                                cJSON *charger_capacity = cJSON_GetObjectItem(properties, CHARGER_CAPACITY_JSON_KEY);
                                cJSON *charger_type = cJSON_GetObjectItem(properties, CHARGER_PHASE_TYPE_JSON_KEY);

                                if (charger_name && charger_conn && charger_capacity && charger_type)
                                {
                                    data = (uint8_t *)cJSON_Print(properties);
                                    cJSON_Minify((char *)data);
                                    LOG_INF("<<<<<<<<<<<<<<<<<<SET_CHARGER_CONFIGURATION_DATA>>>>>>>>>>%s", data);
                                    json_msgData.msg_id = SET_CHARGER_CONFIGURATION;
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;
                        case GET_CHARGER_CONFIGURATION:
                        {
                            LOG_INF(" ********** GET_CHARGER_CONFIGURATION **********");
                            json_msgData.msg_id = GET_CHARGER_CONFIGURATION;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;
#if 0
                        //TODO  : Need to check
                        case SET_UNIT_PRICE_CMD:
                        {
                            LOG_INF( " ********** SET_UNIT_PRICE_CMD **********");
                            cJSON *properties = cJSON_GetObjectItem(root, JSON_PROPERTIES);
                            status = INVALID_JSON_PROPERTIES;
                            if (NULL != properties)
                            {
                                cJSON *unitprice = cJSON_GetObjectItem(properties, JSON_UNITPRICE_KEY);
                                cJSON *saved_price = cJSON_GetObjectItem(properties, JSON_SAVEDPRICE_KEY);
                                cJSON *currency =  cJSON_GetObjectItem(properties, JSON_CURRENCY_KEY);
                                if (unitprice && cJSON_IsNumber(unitprice) && saved_price && cJSON_IsNumber(saved_price) && currency && cJSON_IsNumber(currency))
                                {
                                    nvs_unitprice = unitprice->valueint;
                                    nvs_savedprice = saved_price->valueint;
                                    nvs_currency = currency->valueint;
                                    nvsWriteunitprice(nvs_unitprice);
                                    nvsWritesavedprice(nvs_savedprice);
                                    nvsWritecurrency(nvs_currency);
                                    // (void)memcpy(json_msgData.data, &AutoModeStatus, sizeof(AutoModeStatus));
                                    json_msgData.msg_id = SET_UNIT_PRICE_CMD;
                                    status = VALID_JSON_PAYLOAD;
                                }
                                else
                                {
                                    status = INVALID_PAYLOAD_DATA;
                                }
                            }
                        }
                        break;
#endif
                        case GET_UNIT_PRICE_CMD:
                        {
                            LOG_INF(" ********** GET_UNIT_PRICE_CMD **********");
                            json_msgData.msg_id = GET_UNIT_PRICE_CMD;
                            status = VALID_JSON_PAYLOAD;
                        }
                        break;
                        default: /* Default case execute */
                        {
                            LOG_ERR("Message id mismatched with existing support, msgId: 0x%x", json_msgData.msg_id);
                        }
                        break;
                        }
                    }
                }
            }
            else
            {
                status = INVALID_JSON_PROPERTIES;
            }

            if (VALID_JSON_PAYLOAD == status)
            {
                if (k_msgq_put(&uiMsgQueue, &json_msgData, K_MSEC(10)) != 0)
                {
                    /* message queue is full: purge old data & try again */
                    LOG_ERR("[UiMsgQ] Failed to send message in UI Queue ");
                    k_msgq_purge(&uiMsgQueue);
                }
                else
                {
                    LOG_INF("[UiMsgQ] Message sent successfully");
                }
            }
            cJSON_Delete(root);
        }
        else
        {
            status = JSON_ROOT_ERROR;
        }
    }

    if (VALID_JSON_PAYLOAD != status)
    {
        sendStatus(messageID, status);
    }
    LOG_INF(" ********** status %u **********", status);

    return status;
}

/**
 * @brief: validate device mac address
 */
bool isValidMacAddress(uint8_t *data, bool *err)
{
    bool ret = false;
    uint8_t macStr[13U] = {0U};
    uint8_t macId[6U] = {0U};
    // TODO: Add logic for read NRF mac address
#if 0
    if (NULL != data)
    {
        if (NRF_OK != esp_read_mac(macId, ESP_MAC_BT))
        {
            LOG_ERR( "Failed to get wifi mac adress ");
        }
        else
        {
            sprintf((char *)macStr, "%02x%02x%02x%02x%02x%02x", macId[0U], macId[1U], macId[2U],
                    macId[3U], macId[4U], macId[5U]);

            if (0U != strcmp((char *)macStr, (char *)data))
            {
                *err = true;
            }
            else
            {
                ret = true;
            }
        }
    }
    LOG_INF( " *************** isValidMacAddress: %u ", ret);
#endif
    return ret;
}

/**
 * @brief: Validate message identifier
 */
static bool isValidMessageID(int32_t messageID, bool *err)
{
    bool ret = false;
    if ((messageID >= SET_SYSTEM_TIME_CMD) && (messageID < INVALID_CMD_INDEX))
    {
        ret = true;
    }
    else
    {
        *err = true;
    }
    LOG_INF(" *************** isValidMessageID: %u ", ret);

    return ret;
}

/**
 * @brief: Decode firmware version from the string
 */
bool DecodeOTAfirmwareVer(uint8_t *fileName, uint8_t *version)
{
    bool ret = true;

    if ((NULL != fileName) && (NULL != version))
    {
        uint8_t string[20U] = {0U};
        int8_t *token = (int8_t *)strtok((char *)fileName, "-");
        int8_t *lastDash = 0U;
        uint8_t offset = 0U;

        /** tokenize string based on dash */
        while (token != NULL)
        {
            lastDash = token;
            token = (int8_t *)strtok(NULL, "-");
        }

        (void)memcpy(string, lastDash, strlen((char *)lastDash));
        token = (int8_t *)strtok((char *)string, "bin");
        (void)memcpy(string, token, strlen((char *)token));
        token = (int8_t *)strtok((char *)string, ".");

        while (token != NULL)
        {
            if (offset < 4U)
            {
                version[offset] = strtol((char *)token, NULL, 10U);
                offset++;
            }
            else if (offset >= 4U)
            {
                ret = false;
                break;
            }
            else
            {
                /// Do Nothing
            }
            token = (int8_t *)strtok(NULL, ".");
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

app_err_t sendLatestOTAstatus(uint8_t eStatus, char *dataDiscription)
{
    app_err_t err = NRF_FAIL;

    // cJSON *root = cJSON_CreateObject();
    // if (NULL != root)
    // {
    //     LOG_ERR( "sendLatestOTAstatus start");
    //     cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, OTA_STATUS_NOTIFY_CMD);
    //     //cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
    //     cJSON *properties = cJSON_CreateObject();
    //     if (NULL != properties)
    //     {
    //         uint8_t *data;
    //         cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
    //         cJSON_AddNumberToObject(properties, JSON_OTA_STATUS, eStatus);
    //         cJSON_AddStringToObject(properties, JSON_OTA_DISCRIPTION, dataDiscription);

    //         data = (uint8_t *)cJSON_Print(root);
    //         (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
    //         LOG_INF( "Json String : %s", blemsgData.data);
    //         send_ble_Q(bleMsgQueue, blemsgData);
    //         err = NRF_OK;
    //         free(data);
    //         cJSON_Delete(root);
    //         LOG_ERR( "sendLatestOTAstatus END");
    //     }
    // }
    return err;
}

/**
 * @brief: This function send command status to mobile/cloud
 */
void sendStatus(JSON_Msgid_e msgid, uint8_t status)
{
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, msgid);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            cJSON_AddNumberToObject(properties, JSON_STATUS_KEY, status);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json Status String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
}

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendHeartbeatData(void)
{
    app_err_t err = NRF_OK;
    heartbeat_info_t hb_info = {0U};
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_HEARTBEAT_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetHeartbeatInfo(&hb_info);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                // cJSON_AddNumberToObject(properties, JSON_HB, 1);
                cJSON_AddNumberToObject(properties, JSON_HBTIMESTAMP_KEY, hb_info.sys_tm);
                cJSON_AddNumberToObject(properties, JSON_HBTOTALENERGY_KEY, hb_info.total_energy);
                cJSON_AddNumberToObject(properties, JSON_HBEVCONNECTION_KEY, hb_info.ev_connection_status);
                cJSON_AddNumberToObject(properties, JSON_HBEVSTATE_KEY, hb_info.ev_current_state);
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json heartbeat String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send heartbeat");
        }
        cJSON_Delete(root);
    }
    return err;
}

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendMeteringInfo(void)
{
    app_err_t err = NRF_OK;

    Metering_info_spi_t metering_info = {0U};
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_METERING_INFO_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetSPIMeteringInfo(&metering_info);
        // LOG_INF("metering_info.session_energy %f StartEnergy %f",metering_info.session_energy,StartEnergy);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                cJSON_AddNumberToObject(properties, JSON_VOLTAGE_KEY, metering_info.RPhase_voltage);
                cJSON_AddNumberToObject(properties, JSON_CURRENT_KEY, metering_info.RPhase_current);
                cJSON_AddNumberToObject(properties, JSON_POWER_KEY, metering_info.RPhase_power);
                // cJSON_AddNumberToObject(properties, JSON_SESSION_ENERGY_KEY, (metering_info.session_energy - StartEnergy));
                // TODO :Check and enable above line
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json heartbeat String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send heartbeat");
        }
        cJSON_Delete(root);
    }
    return err;
}

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendFaultInfo(void)
{
    app_err_t err = NRF_OK;
    fault_info_t fault_info = {0U};
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_FAULT_INFO_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetFaultInfo(&fault_info);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                cJSON_AddNumberToObject(properties, JSON_FAULT_KEY, fault_info.err_code);
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json fault String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send heartbeat");
        }
        cJSON_Delete(root);
    }
    return err;
}

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendCurrentLimitData(void)
{
    app_err_t err = NRF_OK;
    heartbeat_info_t hb_info = {0U};
    cJSON *root = cJSON_CreateObject();

    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_CURRENT_LIMIT_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetHeartbeatInfo(&hb_info);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                cJSON_AddNumberToObject(properties, JSON_MAX_CURRENT_KEY, get_max_current_limit());
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json current limit String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send SendCurrentLimitData");
        }
        cJSON_Delete(root);
    }
    return err;
}

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendScheduleData(void)
{
    app_err_t err = NRF_OK;
    app_scheduler_t scheduler_info = {0U};
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_SCHEDULE_CONFIG_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetScheduleInfo(&scheduler_info);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                cJSON_AddNumberToObject(properties, JSON_IS_SCHEDULE_KEY, scheduler_info.is_schedule);
                cJSON_AddNumberToObject(properties, JSON_WD_START_TM_KEY, scheduler_info.wd_Starttm);
                cJSON_AddNumberToObject(properties, JSON_WD_END_TM_KEY, scheduler_info.wd_Endtm);
                cJSON_AddNumberToObject(properties, JSON_WE_START_TM_KEY, scheduler_info.we_Starttm);
                cJSON_AddNumberToObject(properties, JSON_WE_END_TM_KEY, scheduler_info.we_Endtm);
                cJSON_AddNumberToObject(properties, JSON_SCH_CURR_LIMIT_KEY, scheduler_info.sch_current_limit);
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json schedular String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send ScheduleData");
        }
        cJSON_Delete(root);
    }
    return err;
}
/**
 * @brief: This function send mac address data to mobile/cloud
 */
app_err_t SendMACAddress(void)
{
    app_err_t err = NRF_OK;
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_MAC_READ_CMD);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            // TODO :Enable mac if need
            // cJSON_AddStringToObject(properties, JSON_DEVICE_ID_KEY, (char *)dev_mac);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json mac String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
    return err;
}
/**
 * @brief: This function send charging start time to mobile/cloud
 */
app_err_t SendChargingStartTime(void)
{
    app_err_t err = NRF_OK;
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_CHARGING_START_TIME);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            // TODO :Check and enable
            // cJSON_AddNumberToObject(properties, JSON_STARTTM_KEY, StartHistoryLog.timestamp);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json start time String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
    return err;
}
#if 0 // TODO :Need to midify this function
/**
 * @brief: API to Send Diagnose Events to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLast_7DaysEnergy(uint16_t start_record,uint16_t end_record)
{
 	app_err_t err = NRF_FAIL;
    float prev_energy = 0;
    uint8_t rowCount = sqlReadRowCount(ENERGY_DATA_TABLE_NAME);
    LOG_INF("GetLast_7DaysEnergy Row Count %d",rowCount);
    if (rowCount == 0)
    {
        err = ESP_ERR_NVS_NOT_FOUND;
    }
    else
    {
        History_Energydata_t *histroyEvent = (History_Energydata_t *)malloc(rowCount * sizeof(History_Energydata_t));
        err = sqlReadHistoryEnergy(histroyEvent, start_record,end_record);
        if (NRF_OK == err)
        {
            if (NULL != histroyEvent)
            {
                History_Energydata_t *readPtr = histroyEvent;
                cJSON *root = cJSON_CreateObject();
                
                if (NULL != root)
                {
                    uint8_t *data;
                    cJSON *properties;
                    cJSON *array;
                    cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_HISTORY_ENERGY_CMD);
                    //cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
                    cJSON_AddItemToObject(root, JSON_PROPERTIES, properties = cJSON_CreateObject());
                    cJSON_AddNumberToObject(properties, JSON_TYPE_CMD_KEY, LAST_7_DAYS);
                    cJSON_AddItemToObject(properties, JSON_ARRAY_KEY, array = cJSON_CreateArray());
                    if (NULL != properties)
                    {
                        uint8_t noOfEvent;
                        struct timeval tv_now;
                       // char get_current_date[10];
                        char nvsreadptrdate[10];
                        char prevdate[10];
                        char prevweekdate[10];
                        uint64_t current_epoch_get;
                        int prevepoch;
                        gettimeofday(&tv_now, NULL);
                        current_epoch_get = tv_now.tv_sec;
                        epochtimetodate_yyyymmdd(current_epoch_get,prevdate);
                        prevepoch = current_epoch_get;
                        //LOG_INF( "prevepochtodate prevdatestr assigned %d",prevepoch);
                        //LOG_INF( " Current Epoch to date : %s", prevdate); 
                        current_epoch_get = (current_epoch_get - (7*86400)); 
                        epochtimetodate_yyyymmdd(current_epoch_get,prevweekdate);    
                        epochtimetodate_yyyymmdd(readPtr->timestamp,nvsreadptrdate);    
                       // LOG_INF( " Lastweek Date  : %s ", prevweekdate);
                      //  LOG_INF( "nvsreadptr date %s",nvsreadptrdate);
                       // LOG_INF( " Epoch Timestamp in NVS read Readptr : %lld ", readPtr->timestamp);
                        for (noOfEvent = start_record; noOfEvent < end_record; noOfEvent++)
                        {
                            if (readPtr->energy == CHECK_STRUCT_EMPTY &&
                                readPtr->timestamp == CHECK_STRUCT_EMPTY)
                            {
                                LOG_ERR( "Events are Empty");
                                break;
                            }
                            if (strcmp(nvsreadptrdate, prevdate) == 0)
                            {
                                LOG_INF( "compare string no_event row_count %s %s %d %d", nvsreadptrdate, prevdate, noOfEvent, rowCount);
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                        
                                    if (0U != prev_energy)
                                    {
                                        LOG_INF( " energy in if  JSON PARSER prev_energy %f readPtr->energy  %f total %f ", prev_energy,readPtr->energy,prev_energy - readPtr->energy);
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, prev_energy - readPtr->energy);
                                    }
                                    else
                                    {                                       
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                    }
                                }
                                if (1U == noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    LOG_INF( "energy in else  JSON PARSER %f ", readPtr->energy);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                }
                                // if (readPtr->energy != 0U) // Aditya 9 march 2022
                                // {
                                //     prev_energy = readPtr->energy;
                                // }
                                prev_energy = readPtr->energy;
                                readPtr++;
                            }
                            else
                            {
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                                if (1U == noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                            }
                            if (strcmp(prevdate, prevweekdate) == 0)
                            {
                                LOG_ERR( " Prev date and last date is match ");
                                break;
                            }
                            prevepoch = prevepoch - 86400;
                            //LOG_INF( "Epoch time  previous in for loop  %d", prevepoch);
                            epochtimetodate_yyyymmdd(prevepoch, prevdate);
                            //LOG_INF( "Epoch time %lld", readPtr->timestamp);
                            epochtimetodate_yyyymmdd(readPtr->timestamp, nvsreadptrdate);
                           // LOG_INF( " previous date %s", prevdate);
                            //LOG_INF( "nvsreadptr date %s",nvsreadptrdate);
                            // LOG_INF( "JSON Parser epoch time to date time %s",epochtimetodate);
                            // cJSON_AddNumberToObject(properties_obj, JSON_ID_KEY, readPtr->event_id);
                            // cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                            LOG_INF( "nvs readptr energy %f", readPtr->energy);
                            LOG_INF( "prev_energy %f", prev_energy);
                        }
                        data = (uint8_t *)cJSON_Print(root);
                        cJSON_Minify((char *)data);
                        LOG_INF( "Json Status String : %s", data);
                        (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                        err = send_ble_Q(bleMsgQueue, blemsgData);
                        if (NRF_OK != err)
                        {
                            LOG_ERR( "Failed to send Data On Cloud And Ble");
                        }
                        free(data);
                    }
                }
                cJSON_Delete(root);
            }
            free(histroyEvent);
        }
    }
    return err;    
 }
#endif
#if 0 // TODO :Need to midify this function
/**
 * @brief: API to Send Diagnose Events to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLast_30DaysEnergy(uint16_t start_record, uint16_t end_record)
{
	 app_err_t err = NRF_FAIL;
    float prev_energy = 0U;
    uint8_t rowCount = sqlReadRowCount(ENERGY_DATA_TABLE_NAME);
    LOG_INF("GetLast_30DaysEnergy Row Count %d",rowCount);
    if (rowCount == 0U)
    {
        err = ESP_ERR_NVS_NOT_FOUND;
    }
    else
    {
        History_Energydata_t *histroyEvent = (History_Energydata_t *)malloc(rowCount * sizeof(History_Energydata_t));
        // if (rowCount <= MONTH_END_RECORD)//end_record > rowCount && 
        // {
        //     end_record = rowCount;
        // }
        err = sqlReadHistoryEnergy(histroyEvent, start_record, end_record);
        if (NRF_OK == err)
        {
            if (NULL != histroyEvent)
            {
                History_Energydata_t *readPtr = histroyEvent;
                cJSON *root = cJSON_CreateObject();
                if (NULL != root)
                {
                    uint8_t *data;
                    cJSON *properties;
                    cJSON *array;
                        struct timeval tv_now;
                       // char get_current_date[10];
                        char nvsreadptrdate[10];
                        char prevdate[10];  
                        char prevmonthdate[10];
                        uint64_t current_epoch_get;
                        int prevepoch;
                        gettimeofday(&tv_now, NULL);
                        current_epoch_get = tv_now.tv_sec;
                        epochtimetodate_yyyymmdd(current_epoch_get,prevdate);
                        prevepoch = current_epoch_get;
                       // LOG_INF( "prevepochtodate prevdatestr assigned %d",prevepoch);
                        //LOG_INF( " Current Epoch to date : %s", prevdate); 
                        current_epoch_get = (current_epoch_get - (30*86400)); 
                        epochtimetodate_yyyymmdd(current_epoch_get,prevmonthdate);    
                        epochtimetodate_yyyymmdd(readPtr->timestamp,nvsreadptrdate);
#if 0
                        LOG_INF( " Prev month Date  : %s ", prevmonthdate);
                        LOG_INF( "nvsreadptr date %s",nvsreadptrdate);
                        LOG_INF( " Epoch Timestamp in NVS read Readptr : %lld ", readPtr->timestamp);
#endif

                    cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_HISTORY_ENERGY_CMD);
                    //cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
                    cJSON_AddItemToObject(root, JSON_PROPERTIES, properties = cJSON_CreateObject());
                    cJSON_AddNumberToObject(properties, JSON_TYPE_CMD_KEY, LAST_30_DAYS);
                    cJSON_AddItemToObject(properties, JSON_ARRAY_KEY, array = cJSON_CreateArray());
                    if (NULL != properties)
                    {
                        uint8_t noOfEvent;
                        for (noOfEvent = start_record; noOfEvent < end_record; noOfEvent++)
                        {
                            if (readPtr->energy == CHECK_STRUCT_EMPTY &&
                                readPtr->timestamp == CHECK_STRUCT_EMPTY)
                            {
                                LOG_ERR( "Events are Empty");
                                break;
                            }
                            if (strcmp(nvsreadptrdate, prevdate) == 0)
                            {
                                LOG_INF( "compare string no_event row_count %s %s %d %d", nvsreadptrdate, prevdate, noOfEvent, rowCount);
                                // cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    if (0U != prev_energy)
                                    {
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, prev_energy - readPtr->energy);
                                    }
                                    else
                                    {
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                    }
                                }
                                if (1U == noOfEvent)
                                {
                                     LOG_INF( " in if 1U == noOfEvent %f",readPtr->energy);
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                   
                                }
                                // if (readPtr->energy != 0U) // Aditya 9 March 2022
                                // {
                                //     prev_energy = readPtr->energy;
                                // }
                            prev_energy = readPtr->energy;
                            readPtr++;
                            }
                            else
                            {
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                                if (1U == noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                                 LOG_INF( "in else  end record No of event %d %d", end_record,noOfEvent);
                               
                            }
                            if (strcmp(prevdate, prevmonthdate) == 0)
                            {
                                LOG_ERR( " Prev date and last month date is match ");
                                //break;
                            }
                            prevepoch = prevepoch - 86400;
                           // LOG_INF( "Epoch time  previous in for loop  %d",prevepoch);
                            epochtimetodate_yyyymmdd(prevepoch,prevdate);
                           // LOG_INF( "Epoch time readtimestamp from nvs %lld",readPtr->timestamp);
                            epochtimetodate_yyyymmdd(readPtr->timestamp,nvsreadptrdate);
#if 0
                            LOG_INF( " previous date %s",prevdate);
                            LOG_INF( "date nvs readtimestamp %s",nvsreadptrdate);
                            LOG_INF( "Nvs last 30days energy %d",readPtr->energy);
#endif
                            //cJSON_AddNumberToObject(properties_obj, JSON_ID_KEY, readPtr->event_id);    
                            //cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                            //LOG_INF("prev_energy %lld",prev_energy);
                        }
                        data = (uint8_t *)cJSON_Print(root);
                        cJSON_Minify((char *)data);
                        LOG_INF( "Json Status String : %s", data);
                        (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                        err = send_ble_Q(bleMsgQueue, blemsgData);
                        if (NRF_OK != err)
                        {
                            LOG_ERR( "Failed to send Data On Cloud And Ble");
                        }
                        free(data);
                    }
                }
                cJSON_Delete(root);
            }
            free(histroyEvent);
        }
    }
    return err;   
}
#endif
#if 0 // TODO :Need to midify this function
/**
 * @brief: API to Send Diagnose Events to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetLastYearEnergy(uint16_t start_record, uint16_t end_record)
{
    app_err_t err = NRF_FAIL;
    float prev_energy = 0U;
    uint8_t rowCount = sqlReadRowCount(MONTHLY_ENERGY_DATA_TABLE_NAME);
    if (rowCount == 0U)
    {
        err = ESP_ERR_NVS_NOT_FOUND;
    }
    else
    {
        History_Energydata_t *histroyEvent = (History_Energydata_t *)malloc(rowCount * sizeof(History_Energydata_t));
        err = sqlReadMonthlyHistoryEnergy(histroyEvent, start_record, end_record);
        if (NRF_OK == err)
        {
            if (NULL != histroyEvent)
            {
                History_Energydata_t *readPtr = histroyEvent;
                cJSON *root = cJSON_CreateObject();
                if (NULL != root)
                {
                    uint8_t *data;
                    cJSON *properties;
                    cJSON *array;
                    struct timeval tv_now;
                    char nvsreadptrdate[10];
                    char prevdate[10];
                    char prevyeardate[10];
                    uint64_t current_epoch_get;
                    uint32_t prevmonth;
                    uint32_t prevyearmonth;
                    uint32_t nvsreadmonth;
                    int prevepoch;
                    gettimeofday(&tv_now, NULL);
                    current_epoch_get = tv_now.tv_sec;
                    epochtimetodate_yyyymmdd(current_epoch_get, prevdate);
                    LOG_INF( " Current Epoch : %s", prevdate);
                    prevmonth = atoi((const char *)prevdate);
                    // LOG_INF( " Current Epoch prevmonth atoi: %d", prevmonth);
                    prevmonth = (prevmonth % 10000) / 100;
                    prevepoch = current_epoch_get;
                    LOG_INF( " Current Epoch to Month : %d", prevmonth);
                    current_epoch_get = (current_epoch_get - (365 * 86400));
                    epochtimetodate_yyyymmdd(current_epoch_get, prevyeardate);
                    epochtimetodate_yyyymmdd(readPtr->timestamp, nvsreadptrdate);
                    prevyearmonth = atoi(prevyeardate);
                    prevyearmonth = (prevyearmonth % 10000) / 100;
                    nvsreadmonth  = atoi(nvsreadptrdate);
                    nvsreadmonth = (nvsreadmonth % 10000) / 100;
#if 0
                    LOG_INF("PREVMONTHYYYYMM AND PREVYEARMONTHYYYYMM %d %d",prevmonthyyyymm,prevyearmonthyyyymm);
                    LOG_INF( " Prev Year Month  : %d", prevyearmonth);
                    LOG_INF( " nvsreadptr Month %d", nvsreadmonth);
                    LOG_INF( "  NVS  Readptr Energy : %d ", readPtr->energy);
#endif
                    cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_HISTORY_ENERGY_CMD);
                    // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
                    cJSON_AddItemToObject(root, JSON_PROPERTIES, properties = cJSON_CreateObject());
                    cJSON_AddNumberToObject(properties, JSON_TYPE_CMD_KEY, LAST_YEAR);
                    cJSON_AddItemToObject(properties, JSON_ARRAY_KEY, array = cJSON_CreateArray());
                    LOG_INF( "------> LASTYEAR row_count : %d", rowCount);
                    if (NULL != properties)
                    {
                        uint8_t noOfEvent;
                        for (noOfEvent = start_record; noOfEvent < end_record; noOfEvent++)
                        {
                            if (readPtr->energy == CHECK_STRUCT_EMPTY &&
                                readPtr->timestamp == CHECK_STRUCT_EMPTY)
                            {
                                LOG_ERR( "Events are Empty");
                                break;
                            }
                            if (nvsreadmonth == prevmonth)
                            {
                                LOG_INF( "compare string no_event row_count %s %s %d %d", nvsreadptrdate, prevdate, noOfEvent, rowCount);
                                // cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    if (0 != prev_energy)
                                    {
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, prev_energy - readPtr->energy);
                                    }
                                    else
                                    {
                                        cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                    }
                                }
                                if (1U == noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, readPtr->energy);
                                }
                                // if (readPtr->energy != 0U) // Aditya 9 March 2022
                                // {
                                //     prev_energy = readPtr->energy;
                                // }
                                 prev_energy = readPtr->energy;
                                 readPtr++;
                            }
                            else
                            {
                                if (1U != noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                                if (1U == noOfEvent)
                                {
                                    cJSON *properties_obj = cJSON_CreateObject();
                                    cJSON_AddItemToArray(array, properties_obj);
                                    cJSON_AddNumberToObject(properties_obj, JSON_HISTORY_ENERGY_KEY, 0);
                                }
                                
                            }
                           
                            // if (prevmonthyyyymm == prevyearmonthyyyymm)
                            // {
                            //     LOG_ERR( " Prev month and last year month is match ");
                            //     break;
                            // }
                            prevepoch = prevepoch - (30*86400);   // 30 for last month
                            LOG_INF( "Epoch time  previous in for loop  %d", prevepoch);
                            epochtimetodate_yyyymmdd(prevepoch, prevdate);
                            prevmonth = atoi(prevdate);
                            prevmonth = (prevmonth % 10000) / 100;
                            epochtimetodate_yyyymmdd(readPtr->timestamp, nvsreadptrdate);
                            nvsreadmonth  = atoi(nvsreadptrdate);
                            nvsreadmonth = (nvsreadmonth % 10000) / 100;
#if 0
                            LOG_INF( " previous Month %d", prevmonth);
                            LOG_INF( " nvsreadptr Month  %d", nvsreadmonth);
                            // cJSON_AddNumberToObject(properties_obj, JSON_ID_KEY, readPtr->event_id);
                            // cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                             LOG_INF("prev_energy %lld",prev_energy);
#endif
                            
                        }
                        data = (uint8_t *)cJSON_Print(root);
                        cJSON_Minify((char *)data);
                        LOG_INF( "Json Status String : %s", data);
                        (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                        err = send_ble_Q(bleMsgQueue, blemsgData);
                        if (NRF_OK != err)
                        {
                            LOG_ERR( "Failed to send Data On Cloud And Ble");
                        }
                        free(data);
                    }
                }
                cJSON_Delete(root);
            }
            free(histroyEvent);
        }
    }
    return err;
}
#endif
#if 0 // TODO :Need to midify this function
/**
 * @brief: API to Send History Logs to Mobile and Cloud
 *
 * @param[in] void
 * @param[out] app_err_t    Return NRF_OK or NRF_FAIL
 */
app_err_t GetHistoryLogs(uint16_t start_record,uint16_t end_record)
{
    LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@GetHistoryLogs");
        
    app_err_t err = NRF_FAIL;
    uint8_t rowCount = sqlReadRowCount(LOG_DATA_TABLE_NAME);
    if (rowCount == 0)
    {
        err = ESP_ERR_NVS_NOT_FOUND;
        LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@rowCount");
      
    }
    else if ((start_record > rowCount) || (end_record > rowCount))
    {
        err = ESP_ERR_NVS_NOT_FOUND;
     LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ESP_ERR_NVS_NOT_FOUND");
   
    }
    else
    {
           LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@else");
   
        History_Logdata_t *histroyLog = (History_Logdata_t *)malloc(rowCount * sizeof(History_Logdata_t));
        err = sqlReadHistoryLogs(histroyLog, start_record,end_record);
        if (NRF_OK == err)
        {
        LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  if (NRF_OK == err)");
   
            if (NULL != histroyLog)
            {
                         LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ if (NULL != histroyLog)");
   
                History_Logdata_t *readPtr = histroyLog;
                cJSON *root = cJSON_CreateObject();
                if (NULL != root)
                {
                LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@(NULL != root)");
   
                    uint8_t *data;
                    cJSON *properties;
                    cJSON *array;
                    cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, 10);
                    //cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
                    cJSON_AddItemToObject(root, JSON_PROPERTIES, properties = cJSON_CreateObject());
                    cJSON_AddItemToObject(properties, JSON_ARRAY_KEY, array = cJSON_CreateArray());
                    if (NULL != properties)
                    {
                        LOG_ERR( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@NULL != properties)");
   
                        uint8_t noOfDiagnoseEvent;
                        for (noOfDiagnoseEvent = start_record; noOfDiagnoseEvent <= end_record; noOfDiagnoseEvent++)
                        {
                            if (readPtr->event_code == CHECK_STRUCT_EMPTY &&
                                readPtr->timestamp == CHECK_STRUCT_EMPTY)
                            {
                                LOG_ERR( "Events are Empty");
                                break;
                            }
                            cJSON *properties_obj = cJSON_CreateObject();
                            cJSON_AddItemToArray(array, properties_obj);
                            cJSON_AddNumberToObject(properties_obj, JSON_ID_KEY, readPtr->event_id);
                            cJSON_AddNumberToObject(properties_obj, JSON_EVENT_KEY, readPtr->event_code);
                            cJSON_AddNumberToObject(properties_obj, JSON_TIMESTAMP_KEY, readPtr->timestamp);
                            cJSON_AddNumberToObject(properties_obj, JSON_TIME_DURATION_KEY, readPtr->time_duration);
                            cJSON_AddNumberToObject(properties_obj, JSON_SESSION_ENERGY_KEY, readPtr->energy);
                            cJSON_AddNumberToObject(properties_obj, JSON_CHARGING_MODE, readPtr->mode);
                            cJSON_AddStringToObject(properties_obj, JSON_START_USER_NAME,readPtr->start_name);
                            // cJSON_AddStringToObject(properties_obj, JSON_STOP_USER_NAME,readPtr->stop_name);
                            readPtr++;
                        }

                        data = (uint8_t *)cJSON_Print(root);
                        cJSON_Minify((char *)data);
                        LOG_INF( "Json_Status_String_GetHistoryLogs: %s", data);
                        (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                        send_ble_Q(bleMsgQueue, blemsgData);
                        if (NRF_OK != err)
                        {
                            LOG_ERR( "Failed to send Data On Cloud And Ble");
                        }
                        free(data);
                    }
                }
                cJSON_Delete(root);
            }
            free(histroyLog);
        }
    }
    return err;
}
#endif

/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t SendLogCount(void)
{
    app_err_t err = NRF_OK;
#if 0 // TODO :Need to midify this function
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_AVAILABLE_LOGS_CMD);
        //cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        uint8_t rowCount = sqlReadRowCount(LOG_DATA_TABLE_NAME);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            cJSON_AddNumberToObject(properties, JSON_AVAILABLE_LOG, rowCount);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF( "Json schedular String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
#endif
    return err;
}

/**
 * @brief: This function send automode status to mobile/cloud
 */
app_err_t SendAutModeStatus(void)
{
    app_err_t err = NRF_OK;
    uint8_t AutoMode_Status = 0;
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_AUTOMODE_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        GetAutoModeStatus(&AutoMode_Status);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            cJSON_AddNumberToObject(properties, JSON_AUTOMODE_KEY, AutoMode_Status);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
    return err;
}

uint8_t get_uiStartStop_cmd(void)
{
    return UI_Start_Stop_cmd;
}

void resetUIStartCmd(void)
{
    UI_Start_Stop_cmd = 0;
}

uint8_t get_max_current_limit(void)
{
    app_scheduler_t app_scheduler = {0U};
    GetScheduleInfo(&app_scheduler);
    Max_Current_Limit = app_scheduler.sch_current_limit;
    // LOG_ERR( "Max_Current_Limit %d",Max_Current_Limit);

    if (Max_Current_Limit < MINIMUM_CURRENT_SUPPORT)
    {
        Max_Current_Limit = MINIMUM_CURRENT_SUPPORT;
    }
    return Max_Current_Limit;
}

app_err_t send_ble_Q(struct k_msgq QHandler, blemsgQueueData_t msgData1)
{
    app_err_t err = NRF_OK;
    if (k_msgq_put(&QHandler, &msgData1, K_MSEC(10)) != 0)
    {
        /* message queue is full: purge old data & try again */
        LOG_ERR("Failed to send message in tcp Queue %d", err);
        k_msgq_purge(&QHandler);
    }
    return err;
}

app_err_t sendfwversion(void)
{
    app_err_t err = NRF_OK;
    uint8_t fw_version[20] = {0U};

    // TODO :Copy actual fw version here
    // strcpy((char *)fw_version, (char *)app_desc->version);
    LOG_INF("************ FW VERSION : %s ************", fw_version);

    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_FW_VERSION);
        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            cJSON_AddStringToObject(properties, JSON_FW_VERSION_KEY, (char *)fw_version);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json Fw Version String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
    return err;
}
app_err_t sendunitprice(void)
{

    app_err_t err = NRF_OK;
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, GET_UNIT_PRICE_CMD);

        cJSON *properties = cJSON_CreateObject();
        if (NULL != properties)
        {
            uint8_t *data;
            cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
            // TODO :need to check
            // cJSON_AddNumberToObject(properties, JSON_UNITPRICE_KEY, nvs_unitprice);
            // cJSON_AddNumberToObject(properties, JSON_SAVEDPRICE_KEY, nvs_savedprice);
            // cJSON_AddNumberToObject(properties, JSON_CURRENCY_KEY, nvs_currency);
            data = (uint8_t *)cJSON_Print(root);
            cJSON_Minify((char *)data);
            LOG_INF("Json_send_unitprice_String : %s", data);
            (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
            send_ble_Q(bleMsgQueue, blemsgData);
            free(data);
        }
        cJSON_Delete(root);
    }
    return err;
}
#if 0
app_err_t RF_WiFiCredentialUpdate(wifiConfigData_t * wifiInfo)
{
    app_err_t ret = NRF_FAIL;
    wifi_config_t wifi_config = {0U};

    if (NULL != wifiInfo)
    {
        (void) memcpy(wifi_config.sta.ssid, wifiInfo->ssid, sizeof(wifiInfo->ssid));
        (void) memcpy(wifi_config.sta.password, wifiInfo->passphrase, sizeof(wifiInfo->passphrase));

        ret = esp_wifi_disconnect();
        if (NRF_OK != ret)
        {
            LOG_ERR( "Failed to disconnect wifi, ret = %d",ret);
        }
        else
        {
            ret = esp_wifi_stop();
            if (NRF_OK != ret)
            {
                LOG_ERR( "Failed to wifi stop, ret = %d",ret);
            }
            else
            {
                /* Configure WiFi as station */
                ret = esp_wifi_set_mode(WIFI_MODE_STA);
                if (NRF_OK != ret)
                {
                   LOG_ERR( "Failed to set WiFi mode, ret = %d",ret);
                }
                else
                {
                    ret = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
                    if (NRF_OK != ret)
                    {
                        LOG_ERR( "Failed to set WiFi configuration, ret = %d",ret);
                    }
                    else
                    {
                        LOG_INF("WiFi Credential updated successfully");
                        LOG_INF("WiFi SSID: %s ",wifi_config.sta.ssid);
                        LOG_INF("WiFi PASS: %s ",wifi_config.sta.password);

                        /* Start WiFi */
                        ret = esp_wifi_start();
                        if (NRF_OK != ret)
                        {
                           LOG_ERR( "Failed to set WiFi configuration, ret= %d",ret);
                        }
                        else
                        {
                            /* Connect to AP */
                            ret = esp_wifi_connect();
                            if (NRF_OK != ret)
                            {
                               LOG_ERR( "Failed to connect WiFi, ret= %d",ret);
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}
#endif
// app_err_t send_wifi_credentials(void)
// {

// }
/**
 * @brief: This function send heartbeat data to mobile/cloud
 */
app_err_t Heartbeatsend(void)
{
    app_err_t err = NRF_OK;
    heartbeat_info_t hb_info = {0U};
    cJSON *root = cJSON_CreateObject();
    if (NULL != root)
    {
        cJSON_AddNumberToObject(root, JSON_MSG_ID_KEY, SET_HEARTBEAT_CMD);
        // cJSON_AddStringToObject(root, JSON_DEVICE_ID_KEY, (char *)dev_mac);
        err = GetHeartbeatInfo(&hb_info);
        if (NRF_OK == err)
        {
            cJSON *properties = cJSON_CreateObject();
            if (NULL != properties)
            {
                uint8_t *data;
                cJSON_AddItemToObject(root, JSON_PROPERTIES, properties);
                cJSON_AddNumberToObject(properties, JSON_HB, 1);
                //  cJSON_AddNumberToObject(properties, JSON_HBTIMESTAMP_KEY, hb_info.sys_tm);
                // cJSON_AddNumberToObject(properties, JSON_HBTOTALENERGY_KEY, hb_info.total_energy);
                // cJSON_AddNumberToObject(properties, JSON_HBEVCONNECTION_KEY, hb_info.ev_connection_status);
                // cJSON_AddNumberToObject(properties, JSON_HBEVSTATE_KEY, hb_info.ev_current_state);
                data = (uint8_t *)cJSON_Print(root);
                cJSON_Minify((char *)data);
                LOG_INF("Json heartbeat String : %s", data);
                (void)memcpy(blemsgData.data, data, sizeof(blemsgData.data));
                send_ble_Q(bleMsgQueue, blemsgData);
                free(data);
            }
        }
        else
        {
            LOG_ERR("Failed to send heartbeat");
        }
        cJSON_Delete(root);
    }
    return err;
}

/****************************************************************************
 * END OF FILE
 ****************************************************************************/