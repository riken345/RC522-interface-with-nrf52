
/*!
 * \file     app_timer.c
 * \brief
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "app_timer.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
// #include <zephyr/sys/__assert.h>
#include "zephyr/logging/log.h"

#include <stdlib.h>

#include "components/rtc_mcp79400/gw_rtc.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/app_schedule/app_schedule.h"
#include "components/app_db/app_db.h"
#include "components/meter_ade7593/ade7593.h"
#include "components/app_UI/json_parser.h"
// #include "cp_pwm.h"
#include "components/ioExp_mcp23s17/ioExp_mcp23s17.h"
#include "components/adc_services/adc_service.h"
#include "components/volt_curr_monitor/volt_curr_monitor.h"
#include "components/temp_reading/temp.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define TAG "app_timer"
#define APP_TIMER_PERIOD 1000 /**< Timer period. 1000 ms */
#define HEARTBEAT_UPDATE_TIME 2U

LOG_MODULE_REGISTER(appTimer, LOG_LEVEL_DBG);
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
struct k_timer app_timer_handle; /**< Reference to LED1 toggling FreeRTOS timer. */
struct k_timer sync_timer_handle; /**< Reference to LED1 toggling FreeRTOS timer. */
extern int32_t gi32ReadADCValue[ADC_MAX_CHANNEL];
extern bool bSyncDone;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/
/**
 * @brief: System tick handler
 *
 * @param[in] void
 * @param[out] void
 */
void systemTickTimer(struct k_timer *timer_id);
/**
 * @brief: PWM sync timer handler
 *
 * @param[in] void
 * @param[out] void
 */
void syncTimer(struct k_timer *timer_id);
/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/

/**@brief The function to call when the timer expires.(1000 ms timer)
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
 */

/**
 * @brief: System tick handler
 */
void systemTickTimer(struct k_timer *timer_id)
{
    // heartbeat_info_t hb_info = {0U};
    // energy_info_t liveEnergyData = {0U};
    // Metering_info_spi_t meter_info = {0U};
    // fault_info_t fault_info = {0U};

    // static uint16_t tickCnt = 0U;
    // static uint16_t LiveDataTickCnt = 0U;
    // static uint16_t EnergyTickCnt = 0U;
    // static uint8_t plugandplaymode = 0U;
    // app_err_t err = NRF_FAIL;

    // uint32_t temp_u32energy = 0;
    // static uint8_t HBTickCnt = 0;
    // static bool firsttm_flag = false;
    // GetHeartbeatInfo(&hb_info);
    // GetAutoModeStatus(&plugandplaymode);
    // LOG_DBG("!!!!!!!!!!!!!!!!! systemTickTimer called !!!!!!!!!!!!!!!!!\n");
    VOLTAGE_processADCData();
    adctempdataread();
#if 0 // TODO:Need to update
    //if (MOBILE_APP_MODE ==  get_charger_mode())
    {

        if ((AUTOMODE_ENABLE == plugandplaymode))//true == get_connection_status() || 
        {
#if 0
            if (HBTickCnt++ >= HEARTBEAT_UPDATE_TIME || (!firsttm_flag))
            {
                HBTickCnt = 0;
                firsttm_flag = true;//this flag is used for first time send only
                GetHeartbeatInfo(&hb_info);
                hb_info.sys_tm = get_system_real_time();
                UpdateHeartbeatInfo(&hb_info);
                SendHeartbeatData();
            }
#endif

            if (HBTickCnt++ >= HEARTBEAT_UPDATE_TIME )
            {
                HBTickCnt = 0;
                Heartbeatsend();
            }
            //TODO :Need to update
            //if (Charging == hb_info.ev_current_state && tickCnt++ >= LIVE_DATA_UPDATE_TIME)
            {
                LOG_ERR("$$$$$$$$$$$$$$$$$$$$$$$$$$$ev_current_state %d",hb_info.ev_current_state);
                SendMeteringInfo();
                err = GetSPIMeteringInfo(&meter_info);
                if (NRF_OK == err)
                {
                    // fault_info.err_code = Check_Fault(meter_info.voltage, meter_info.current);
                    // UpdateFaultInfo(&fault_info);
                }
                else
                {
                    LOG_ERR("Error in read meterinfo");
                }
                //SendFaultInfo();
                tickCnt = 0;
            }
        }
    
    }
    /** Update energy data  on fix time interval */
    //TODO : Need to update
    //if (Charging == hb_info.ev_current_state && EnergyTickCnt++ >= ENERGY_UPDATE_TIME)
    {
#if ENERGYFROMPOWER
            vCalculateEnergyFromPower();
#endif
#if ENERGYFROMMETER
            read_energy_flag = 1;
#endif
        EnergyTickCnt = 0;
    
    }
    /** Flushing live data block in non-volatile storage on fix time interval */
    //TODO :Need  to update
    //if (Charging == hb_info.ev_current_state && LiveDataTickCnt++ >= FLASH_UPDATE_TIME)
    {
        if ((get_system_real_time() > 1636588800) && (get_system_real_time() < 1920585600))
        {
            err = GetSPIMeteringInfo(&meter_info);
            if (NRF_OK == err)
            {
                liveEnergyData.cumulative_energy = meter_info.RPhase_session_energy;
                LOG_INF("liveEnergyData.cumulative_energy %f", liveEnergyData.cumulative_energy);
            }
            else
            {
                LOG_ERR("Error in read meterinfo");
            }
            NvsWriteLiveEnergyBlock(liveEnergyData);
            LiveDataTickCnt = 0;
        }
    }
    
    if (1U == ev_error_info[0].StartCurrentChkTimmer)
    {
        ev_error_info[0].CurrentChkTimmer++;
    }
    else
    {
        ev_error_info[0].CurrentChkTimmer = 0;
    }
#endif
}
void appTickTimer_Init(void)
{
    /* Init RTOS timer for */
    k_timer_init(&app_timer_handle, systemTickTimer, NULL);
    /* start timer for 500 ms */
    k_timer_start(&app_timer_handle, K_NO_WAIT, K_MSEC(1));
    k_timer_stop(&sync_timer_handle);
}

void vSyncPWM_Timer(void)
{
     /* Init RTOS timer for */
    k_timer_init(&sync_timer_handle, syncTimer, NULL);
    /* start timer for 500 ms */
    k_timer_start(&sync_timer_handle, K_NO_WAIT, K_USEC(50));
}
/**
 * @brief: System tick handler
 */
void syncTimer(struct k_timer *timer_id)
{
    bSyncDone = true;
    k_timer_stop(timer_id);
}