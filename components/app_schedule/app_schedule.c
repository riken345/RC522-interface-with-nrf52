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
/** \file app_schedule.c
 * \brief
 *
 **/
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
// #include <zephyr/sys/__assert.h>
#include "app_schedule.h"
#include "components/app_db/app_db.h"
#include "zephyr/logging/log.h"
#include "components/adc_services/adc_service.h"
#include "components/temp_reading/temp.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define TAG "schedule"
#define SCHEDULE_TASK_NAME "schedule_task"
#define SCHEDULE_TASK_STACK_SIZE (4 * 1024)
#define SCHEDULE_TASK_PRIORITY 8
#define TASK_DELAY_MS 1000U

#define INPUT_BUTTON_PIN 1                                                                                                                                         // TODO :add actual GPIO here
char *network_reset_string = "{\"ssid\": \"Enter_ssid\",\"pwd\": \"Enter_password\",\"security\": \"1\",\"mode\": \"1\",\"i_type\": \"0\"\",\"app_mode\": \"1\"}"; //

static struct k_thread schedule_task_handle;
static K_KERNEL_STACK_DEFINE(schedule_task_stack, SCHEDULE_TASK_STACK_SIZE);
// extern const k_tid_t schedule_task_handle;																											   /**< Reference to schedule FreeRTOS task. */
// K_THREAD_DEFINE(schedule_task_handle, SCHEDULE_TASK_STACK_SIZE, ScheduleTask, NULL, NULL, NULL, SCHEDULE_TASK_PRIORITY, 0, 0);

LOG_MODULE_REGISTER(appSchedule, LOG_LEVEL_DBG);
/****************************************************************************
 * TYPES
 ****************************************************************************/

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
uint8_t schedule_charging_status;
extern uint8_t StopCmdRcvd;
/****************************************************************************
 * PRIVATE FUNCTIONS
 ****************************************************************************/

/****************************************************************************
 * EXPORTED FUNCTIONS
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
uint8_t is_schedule_charging(void)
{
    return schedule_charging_status;
}

void check_schedule(void)
{
#if 0
    uint16_t current_time = 0;
    app_scheduler_t app_scheduler = {0U};
    GetScheduleInfo(&app_scheduler);
    /**
     * check if vehicle is available and charger is connected
    */
    if (get_system_real_time() > 1634627713) //TODO :add api for check charger state
    {
        current_time = compare_scheduletime(); 
        printk("charging_start-----------> %d compare_time --------> %d",is_schedule_charging(),current_time);
        if (SCHEDULE_CHARGING_STOP == is_schedule_charging())
        {
            if (app_scheduler.wd_Endtm > app_scheduler.wd_Starttm && 
                (current_time >= app_scheduler.wd_Starttm) && (current_time < app_scheduler.wd_Endtm))
            {
                printk("<<<<<<<<<<<<<Stage 1>>>>>>>>>>>");
                schedule_charging_status = SCHEDULE_CHARGING_START;
            }
            else if (app_scheduler.wd_Endtm < app_scheduler.wd_Starttm && 
                    (current_time >= app_scheduler.wd_Starttm || current_time < app_scheduler.wd_Endtm))
            {
                printk("<<<<<<<<<<<<<Stage 2>>>>>>>>>>>");
                schedule_charging_status = SCHEDULE_CHARGING_START;
            }
        }
        if (SCHEDULE_CHARGING_START == is_schedule_charging())
        {
            if (app_scheduler.wd_Endtm > app_scheduler.wd_Starttm && current_time >= app_scheduler.wd_Endtm)
            {
                printk( "<<<<<<<<<<<<<Stage 3>>>>>>>>>>>");
                schedule_charging_status = SCHEDULE_CHARGING_STOP;
                StopCmdRcvd = 0;
            }
            else if (app_scheduler.wd_Endtm <= app_scheduler.wd_Starttm &&
                     current_time >= app_scheduler.wd_Endtm && current_time < app_scheduler.wd_Starttm)
            {
                printk( "<<<<<<<<<<<<<Stage 4>>>>>>>>>>>");
                schedule_charging_status = SCHEDULE_CHARGING_STOP;
                StopCmdRcvd = 0;
            }
        }
    }
#endif
}

uint16_t compare_scheduletime(void)
{
    uint16_t final_tm = 0;
#if 0
    uint8_t hr = 0;
    uint8_t min = 0;
    
    get_system_min(&min);
    get_system_hr(&hr);
    final_tm = (hr * 100) + min;
    LOG_ERR( "hr : %d min: %d", hr, min);
#endif
    return final_tm;
}
/**
 * @brief: This Function initialize Schedule task
 */
void ScheduleTask_Init(void)
{
    // k_thread_create(schedule_task_handle,NULL,SCHEDULE_TASK_STACK_SIZE,NULL,NULL,NULL,SCHEDULE_TASK_PRIORITY,NULL,K_NO_WAIT);
    // k_thread_start(schedule_task_handle);
    k_thread_create(&schedule_task_handle, schedule_task_stack,
                    K_KERNEL_STACK_SIZEOF(schedule_task_stack),
                    (k_thread_entry_t)ScheduleTask, NULL, NULL, NULL,
                    K_PRIO_COOP(SCHEDULE_TASK_PRIORITY),
                    0, K_NO_WAIT);
    k_thread_name_set(&schedule_task_handle, "Schedule task");
    // k_thread_start(schedule_task_handle);
}

/**
 * @brief: This is Scheduleing task handler.
 *
 * @param[in] arg
 *
 * @param[out] void
 */
void ScheduleTask(void *arg)
{
    

    while (true)
    {
#if 0
        app_scheduler_t app_scheduler = {0U};
        uint8_t ChargingMode = 0;
        processDayChangeEvent();
        GetScheduleInfo(&app_scheduler);
        printk("%d  %d  %d  %d  %d %d",
               app_scheduler.is_schedule,
               app_scheduler.wd_Starttm,
               app_scheduler.wd_Endtm,
               app_scheduler.we_Starttm,
               app_scheduler.we_Endtm,
               app_scheduler.sch_current_limit);
        /**
         * start scheduling if user enable scheduling
         * */
        GetAutoModeStatus(&ChargingMode);
        if (1 == app_scheduler.is_schedule)
        {
            check_schedule();
        }
        printk("!!!!!!!!!!!!!!!! ScheduleTask !!!!!!!!!!!!!*\n");
#endif
        adc0_read_raw();
        k_msleep(1);
    }
}

void processDayChangeEvent(void)
{
#if 0
    uint32_t rtc_date;
    uint8_t rtc_month;
uint32_t nvs_LastDate;//TODO :Read this variable form EEPROM or NVS and update there
uint8_t nvs_LastMonth;//TODO :Read this variable form EEPROM or NVS and update there

    get_YYMMDD(&rtc_date);
    rtc_month = (rtc_date % 10000) / 100;
    // printk( "Last date %u ,rtc_date %u last_month %d rtc_month %d", nvs_LastDate, rtc_date,nvs_LastMonth,rtc_month);
    if (rtc_date != nvs_LastDate)
    {
        if ((get_system_real_time() > 1636588800) && (get_system_real_time() < 1920585600))
        {
            printk( "<<<<<<<<<<<<<<<<<<<Day Change detected>>>>>>>>>>>>>>>>>");
            AddHistoryEnergy();
            nvs_LastDate = rtc_date;
            nvsWriteLastDate(nvs_LastDate);
            printk( "<<<<<<<<<<<<<<<<<<<Saving energy value to SQL database>>>>>>>>>>>>>>>>>");
        }
    }
    if (rtc_month != nvs_LastMonth)
    {
        printk("<<<<<<<<<<<<<<<<<< Month Change Detected >>>>>>>>>>>>>>>>>>>>>");
        AddMonthlyHistoryEnergy();
        nvs_LastMonth = rtc_month;
        nvsWriteLastMonth(nvs_LastMonth);
        printk( "<<<<<<<<<<<<<<<<<<<Saving Month change energy value to SQL database>>>>>>>>>>>>>>>>>");
    }
#endif
}

app_err_t init_input_button(void)
{
    app_err_t err = NRF_FAIL;
    // TODO:add original GPIO here
    return err;
}

uint8_t input_button_state(void)
{
    uint8_t button_state = 0U;
    // TODO:Add Button state
    // printk( "button_state+++++++++++++++++++++++++++++++ = %d", button_state);
    return button_state;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/