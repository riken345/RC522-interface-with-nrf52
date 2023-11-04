/*
 *      Copyright (c) 2021 Private Company
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *		@author: NA
 *
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of Private Company
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise
 *      duplicated and the information herein may not be used,
 *      disseminated or otherwise disclosed, except with the prior
 *      written consent of Private Company
 *
 */
/** \file cp_pwm.c
 * \brief Filename : pwm_handler.h Function prototypes for the PWM driver initialization.
 *
 **/

/***************************************************************************
 * INCLUDES
 ***************************************************************************/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "zephyr/logging/log.h"
#include "components/cp_pwm/cp_pwm.h"
#include <math.h>
#include "components/app_schedule/app_schedule.h"
#include "components/app_UI/app_ui.h"
#include "components/rtc_mcp79400/gw_rtc.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/ioExp_mcp23s17/ioExp_mcp23s17.h"
#include <components/adc_services/adc_service.h>
#include "components/gpio_config/gpio_config.h"
#include "components/oled_ssd1306/ssd1306.h"

/***************************************************************************
 * MACROS
 ****************************************************************************/
#define EARTHFAULT_DETECT 1
#define CHARGING_ONGOING 1
#define CHARGING_STOP 0
#define NO_STATE 0
#define FAULT_COUNTER 20
/*  CP PP Fault Check TImer */
#define CP_PP_FAULT_CHECK_COUNTER 10
#define MAX_RFID_LENGTH 5U
#define DUTYCYCLE_DIV_FACTOR  0.6f
LOG_MODULE_REGISTER(cpPwm, LOG_LEVEL_DBG);
/***************************************************************************
 * TYPES
 ***************************************************************************/
static J1772adc_st J1772Adc = {0};
/***************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************/
float fTotalInstaValue = 0;
uint8_t LastSessionStatus;
uint8_t ChargingMode = 0;
uint8_t mobileappcharging_flag = 0;
char rfid_store[11] ={0};

fault_detections_t faults  ={0};
uint16_t Max_Current_Limit = 0;
uint8_t ChargingStartMode =0 ;
History_Logdata_t StartHistoryLog ={0};
float StartEnergy = 0.0;
uint8_t u8Startmeasurecurrent = 0;
bool start_pwm =0 ; // this flag is deciding factor whether to start or stop pwm in all 3 modes
uint8_t rfidofflinetap;
ev_error_t ev_error_info;
char rfid_with_button_Selection[RFID_WITH_BUTTON_STR_LEN];

static struct k_thread PWMTaskHandler;
static K_KERNEL_STACK_DEFINE(CP_task_stack, CPPWM_TASK_STACK_SIZE);

bool bSyncDone = true;
extern int32_t gi32ReadADCValue[ADC_MAX_CHANNEL];
// extern const k_tid_t PWMTaskHandler; /**< Reference to J1772 FreeRTOS task. */
// K_THREAD_DEFINE(PWMTaskHandler, CPPWM_TASK_STACK_SIZE, cp_pwmTask, NULL, NULL, NULL, CPPWM_TASK_PRIORITY, 0, 0);
/***************************************************************************
 * PRIVATE FUNCTION DEFINATION
 ***************************************************************************/
uint8_t check_already_running_session(void);
/***************************************************************************
 * FUNCTIONS
 ***************************************************************************/
extern void vSyncPWM_Timer(void);



app_err_t ADC_ErrReadAdcOverI2C(void)
{
    static uint8_t u8FaultTimer = 0;
    app_err_t port_ret = 0;
    static uint8_t Limit = 9U;
    app_err_t err = NRF_FAIL;
    static uint8_t ui8EVState = Available;

    memset(&J1772Adc, 0, sizeof(J1772Adc));

    ui8EVState = ui8ReadCurrentEVstate();
    UpdateChargingState(ui8EVState);
    // TODO : Need to call check_Fault
    switch (ui8EVState)
    {
    case Available:
    {
        port_ret = Available;
        // resetrfidParameter();
        resetUIStartCmd();
        DutyCycle_from_Current(DUTY100);
        start_pwm = false;
        set_relay_output(RELAY_OFF);
    }
    break;
    case Preparing:
    {
        port_ret = Preparing;
        if (MOBILE_APP_MODE == get_charger_mode())//TODO: Need to add start command
        {
            start_pwm = true;
            // bSyncDone = false;
            // vSyncPWM_Timer();
            /**
             * when bSyncDone flag is true we read adc value of cp feedback
             * when we start pwm at that time we stop bSyncDone flag and start 
             * timer for 50us
             * on timer done we stop timer and enable bSyncDone flag so we get
             * rising edge every time
            */
        }
        if (start_pwm)
        {
            DutyCycle_from_Current(Limit);//TODO: Need to raed current limit 
            LOG_DBG("PWM giving in preparing state");
        }
        else
        {
            DutyCycle_from_Current(DUTY100);
        }
        resetUIStartCmd();
        if (MOBILE_APP_MODE == get_charger_mode())// && mobileappcharging_flag == true)
        {
            set_relay_output(RELAY_OFF);
        }
    }
    break;
    case Charging:
    {
        port_ret = Charging;
        if (start_pwm)
        {
            DutyCycle_from_Current(Limit);
        }
        else
        {
            DutyCycle_from_Current(DUTY100);
        }
        if (MOBILE_APP_MODE == get_charger_mode())//&& mobileappcharging_flag == false)
        {
            set_relay_output(RELAY_ON);
            set_chargingstart_time();
        }
    }
    break;
    case Faulted:
    {
        u8FaultTimer = u8FaultTimer + 1;
        if (u8FaultTimer > FAULT_COUNTER)
        {
            u8FaultTimer = 0;
            // u8LocalTransaction[1] = LOCAL_TX_DEFAULT;
            faults.bEmergncyStopDetect = false;
            faults.bTripSensorDetect = false;
            faults.bZeroCurrentDetect = false;
            faults.bCarEstateDetect = false;
            faults.bCpFaultDetect = false;
            faults.bEarthFaultDetect = false;
        }
        port_ret = Faulted;

        // resetrfidParameter();
        resetUIStartCmd();
        DutyCycle_from_Current(DUTY100);
        if (MOBILE_APP_MODE == get_charger_mode())
        {
            start_pwm = false;
            set_relay_output(RELAY_OFF);
        }
    }
    break;
    case SuspendedEVSE:
    {
        port_ret = Available;
        if (MOBILE_APP_MODE == get_charger_mode())
        {
            start_pwm = false;
            set_relay_output(RELAY_OFF);
        }
        DutyCycle_from_Current(DUTY100);
        // resetrfidParameter();
    }
    break;
    default:
    {
        LOG_ERR("invalid state case");
        port_ret = NRF_FAIL;
        DutyCycle_from_Current(DUTY100);
        if (MOBILE_APP_MODE == get_charger_mode())
        {
            start_pwm = false;
            set_relay_output(RELAY_OFF);
        }
        // resetrfidParameter();
    }
    break;
    }
    return port_ret;
}

uint8_t ui8ReadCurrentEVstate(void)
{
    int icppwmgpiolevel = HIGH;
    static uint8_t ui8EVHwState = Available;
    float cpvoltage = 0.0f;
    static uint8_t ui8EVStateFailCount = 0U;

    if (true == faults.bEmergncyStopDetect || true == faults.bTripSensorDetect || true == faults.bZeroCurrentDetect || true == faults.bCpFaultDetect || true == faults.bCarEstateDetect || true == faults.bEarthFaultDetect)
    {
        ui8EVHwState = Faulted;
        return ui8EVHwState;
    }
    /* CP voltage reading */
    J1772Adc.ui8CPMonAdcCount = ADC_u8ReadCount(ADC_CH_CP_MON);
    J1772Adc.ADC_Voltage = (float)(J1772Adc.ui8CPMonAdcCount * ADC_VREF / 4095.0f);
    cpvoltage = J1772Adc.ADC_Voltage * CP_FACTOR;

    /* PP voltage reading */
    // J1772Adc.ui8PPMonAdcCount = ADC_u8ReadCount(ADC_CH_PP_MON);
    // icppwmgpiolevel = gpio_get_level(CP_PWM_GPIO);

    if (NRF_FAIL == icppwmgpiolevel)
    {
        LOG_ERR("Fail to read CP_PWM_GPIO");
    }
    if (LOW == icppwmgpiolevel)
    { 
        // measured CP negative voltage only when CP PWM is low
        /* CP Negative voltage reading */
        // J1772Adc.ui8CPMonNegAdcCount = ADC_u8ReadCount(ADC_CH_CP_MON_NEG);
    }
    LOG_DBG("********* count %d ", J1772Adc.ui8CPMonAdcCount);

    if (IS_IN_RANGE(J1772Adc.ui8CPMonAdcCount, CP_VOL_STATE_A_LOW, CP_VOL_STATE_A_HIGH))
    {
        ui8EVStateFailCount = 0;
        ui8EVHwState = Available;
        LOG_DBG("return state:Available");
    }
    else if (IS_IN_RANGE(J1772Adc.ui8CPMonAdcCount, CP_VOL_STATE_B_LOW, CP_VOL_STATE_B_HIGH))
    { // State B : EV connected CP_Volatge= 8.3 to 9.5 V	//PP voltage = 0.3V to 0.8 V
        ui8EVStateFailCount = 0;
        ui8EVHwState = Preparing;
        resetUIStartCmd();
        LOG_DBG("return state:Preparing");
    }
    else if (IS_IN_RANGE(J1772Adc.ui8CPMonAdcCount, CP_VOL_STATE_C_LOW, CP_VOL_STATE_C_HIGH))
    { // State C : EV connected,Ventilation not required CP_Volatge= 5.7 to 6.5 V	//PP voltage = 0.3V to 0.8 V
        if (1U == CheckStartCmdStatus())
        {
            ui8EVStateFailCount = 0;
            ui8EVHwState = Charging;
            LOG_DBG("return state:Charging");
        }
        else
        {
            ui8EVStateFailCount = 0;
            ui8EVHwState = Preparing;
            resetUIStartCmd();
            LOG_DBG("return state:Preparing");
        }
    }
    else if (IS_IN_RANGE(J1772Adc.ui8CPMonAdcCount, CP_VOL_STATE_D_LOW, CP_VOL_STATE_D_HIGH))
    { // State D : EV connected,Ventilation required CP_Volatge= 2.59 to 3.28 V	//PP voltage = 0.3V to 0.8 V
        // ui8EVHwState = Charging;
        if (1U == CheckStartCmdStatus())
        {
            ui8EVStateFailCount = 0;
            ui8EVHwState = Charging;
            LOG_DBG("return state:Charging");
        }
        else
        {
            ui8EVStateFailCount = 0;
            ui8EVHwState = Preparing;
            resetUIStartCmd();
            LOG_DBG("return state:Preparing");
        }
    }
    else
    { // State E : EV connected,Error CP_Volatge = undefined range         //PP voltage = 0.3V to 0.8 V
        ui8EVStateFailCount = ui8EVStateFailCount + 1;
        if (ui8EVStateFailCount == CP_PP_FAULT_CHECK_COUNTER)
        { // to ensure failure
            ui8EVStateFailCount = 0;
            LOG_DBG("return state:Faulted");
            faults.bCpFaultDetect = true;
            faults.bCarEstateDetect = true;
            ui8EVHwState = Faulted;
        }
    }
    return ui8EVHwState;
}

float ADC_fReadValue(uint8_t ui8ADCNum)
{
    uint8_t u8AdcCount = 0;
    float fAdcVoltage = 0.0f;
    u8AdcCount = ADC_u8ReadCount(ui8ADCNum);
    fAdcVoltage = u8AdcCount * ADC_TO_VOLT_MULTIPLIER;
    return fAdcVoltage;
}

uint32_t ADC_u8ReadCount(uint8_t ui8ADCNum)
{
    // uint8_t u8AdcCount = 0;
    uint32_t raw_AdcCount = 0;
    uint16_t u16SamplIndx = 0;
    uint32_t u32AdcCount = 0;
    const uint16_t u16SampleNo = 64;
    for (u16SamplIndx = 0; u16SamplIndx < u16SampleNo; u16SamplIndx++)
    {
        raw_AdcCount = gi32ReadADCValue[ADC_PILOT_FEEDBACK_CHANNEL];
        u32AdcCount = u32AdcCount + raw_AdcCount;
    }
    raw_AdcCount = u32AdcCount / u16SampleNo;
    return raw_AdcCount;
}
/**
 * @brief: This Function initialize pwm monitor task.
 *
 * @param[in] void
 * @param[out] void
 */
bool cppwm_TaskInit(void)
{
    k_thread_create(&PWMTaskHandler, CP_task_stack,
                    K_KERNEL_STACK_SIZEOF(CP_task_stack),
                    (k_thread_entry_t)cp_pwmTask, NULL, NULL, NULL,
                    K_PRIO_COOP(CPPWM_TASK_PRIORITY),
                    0, K_NO_WAIT);
    k_thread_name_set(&PWMTaskHandler, "J1772 task");
    LOG_DBG("********* cppwm_TaskInit *********\n");
    return true;
}

/**
 * @brief: This is CP pwm task handler.
 *
 * @param[in] arg
 *
 * @param[out] void
 */
void cp_pwmTask(void *arg)
{
    while (true)
    {
        ADC_ErrReadAdcOverI2C();
        // oledstartupdisplayprint();
        OledPrintData();
        // display_play();
        k_msleep(1000);
    }
}
app_err_t set_relay_output(bool status)
{
    app_err_t err = NRF_FAIL;
    /*On relay */
    err = MCP23017_IOExpGpioLevelSet(GPA5_RLY1, status);
    GPIO_OnOff(OP_RLY1_EN, status);
    // LOG_DBG("relay status is %d", status);
    return err;
}

void DutyCycle_from_Current(uint8_t PwmDuty)
{
    uint8_t Limit = 0;
    static uint8_t prev_Limit = 0;
    uint8_t dutycycle = 0;
    Limit = PwmDuty;
    // LOG_DBG("Limit  %d prev_Limito %d", Limit,prev_Limit);
    if ((Limit != prev_Limit))
    {
        prev_Limit = Limit;
        if(Limit <= 30)
        {
            dutycycle = (uint8_t)(Limit / DUTYCYCLE_DIV_FACTOR);
            // LOG_DBG("Limit <= 30 Dutycycle is changed to %d", Limit);
            PWM_Start(dutycycle);
        }
        else if(100U == Limit)
        {
            dutycycle = Limit;
            // LOG_DBG(" else if(100U == Limit) to %d", Limit);
            PWM_Start(dutycycle);
        }

        
    }
    // else if (PwmDuty == 100U)
    // {
    //      prev_Limit = Limit;
    //      dutycycle = PwmDuty;
    //      PWM_Start(dutycycle);
    //     LOG_DBG("if (PwmDuty == 100U) Dutycycle is changed to %d", Limit);
    // }
    
}

void UpdateChargingState(uint8_t Current_state)
{
    heartbeat_info_t hb_info = {0U};
    static uint8_t Prev_State = 0;
    if (Prev_State != Current_state)
    {
        GetHeartbeatInfo(&hb_info);
        hb_info.ev_current_state = Current_state;
        hb_info.sys_tm = get_system_real_time();
        UpdateHeartbeatInfo(&hb_info);
        if (MOBILE_APP_MODE == get_charger_mode())
        {
            // SendHeartbeatData();
            // updateLogInfo(Prev_State, Current_state);
            GetAutoModeStatus(&ChargingMode);
        }
        Prev_State = Current_state;
    }
}

uint8_t CheckStartCmdStatus(void)
{   
    uint8_t status = 0;
    fault_info_t fault_info = {0U};

    GetFaultInfo(&fault_info);
    // ChargingMode = RFIDMODE_ENABLE;
    if (1) //(1U != plug_and_play_mode)
    {
        ChargingMode = AUTOMODE_ENABLE;
    }
    else
    {
        ChargingMode =  RFIDMODE_ENABLE;
    }
    if (MOBILE_APP_MODE == get_charger_mode())
    {
        if (MANUALMODE_ENABLE == ChargingMode)
        {
            if (UI_CHARGING_START_CMD == get_uiStartStop_cmd())
            {
                status = 1;
                ChargingStartMode = UI_START;
            }
            else if (SCHEDULE_CHARGING_START == is_schedule_charging()) // && (0U == StopCmdRcvd)
            {
                status = 1;
                ChargingStartMode = SCHEDULE_START;
            }
            else if (SCHEDULE_CHARGING_STOP == is_schedule_charging() && (SCHEDULE_START == ChargingStartMode))
            {
                status = 0;
                ChargingStartMode = SCHEDULE_STOP;
            }
            else if (UI_CHARGING_STOP_CMD == get_uiStartStop_cmd() && (UI_START == ChargingStartMode || SCHEDULE_START == ChargingStartMode))
            {
                status = 0;
                if (UI_START == ChargingStartMode)
                {
                    ChargingStartMode = UI_STOP;
                }
                else if (SCHEDULE_START == ChargingStartMode)
                {
                    ChargingStartMode = SCHEDULE_STOP;
                }
            }
        }
        else if ((AUTOMODE_ENABLE == ChargingMode))
        {
            status = 1;
        }
        else if (RFIDMODE_ENABLE == ChargingMode)
        {
            LOG_DBG("comes in RFID enable");
            static uint8_t previousstate = 0;
            GetFaultInfo(&fault_info);
            if (0U == fault_info.err_code && 1U == rfidofflinetap)
            {
                if (0U == check_already_running_session())
                {
                    status = 1;
                    previousstate = status;
                    // strcpy(rfid_store, (char *)user_rfid);
                }
                else if (1U == check_already_running_session())
                {
                    status = 0;
                    previousstate = status;
                    /*
                     * this will stop pwm for offline RFID mode
                     */
                    start_pwm = false;
                    memset(rfid_store, 0, MAX_RFID_LENGTH);
                }
                else
                {
                    status = previousstate;
                }
                // memset((char *)user_rfid, 0, MAX_RFID_LENGTH);
                rfidofflinetap = 0U;
            }
            else if (0U != fault_info.err_code)
            {
                LOG_DBG("comes in faulted state");
                status = 0;
                previousstate = status;
                memset(rfid_store, 0, MAX_RFID_LENGTH);
                /*
                 * this will stop pwm for offline RFID mode
                 */
                start_pwm = false;
            }
            else
            {
                LOG_DBG("rfid_store %s", rfid_store);
                if (true == mobileappcharging_flag)
                {
                    status = 1;
                }
                else
                {
                    status = 0;
                    /*
                     * this will stop pwm for offline RFID mode
                     */
                    start_pwm = false;
                    memset(rfid_store, 0, MAX_RFID_LENGTH);
                }
            }
        }
        // else if (PUSH_BUTTONMODE == ChargingMode)
        // {
        // 	LOG_ERR("cp_pwm", ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>PUSH_BUTTONMODE %d ", push_button_state());
        // 	if (1U == push_button_state())
        // 	{
        // 		status = 1;
        // 	}
        // 	else if (2U == push_button_state())
        // 	{
        // 		status = 0;
        // 	}
        // }
        if (0 != fault_info.err_code)
        {
            status = 0;
        }
    }
    status = 1; // TODO Remove this
    return status;
}

uint8_t Check_UnderVoltage(float voltage)
{
    uint8_t err = 0;
    float undervoltage_limit = 10.0; // atof(limit_configuration[CP_UNDER_VOLTAGE]);
    if (0.0f == undervoltage_limit)
    {
        undervoltage_limit = UNDERVOLTAGE_LIMIT;
    }
    if (voltage <= undervoltage_limit)
    {
        err = UNDERVOLTAGE;
    }
    return err;
}
uint8_t Check_OverVoltage(float voltage)
{
    uint8_t err = 0;
    float overvoltage_limit = 11.0; // atof(limit_configuration[CP_OVER_VOLTAGE]);
    if (0.0f == overvoltage_limit)
    {
        overvoltage_limit = OVERVOLTAGE_LIMIT;
    }
    if (voltage >= overvoltage_limit)
    {
        err = OVERVOLTAGE;
    }
    return err;
}
uint8_t Check_UnderCurrent(float current)
{
    uint8_t err = 0;
    float undercurrent_limit = 1; // atof(limit_configuration[CP_UNDER_CURRENT]);

    if (0.0f == undercurrent_limit)
    {
        undercurrent_limit = ZEROCURRENT_LIMIT;
    }
    if (current < undercurrent_limit)
    {
        err = ZEROCURRENT;
    }
    return err;
}

uint8_t Check_OverCurrent(float current)
{
    uint8_t err = 0;
    float overcurrent_limit = 1; // atof(limit_configuration[CP_OVER_CURRENT]);
    if (0.0f == overcurrent_limit)
    {
        overcurrent_limit = OVERCURRENT_LIMIT;
    }
    if (current >= overcurrent_limit)
    {
        err = OVERCURRENT;
    }
    return err;
}

uint8_t Check_Temprature(int16_t temprature)
{
    uint8_t err = 0;
    int16_t overtemprature_limit = 1; // atoi(limit_configuration[CP_OVER_TEMPRATURE]);
    if (0 == overtemprature_limit)
    {
        overtemprature_limit = OVERTEMPRATURE_LIMIT;
    }
    if (temprature >= overtemprature_limit)
    {
        err = HIGH_TEMPRATURE;
    }
    return err;
}

uint8_t Check_EmergencyStop(void)
{
    uint8_t err = 0;
    // err = gpio_get_level(EMERGENCYSTOP_INPUT);
    if (true == err)
    {
        faults.bEmergncyStopDetect = true;
    }
    else
    {
        err = faults.bEmergncyStopDetect;
    }
    return err;
}

uint8_t Check_TripSensor(void)
{
    /*
     * we need pin state as well as flag because we need to display this fault pin maybe high or maybe low
     */
    uint8_t err = 0;
    // err = gpio_get_level(TRIP_GPIO);
    if (true == err)
    {
        faults.bTripSensorDetect = true;
    }
    else
    {
        err = faults.bTripSensorDetect;
    }
    return err;
}

uint8_t Check_Fault(float voltage, float current, int16_t temprature, uint8_t connectorId)
{
    uint8_t err = 0;
    uint8_t ret = 0;
    heartbeat_info_t hb_info = {0U};
    ret = GetHeartbeatInfo(&hb_info);
    if (NRF_OK != ret)
    {
        LOG_ERR("Fail to get GetMeteringInfo");
    }
    if (EARTHFAULT_DETECT == get_earthfault_status())
    {
        err = EARTHFAULT;
    }
    if (OCPP_MODE == get_charger_mode())
    {
        if (true == faults.bZeroCurrentDetect)
        {
            err = ZEROCURRENT;
        }
        else if (ZEROCURRENT == Check_UnderCurrent(current) && u8Startmeasurecurrent == 1) // && u8Startmeasurecurrent == 1 && CP.port_status[connectorId] == Charging && u8Startmeasurecurrent == 1
        {
            err = ZEROCURRENT;
        }
    }
    else if (MOBILE_APP_MODE == get_charger_mode())
    {
        if (true == faults.bZeroCurrentDetect)
        {
            err = ZEROCURRENT;
        }
        else if (ZEROCURRENT == Check_UnderCurrent(current) && (hb_info.ev_current_state == Charging)) // && u8Startmeasurecurrent == 1 && CP.port_status[connectorId] == Charging && u8Startmeasurecurrent == 1
        {
            err = ZEROCURRENT;
        }
    }
    if (OVERVOLTAGE == Check_OverVoltage(voltage)) // && u8Startmeasurecurrent == 1CP.port_status[connectorId] == Charging &&
    {
        err = OVERVOLTAGE;
    }
    if (UNDERVOLTAGE == Check_UnderVoltage(voltage)) //&& CP.port_status[connectorId] == Charging && u8Startmeasurecurrent == 1
    {
        err = UNDERVOLTAGE;
    }
    if (OCPP_MODE == get_charger_mode())
    {
        if (OVERCURRENT == Check_OverCurrent(current) && u8Startmeasurecurrent == 1) //&& CP.port_status[connectorId] == Charging && u8Startmeasurecurrent == 1
        {
            err = OVERCURRENT;
        }
    }
    else if (MOBILE_APP_MODE == get_charger_mode())
    {
        if (OVERCURRENT == Check_OverCurrent(current) && (hb_info.ev_current_state == 67U)) //&& CP.port_status[connectorId] == Charging && u8Startmeasurecurrent == 1
        {
            err = OVERCURRENT;
        }
    }
    if (true == Check_EmergencyStop())
    {
        err = ESTOP;
    }
    if (true == faults.bCpFaultDetect)
    {
        err = GUN_FAULT;
    }
    if (true == faults.bCarEstateDetect)
    {
        err = CAR_ERROR_STATE;
    }

    return err;
}

void ResetFaultInfo(void)
{
    fault_info_t fault_info = {0U};
    fault_info.err_code = 0U;
    UpdateFaultInfo(&fault_info);
}

void set_chargingstart_time(void)
{
    Metering_info_spi_t metering_info_spi = {0U};
    GetSPIMeteringInfo(&metering_info_spi);
    // StartHistoryLog.timestamp1 = get_system_real_time();
    StartHistoryLog.energy = metering_info_spi.RPhase_session_energy + metering_info_spi.YPhase_session_energy + metering_info_spi.BPhase_session_energy;
}

uint8_t check_already_running_session()
{
    uint8_t charging_on_other_plug = 0;
    heartbeat_info_t hb_info = {0U};
    GetHeartbeatInfo(&hb_info);
#if 0
    if (0U == strcasecmp(rfid_store, (char *)user_rfid) && Charging == hb_info.ev_current_state)
    {
        charging_on_other_plug = 1; // this will stop charging
    }
    else if (0U == strlen(rfid_store))
    {
        charging_on_other_plug = 0; // this will start charging
    }
    else
    {
        charging_on_other_plug = 2; // this means not same RFID as tapped for start charging
    }
#endif
    return charging_on_other_plug;
}
/***************************************************************************
 * END OF FILE
 ***************************************************************************/
