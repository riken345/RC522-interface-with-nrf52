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
 * \file     app_db.h
 * \brief
 */

#ifndef APP_DB_H
#define APP_DB_H

/**
 * \headerfile ""
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "components/general/general.h"
#include "components/app_schedule/app_schedule.h"
/************************************************************************
 * Define Macros
 ************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
/**
 *  Energy information
 */
typedef struct energy_info
{
    float cumulative_energy;
} __attribute__((packed)) energy_info_t;
/**
 *  Heartbeat information
 */
typedef struct heartbeat_info
{
    uint8_t ev_connection_status;
    uint8_t ev_current_state;
    uint32_t sys_tm;
    float total_energy;
} __attribute__((packed)) heartbeat_info_t;
/**
 *  fault information
 */
typedef struct fault_info
{
    uint16_t err_code;
} __attribute__((packed)) fault_info_t;
/**
 *  History Event information
 */
typedef struct History_Energydata
{
    uint16_t event_id;
    uint64_t timestamp;
    float energy;
} __attribute__((packed)) History_Energydata_t;
/**
 *  History Log information
 */
typedef struct History_Logdata
{
    uint64_t timestamp;     // Start time as per PRD
    uint16_t time_duration; // Session time as per PRD
    float energy;           // Energy usage as per PRD
    uint16_t event_id;      // Counter for save data in EEPROM
    uint8_t rfidTag[10];    //RFID Tag as per PRD
    uint16_t userId;        //User iD as per PRD
    uint8_t event_code;     
} __attribute__((packed)) History_Logdata_t;
/**
 *  find History entry information
 */
typedef struct ReadHistoryEntry
{
    uint16_t start_record;
    uint16_t end_record;
    uint8_t type;
} __attribute__((packed)) ReadHistoryEntry_t;
/**
 *  SPI Metering information
 */
typedef struct Metering_info_spi
{
    float RPhase_power;
    float RPhase_session_energy;
    float RPhase_voltage;
    float RPhase_current;
    float YPhase_power;
    float YPhase_session_energy;
    float YPhase_voltage;
    float YPhase_current;
    float BPhase_power;
    float BPhase_session_energy;
    float BPhase_voltage;
    float BPhase_current;
} __attribute__((packed)) Metering_info_spi_t;

/**
 *  Energy information
 */
typedef struct energy3ph_info
{
    float RPhase_total_energy;
    float YPhase_total_energy;
    float BPhase_total_energy;
} __attribute__((packed)) energy3ph_info_t;

/**
 *  Configuration block
 */
typedef struct app_ConfigBlock
{
    app_scheduler_t scheduler_info;
    uint8_t AutoModeStatus;
} __attribute__((packed)) app_ConfigBlock_t;
/**
 *  Live DataBlock Structure
 */
typedef struct LiveDataBlock
{
    heartbeat_info_t heartbeatInfo;   /*!< heartbeat information structure */
    fault_info_t faultinfo;           /*!< fault information structure */
    Metering_info_spi_t spimeterinfo; /*!< spi 3phase metering information structure */
} __attribute__((packed)) LiveDataBlock_t;
/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/

/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
/**
 * @brief: This Function Get schedule Information From configData Structure
 *
 * @param[in]   data            scheduler config structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t GetScheduleInfo(app_scheduler_t *data);

/**
 * @brief: This Function Update schedule Information To configData Structure
 *
 * @param[in]   data            scheduler config structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t UpdateScheduleInfo(app_scheduler_t *data);

/**
 * @brief: This Function Get heartbeat Information From liveData Structure
 *
 * @param[in]   data            heartbeat structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t GetHeartbeatInfo(heartbeat_info_t *data);

/**
 * @brief: This Function Update heartbeat Information To configData Structure
 *
 * @param[in]   data            heartbeat structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t UpdateHeartbeatInfo(heartbeat_info_t *data);

/**
 * @brief: This Function Get fault Information From liveData Structure
 *
 * @param[in]   data            fault structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t GetFaultInfo(fault_info_t *data);

/**
 * @brief: This Function Update fault Information To configData Structure
 *
 * @param[in]   data            fault structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t UpdateFaultInfo(fault_info_t *data);
/**
 * @brief: This Function Get metering Information From liveData Structure
 *
 * @param[in]   data            metering structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */

/**
 * @brief: This Function Get SPI metering Information From liveData Structure
 *
 * @param[in]   data            metering structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t GetSPIMeteringInfo(Metering_info_spi_t *data);
/**
 * @brief: This Function Update SPI metering Information To configData Structure
 *
 * @param[in]   data            metering structure pointer
 * @param[out]  app_err_t       return NRF_OK or NRF_FAIL
 */
app_err_t UpdateSPIMeteringInfo(Metering_info_spi_t *data);

/**
 * @brief: This Function flush config data into NVS storage
 *
 * @param[in] void
 * @param[out] app_err_t    return NRF_OK or NRF_FAIL
 */
app_err_t flush_ConfigData(void);

/**
 * @brief: This Function Write Configuration data into NVS
 *
 * @param[in] configData            configuration data object
 * @param[out] app_err_t            return NRF_OK or NRF_FAIL
 */
app_err_t NvsWriteConfiguration(app_ConfigBlock_t configData);

/**
 *  @brief: This Function initialize database
 *  @param[in] void
 *  @param[out] app_err_t      return NRF_OK or NRF_FAIL
 */
app_err_t nvsDatabaseInit(void);

/**
 * @brief: This Function restore data from nvs storage
 *          and fill respective structure
 *  @param[in] void
 *  @param[out] app_err_t      return NRF_OK or NRF_FAIL
 */
app_err_t RestoreNVSData(void);
/**
 * @brief: API to read last date from NVS
 *
 * @param[in] uint32_t* LastDate : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t nvsReadLastDate(uint32_t *LastDate);
/**
 * @brief: API to read last Month from NVS
 *
 * @param[in] uint32_t* LastDate : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t nvsReadLastMonth(uint32_t *LastMonth);
/**
 * @brief: API to write last date to NVS
 *
 * @param[in] uint32_t LastDate
 * @param[out] return : app_err_t
 */
app_err_t nvsWriteLastDate(uint32_t LastDate);
/**
 * @brief: API to write last Month to NVS
 *
 * @param[in] uint32_t LastMonth
 * @param[out] return : app_err_t
 */
app_err_t nvsWriteLastMonth(uint32_t LastMonth);
/**
 * @brief: API to write energy data to NVS
 *
 * @param[in] energyData energyData
 * @param[out] return : app_err_t
 */
app_err_t NvsWriteLiveEnergyBlock(energy_info_t energyData);
/**
 * @brief: API to read energy data from NVS
 *
 * @param[in] energy_info_t* energyData
 * @param[out] return : app_err_t
 */
app_err_t NvsReadLiveEnergyBlock(energy_info_t *energyData);

/**
 * @brief: API to write automode to NVS
 *
 * @param[in] uint8_t mode
 * @param[out] return : app_err_t
 */
app_err_t UpdateAutoModeStatus(uint8_t *mode);
/**
 * @brief: API to read automode status from NVS
 *
 * @param[in] uint8_t* mode : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t GetAutoModeStatus(uint8_t *AutoMode_Status);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
