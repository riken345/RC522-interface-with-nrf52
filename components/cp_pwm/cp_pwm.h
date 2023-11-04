/*
 *      Copyright (c) 2021 private company.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *		@author: Jaimit Shah
 *
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of private company.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise
 *      duplicated and the information herein may not be used,
 *      disseminated or otherwise disclosed, except with the prior
 *      written consent of private company.
 *
 */
/** \file cp_pwm.h
 * \brief
 *
 **/

#ifndef CP_PWM_H
#define CP_PWM_H
/***************************************************************************
 * INCLUDES
 ***************************************************************************/
#include "components/cp_pwm/pwm_handler.h"
#include <stdio.h>
#include "string.h"
#include "components/adc_services/adc_service.h"
#include "components/app_db/app_db.h"
#include "components/general/general.h"
/***************************************************************************
 * MACROS
 ***************************************************************************/
#define ADC_TO_VOLT_MULTIPLIER 3.3 / 4095
#define CONFIG_CPU_ESP32 1
#define ADC_CH_CT3 CHANNEL_SELECTION_0
#define ADC_CH_CT2 CHANNEL_SELECTION_4
#define ADC_CH_CT1 CHANNEL_SELECTION_1

#define ADC_CH_CP_MON 1
#define ADC_CH_CP_MON_NEG ADC_CHANNEL_0
#define ADC_CH_PP_MON 2

/*EV charger STATE - hardware state*/
#define EV_STATE_A 1 // Available
#define EV_STATE_B 2 // Preparing
#define EV_STATE_C 3 // Charging
#define EV_STATE_D 4 // Charging
#define EV_STATE_E 5 // Faulted
#define EV_STATE_P 6
#define EV_STATE_Z 7

#define IS_IN_RANGE(value, low_limit, High_limit) (((low_limit) < (value)) && ((value) <= (High_limit)))

#define ADC_OFFSET_STATE_A 50
#define ADC_OFFSET_STATE_B 50
#define ADC_OFFSET_STATE_C 50
#define ADC_OFFSET_STATE_D 50
#define ADC_OFFSET_STATE_E 50

#define IS_IN_PPVO_RANGE_LOW_PLUG 24
#define IS_IN_PPVO_RANGE_HIGH_PLUG 62
#define IS_IN_PPVO_RANGE_LOW_UNPLUG 199
#define IS_IN_PPVO_RANGE_HIGH_UNPLUG 255

#define CP_VOL_STATE_A_HIGH (2750 + ADC_OFFSET_STATE_A)//0XFDE
#define CP_VOL_STATE_A_LOW (2700 - ADC_OFFSET_STATE_B)//0XF16

#define CP_VOL_STATE_B_HIGH (2400 + ADC_OFFSET_STATE_B)//0XDFF
#define CP_VOL_STATE_B_LOW (2400 - ADC_OFFSET_STATE_B)//0XCC0

#define CP_VOL_STATE_C_LOW (2050 - ADC_OFFSET_STATE_C)//0XC20
#define CP_VOL_STATE_C_HIGH (2070 + ADC_OFFSET_STATE_C)//0XB00

#define CP_VOL_STATE_D_LOW (1823 - ADC_OFFSET_STATE_D)//0XA40
#define CP_VOL_STATE_D_HIGH (1823 + ADC_OFFSET_STATE_D)//0X8E5


#define DUTY100 100 // by default PWM is high
#define DUTY50 50
#define DUTY40 40
#define DUTY30 30
#define DUTY25 25
#define DUTY16 16
#define DUTY10 10

#define RELAY_ON 1
#define RELAY_OFF 0

#define HIGH 1
#define LOW 0

#define CPPWM_TASK_STACK_SIZE (4 * 1024) // 6000
#define CPPWM_TASK_PRIORITY 5

#define MINIMUM_CURRENT_SUPPORT 6
#define MAXIMUM_CURRENT_SUPPORT 30

#define CURRENT_LIMIT_6 6
#define CURRENT_LIMIT_9_6 10 //(9.6)
#define CURRENT_LIMIT_15 15
#define CURRENT_LIMIT_18 18
#define CURRENT_LIMIT_24 24
#define CURRENT_LIMIT_30 30

#define OVERVOLTAGE_LIMIT 270.0f
#define UNDERVOLTAGE_LIMIT 190.0f
#define ZEROCURRENT_LIMIT 0.5f
#define OVERCURRENT_LIMIT 33.0f
#define OVERTEMPRATURE_LIMIT 60

#define CP_UNDER_VOLTAGE 0
#define CP_UNDER_CURRENT 1
#define CP_OVER_CURRENT 2
#define CP_OVER_VOLTAGE 3
#define CP_MAX_CURRENT 4
#define Cp_UC_TIME_LIMIT 5 // under current (Zero current) time limit
#define CP_OVER_TEMPRATURE 6

#define MINCURRENTCHECKTIMER 120U
#define EARTHFAULTTIMER 5U

#define RFID_WITH_BUTTON_STR_LEN 5


#define ADC_VREF (3.3f)

#define CP_FACTOR (0.217f) //(0.225f)
/***************************************************************************
 * TYPES
 ***************************************************************************/
typedef struct
{
	uint32_t ui8CPMonAdcCount;
	uint32_t ui8PPMonAdcCount;
	uint32_t ui8CPMonNegAdcCount;
	float ADC_Voltage;
} __attribute__((__packed__)) J1772adc_st;

/**
 *  Charging mode Enumeration
 */
typedef enum ChargStartStopMode
{
	UI_START = 1,	/*!< Start From UI */
	SCHEDULE_START, /*!< Start from schedule */
	PB_START,		/*!< start form push button */
	UI_STOP = 6,	/*!< stop from ui */
	SCHEDULE_STOP,	/*!< schedule stop */
	PB_STOP			/*!< stop form push button  */
} __attribute__((packed)) ChargStartStopMode_e;

/**
 *  Charging Stop reason Enumeration
 */
typedef enum ChargStopReason
{
	NO_ERROR,
	FULL_CHARGE = 1, /*!< Full charged */
	GUN_REMOVED,	 /*!< gun removed */
	FAULT_OCCUER,	 /*!< fault occur */
	MANUAL_STOP,	 /*!< Manual stop */
	POWERLOSS,		 /*!< Powerloss */
	CHARGING_START,
	OTHER,		  /*!< Due to other reason(MG Car Lock/Unlock) */
	OVERCURRENT,  /*!< Over Current fault */
	OVERVOLTAGE,  /*!< Over voltage fault */
	UNDERVOLTAGE, /*!< under Current fault */
	ZEROCURRENT,
	ESTOP,
	EARTHFAULT,
	GUN_FAULT,
	CAR_ERROR_STATE,
	HIGH_TEMPRATURE,
	NO_REASON = 200
} __attribute__((packed)) ChargStopReason_e;
/**
 *  Charging Stop reason Enumeration
 */
typedef struct ev_error
{
	uint8_t StartCurrentChkTimmer;
	uint8_t CurrentChkTimmer;
} __attribute__((packed)) ev_error_t;

typedef struct fault_detections
{
	bool bTripSensorDetect;
	bool bEmergncyStopDetect;
	bool bCpFaultDetect;
	bool bEarthFaultDetect;
	bool bZeroCurrentDetect;
	bool bCarEstateDetect;
} __attribute__((packed)) fault_detections_t;
enum CP_Status
{
	Available = 'A',
	Preparing = 'B',
	Charging = 'C',
	SuspendedEV = 'D',
	SuspendedEVSE = 'E',
	Finishing = 'F',
	Reserved = 'G',
	Unavailable = 'H',
	Faulted = 'I'
} __attribute__((packed));
/***************************************************************************
 * GLOBAL VARIABLES
 ***************************************************************************/

/***************************************************************************
 * FUNCTIONS PROTOTYPES
 ***************************************************************************/
app_err_t ADC_ErrReadAdcOverI2C(void);
uint8_t ui8ReadCurrentEVstate(void);
float ADC_fReadValue(uint8_t ui8ADCNum);
uint32_t ADC_u8ReadCount(uint8_t ui8ADCNum);
/**
 * @brief: This Function set relay output
 *
 * @param[in] bool
 * @param[out] app_err_t
 */
app_err_t set_relay_output(bool status);
/**
 * @brief: This Function initialize pwm monitor task.
 *
 * @param[in] void
 * @param[out] void
 */
bool cppwm_TaskInit(void);
/**
 * @brief: This Function set current limit from user input
 *
 * @param[in] void
 * @param[out] void
 */
void DutyCycle_from_Current(uint8_t PwmDuty);
/**
 * @brief: This Function set Update charging state to UI
 *
 * @param[in] uint8_t
 * @param[out] void
 */
void UpdateChargingState(uint8_t Current_state);
/**
 * @brief: This Function read and retun start
 *         command status from UI /Push button
 *
 * @param[in] void
 * @param[out] uint8_t status
 */
uint8_t CheckStartCmdStatus(void);

/**
 * @brief: This Function will reset fault information for next start
 *
 * @param[in] void
 * @param[out] void
 */
void ResetFaultInfo(void);

/**
 * @brief: This Function check over voltage condition while charging is going on
 *
 * @param[in] float voltage
 * @param[out] uint8_t error code
 */
uint8_t Check_OverVoltage(float voltage);
/**
 * @brief: This Function check under voltage condition while charging is going on
 *
 * @param[in] float voltage
 * @param[out] uint8_t error code
 */
uint8_t Check_UnderVoltage(float voltage);
/**
 * @brief: This Function check over current condition while charging is going on
 *
 * @param[in] float current
 * @param[out] uint8_t error code
 */
uint8_t Check_OverCurrent(float current);
/**
 * @brief: This Function check under current condition while charging is going on
 *
 * @param[in] float current
 * @param[out] uint8_t error code
 */
uint8_t Check_UnderCurrent(float current);
/**
 * @brief: This Function check all fault condition while charging is going on
 *
 * @param[in] float voltage
 *  @param[in] uint16_t current
 *  @param[in] uint8_t connectorId
 * @param[out] uint8_t error code
 */
uint8_t Check_Fault(float voltage, float current, int16_t temprature, uint8_t connectorId);
/**
 * @brief: This Function to set charging start time
 *
 * @param[in] : uint8_t
 *
 * @param[out] : void
 */
void set_chargingstart_time(void);
/**
 * @brief: This is CP pwm task handler.
 *
 * @param[in] arg
 *
 * @param[out] void
 */
void cp_pwmTask(void *arg);
/***************************************************************************
 * END OF FILE
 ***************************************************************************/
#endif
/************************************************************************
 * END OF FILE
 ************************************************************************/
