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
/** \file app_ui.c
 * \brief
 *
 **/
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
// #include <zephyr/sys/__assert.h>
#include "app_ui.h"
#include "components/rtc_mcp79400/gw_rtc.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/app_schedule/app_schedule.h"
#include "components/app_db/app_db.h"
#include "components/meter_ade7593/ade7593.h"
// #include "cp_pwm.h"
#include "zephyr/logging/log.h"
#include "components/ioExp_mcp23s17/ioExp_mcp23s17.h"
#include "zephyr/kernel.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define TAG "UI_TASK"
#define UI_TASK_NAME "UI task"
#define UI_TASK_STACK_SIZE (6 * 1024) ///< Define Task Stack Size
#define DEFAULT_UI_BITMASK (0x00000000U)
#define UI_TASK_PRIORITY 7

#define UI_Q_LEN (5U)
#define UI_Q_ITEM_SIZE (sizeof(uimsgQueueData_t))

LOG_MODULE_REGISTER(appUI, LOG_LEVEL_DBG);
/****************************************************************************
 * TYPES
 ****************************************************************************/

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
extern uint16_t Max_Current_Limit;
extern uint8_t schedule_charging_status;
// extern ev_error_t ev_error_info[];
uimsgQueueData_t uimsgData;
struct k_msgq uiMsgQueue;
struct k_msgq bleMsgQueue;
uint8_t read_energy_flag;
uint8_t emergencySwitchPressed;
uint8_t earthFaultDetect;
extern uint8_t StopCmdRcvd;
char __aligned(4) uiMsgQueueBuffer[UI_Q_LEN * UI_Q_ITEM_SIZE];
char __aligned(4) bleMsgQueueBuffer[BLE_Q_LEN * BLE_Q_ITEM_SIZE];
/**
 * @brief: This is UI task handler
 */
void app_vUITask(void *arg);
static struct k_thread uiTaskHandler;
static K_KERNEL_STACK_DEFINE(ui_task_stack, UI_TASK_STACK_SIZE);
// extern const k_tid_t uiTaskHandler;/**< Reference to schedule FreeRTOS task. */
// K_THREAD_DEFINE(uiTaskHandler, UI_TASK_STACK_SIZE, app_vUITask, NULL, NULL, NULL,UI_TASK_PRIORITY, 0, 0);
/****************************************************************************
 * PRIVATE FUNCTIONS
 ****************************************************************************/

/****************************************************************************
 * EXPORTED FUNCTIONS
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
/**
 * @brief: This Function initialize UI task
 */
bool app_UITaskInit(void)
{
	bool ret = true;
#if 1
	/* Create the queue, storing the returned handle in the xQueue variable. */
	k_msgq_init(&uiMsgQueue, uiMsgQueueBuffer, UI_Q_ITEM_SIZE, UI_Q_LEN);
	k_msgq_init(&bleMsgQueue, bleMsgQueueBuffer, BLE_Q_ITEM_SIZE, BLE_Q_LEN);
#endif
	k_thread_create(&uiTaskHandler, ui_task_stack,
					K_KERNEL_STACK_SIZEOF(ui_task_stack),
					(k_thread_entry_t)app_vUITask, NULL, NULL, NULL,
					K_PRIO_COOP(UI_TASK_PRIORITY),
					0, K_NO_WAIT);
	k_thread_name_set(&uiTaskHandler, "UI task");
	return ret;
}

/**
 * @brief: This is UI task handler
 */
void app_vUITask(void *arg)
{
	uimsgQueueData_t uimsgData = {0};
	while (true)
	{
		// LOG_DBG("!!!!!!!!!!!!!! app_vUITask !!!!!!!!!!!!!!\n");

		//if (0U == k_msgq_get(&uiMsgQueue, &uimsgData, K_MSEC(10)))
		{
			app_err_t err = NRF_FAIL;
			uimsgData.msg_id = SET_CHARGING_START_STOP_CMD; //ToDO :Remove this
			switch (uimsgData.msg_id)
			{
			case SET_SYSTEM_TIME_CMD:
			{
				LOG_INF(" ********** SET_SYSTEM_TIME_CMD **********");
				System_tm_t sys_tm = {0U};
				(void)memcpy(&sys_tm, uimsgData.data, sizeof(System_tm_t));
				if (sys_tm.sys_epoch > 1632987631)
				{
					set_get_system_time_from_cli(1, sys_tm.sys_epoch);
					mcp794x_save_mcu_time_to_rtc();
					sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
				}
				else
				{
					sendStatus(uimsgData.msg_id, INVALID_PAYLOAD_DATA);
				}
			}
			break;
			case SET_SCHEDULE_CONFIG_CMD:
			{
				app_scheduler_t scheduler_config = {0U};
				err = GetScheduleInfo(&scheduler_config);
				if (NRF_OK == err)
				{
					LOG_INF("------------- %s", (char *)uimsgData.data);
					(void)memcpy(&scheduler_config, uimsgData.data, sizeof(app_scheduler_t));

					err = UpdateScheduleInfo(&scheduler_config);
					if (NRF_OK == err)
					{
						schedule_charging_status = SCHEDULE_CHARGING_STOP;
						StopCmdRcvd = 0;
						sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
					}
					else
					{
						LOG_ERR("Error while Write Configuration data To nvs in Gprs Configuration, Error = %d", err);
						sendStatus(uimsgData.msg_id, CMD_NVS_FLASH_ERROR);
					}
				}
				else
				{
					LOG_ERR("Error while Read Configuration data from nvs in Gprs Configuration, Error = %d", err);
					sendStatus(uimsgData.msg_id, CMD_NVS_FLASH_ERROR);
				}
			}
			break;
			case OTA_UPDATE_CMD:
			{
			}
			break;

			case GET_FAULT_INFO_CMD:
			{
			}
			break;

			case GET_ALERT_INFO_CMD:
			{
			}
			break;

			case FACTORY_RESET_CMD:
			{
			}
			break;

			case OTA_STATUS_NOTIFY_CMD:
			{
			}
			break;
			case GET_CONNECTION_STATUS_CMD:
			{
			}
			break;
			case GET_HEARTBEAT_CMD:
			{
				err = SendHeartbeatData();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case SET_CURRENT_LIMIT_CMD:
			{
				LOG_INF(" ********** SET_CURRENT_LIMIT_CMD **********");
				app_scheduler_t read_scheduler = {0U};
				err = GetScheduleInfo(&read_scheduler);
				if (NRF_OK == err)
				{
					read_scheduler.sch_current_limit = Max_Current_Limit;
					LOG_INF(" ********** SET_CURRENT_LIMIT_CMD##########################%d", Max_Current_Limit);

					err = UpdateScheduleInfo(&read_scheduler);
					if (NRF_OK == err)
					{
						sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
					}
					else
					{
						sendStatus(uimsgData.msg_id, CMD_NVS_FLASH_ERROR);
					}
				}
				else
				{
					sendStatus(uimsgData.msg_id, CMD_NVS_FLASH_ERROR);
				}
				// sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
			}
			break;
			case SET_CHARGING_START_STOP_CMD:
			{
				// LOG_INF("********** SET_CHARGING_START_STOP_CMD **********");
				//sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
			}
			break;
			case GET_HISTORY_LOG_CMD:
			{
				LOG_INF(" ********** GET_HISTORY_LOG_CMD **********");
#if 0
					ReadHistoryEntry_t record = {0U};
					(void)memcpy(&record, uimsgData.data, sizeof(ReadHistoryEntry_t));
					if (record.start_record <= record.end_record)
					{
						//err = GetHistoryLogs(record.start_record, record.end_record);
						if (NRF_ERR_NVS_NOT_FOUND == err)
                        {
                            sendStatus(uimsgData.msg_id, CMD_DATA_NOT_FOUND);
                        }
					}
					else
					{
						sendStatus(uimsgData.msg_id, INVALID_PAYLOAD_DATA);
					}
#endif
			}
			break;
			case GET_HISTORY_EVENT_CMD:
			{
				LOG_INF(" ********** GET_HISTORY_EVENT_CMD **********");
#if 0
					ReadHistoryEntry_t record = {0U};
					(void)memcpy(&record, uimsgData.data, sizeof(ReadHistoryEntry_t));
					if (record.start_record <= record.end_record)
					{
						//err = GetAllHistoryEnergy(record.start_record, record.end_record);
						if (NRF_ERR_NVS_NOT_FOUND == err)
                        {
                            sendStatus(uimsgData.msg_id, CMD_DATA_NOT_FOUND);
                        }
					}
					else
					{
						sendStatus(uimsgData.msg_id, INVALID_PAYLOAD_DATA);
					}
#endif
			}
			break;
			case GET_HISTORY_ENERGY_CMD:
			{
				LOG_INF(" ********** GET_HISTORY_ENERGY_CMD **********");
#if 0
					ReadHistoryEntry_t record = {0U};
					(void)memcpy(&record, uimsgData.data, sizeof(ReadHistoryEntry_t));
					if (LAST_7_DAYS == record.type)
					{
						err = GetLast_7DaysEnergy(WEEK_START_RECORD, WEEK_END_RECORD);
						if (NRF_ERR_NVS_NOT_FOUND == err)
						{
							sendStatus(uimsgData.msg_id, CMD_DATA_NOT_FOUND);
						}
					}
					else if (LAST_30_DAYS == record.type)
					{
						err = GetLast_30DaysEnergy(MONTH_START_RECORD, MONTH_END_RECORD);
						if (NRF_ERR_NVS_NOT_FOUND == err)
						{
							sendStatus(uimsgData.msg_id, CMD_DATA_NOT_FOUND);
						}
					}
					else if (LAST_YEAR == record.type)
					{
						err = GetLastYearEnergy(YEAR_START_RECORD,YEAR_END_RECORD);
						if (NRF_ERR_NVS_NOT_FOUND == err)
						{
							sendStatus(uimsgData.msg_id, CMD_DATA_NOT_FOUND);
						}
					}
					else
					{
						sendStatus(uimsgData.msg_id, INVALID_PAYLOAD_DATA);
					}
#endif
			}
			break;
			case GET_CURRENT_LIMIT_CMD:
			{
				LOG_INF(" ********** GET_CURRENT_LIMIT_CMD **********");
				err = SendCurrentLimitData();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case GET_SCHEDULE_CONFIG_CMD:
			{
				LOG_INF(" ********** GET_SCHEDULE_CONFIG_CMD **********");
				err = SendScheduleData();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case GET_AVAILABLE_LOGS_CMD:
			{
				LOG_INF(" ********** GET_AVAILABLE_LOGS_CMD **********");
				err = SendLogCount();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case SET_AUTOMODE_CMD:
			{
				LOG_INF(" ********** SET_AUTOMODE_CMD **********");
				uint8_t AutoModeStatus = 0;
				(void)memcpy(&AutoModeStatus, uimsgData.data, sizeof(AutoModeStatus));
				err = UpdateAutoModeStatus(&AutoModeStatus);
				if (NRF_OK == err)
				{
					sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
				}
				else
				{
					sendStatus(uimsgData.msg_id, CMD_NVS_FLASH_ERROR);
				}
			}
			break;
			case GET_AUTOMODE_CMD:
			{
				LOG_INF(" ********** GET_AUTOMODE_CMD **********");
				err = SendAutModeStatus();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case GET_ALL_CONFIG_READ_CMD:
			{
				LOG_INF(" ********** GET_ALL_CONFIG_READ_CMD **********");
				heartbeat_info_t hb_info = {0U};
				GetHeartbeatInfo(&hb_info);
				err = SendHeartbeatData();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				k_msleep(100); // portTICK_PERIOD_MS
				err = SendAutModeStatus();
				k_msleep(100); // portTICK_PERIOD_MS
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				err = SendCurrentLimitData();
				k_msleep(100); // portTICK_PERIOD_MS
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				err = SendScheduleData();
				k_msleep(100); // portTICK_PERIOD_MS
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				// err = GetHistoryLogs(1, 1);
				k_msleep(100); // portTICK_PERIOD_MS
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				err = SendChargingStartTime();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				k_msleep(100); // portTICK_PERIOD_MS
				err = SendMACAddress();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				k_msleep(100); // portTICK_PERIOD_MS
				err = sendfwversion();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
				k_msleep(100); // portTICK_PERIOD_MS
				err = sendunitprice();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;

			case GET_MAC_READ_CMD:
			{
				LOG_INF(" ********** GET_MAC_READ_CMD **********");
				err = SendMACAddress();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case GET_CHARGING_START_TIME:
			{
				LOG_INF(" ********** GET_CHARGING_START_TIME **********");
				err = SendChargingStartTime();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case GET_FW_VERSION:
			{
				LOG_INF(" ********** GET_FW_VERSION **********");
				err = sendfwversion();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			case SET_CHARGER_CONFIGURATION:
			{
				LOG_INF(" ********** SET_CHARGER_CONFIGURATION **********");
				sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
			}
			break;

			case SET_UNIT_PRICE_CMD:
			{
				LOG_INF(" ********** SET_UNIT_PRICE_CMD **********");
				sendStatus(uimsgData.msg_id, CMD_SUCCESS_RESP);
			}
			break;
			case GET_UNIT_PRICE_CMD:
			{
				err = sendunitprice();
				if (NRF_OK != err)
				{
					sendStatus(uimsgData.msg_id, CMD_FAILURE_RESP);
				}
			}
			break;
			default: /* Invalid case execute */
			{
				LOG_ERR("Invalid Message ID received, msg id: 0x%x", uimsgData.msg_id);
			}
			break;
			}
		}
		k_msleep(1000);
	}
}

/**
 * @brief: This Function check earth fault condition while charging is going on
 *
 * @param[in] void
 * @param[out] uint8_t 1 or 0
 */
uint8_t get_earthfault_status(void)
{
	static uint8_t earth_pin;
	earth_pin = MCP23017_IOExpGpioLevelGet(0); // TODO:Replace actual earth pin
	return earth_pin;
}
/**
 * @brief: This Function check Emergency fault condition
 *
 * @param[in] void
 * @param[out] uint8_t 1 or 0
 */
uint8_t getEmergencyStopSWState(void)
{
	uint8_t Estop_state = 0;
	// TODO :detect ESTOP state
	return Estop_state;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/