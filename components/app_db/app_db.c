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
 * \file     app_db.c
 * \brief
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <zephyr/kernel.h>
#include "components/general/general.h"
#include "components/meter_ade7593/ade7593.h"
#include "zephyr/logging/log.h"
#include "app_db.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define INVALID_NVS_HANDLER (0U)
#define MAX_NVS_HANDLER (3U) ///< Max Number NVS handler store
#define TAG "app_datalogger"
#define MAX_NO_HISTORY_LOG 30U
#define MAX_NO_HISTORY_ENERGY 32U
#define MAX_NO_MONTHLY_HISTORY_LOG 13U          /*!< Yearly History Max Energy */
#define TIMIOUT_DELAY   K_MSEC(3000U)


LOG_MODULE_REGISTER(appDB,LOG_LEVEL_DBG);
//K_MUTEX_DEFINE(lockGlobalData);
//K_MUTEX_DEFINE(nvsMutexlock);
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
struct k_mutex lockGlobalData; /*!< Mutex lock for access global data */
struct k_mutex nvsMutexlock; /*!< spiff Mutex lock */
/** application Configuration global structure */
app_ConfigBlock_t configData = {0};   /*!< application configuration structure object */
LiveDataBlock_t   liveData   = {0};   /*!< application live data structure object */
uint8_t idCount;
extern History_Logdata_t StartHistoryLog;
extern uint32_t StartEnergy;
extern spimeterData_st R_Phase, B_Phase, Y_Phase;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
/**
 * @brief: This Function Get heartbeat Information From Structure
 */
app_err_t GetFaultInfo(fault_info_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			(void)memcpy(data, &liveData.faultinfo, sizeof(fault_info_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Update heartbeat Information To Structure
 */
app_err_t UpdateFaultInfo(fault_info_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			// printk( "************Fault data**********%d",data->err_code);
			(void)memcpy(&liveData.faultinfo, data, sizeof(fault_info_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Get spi metering Information From Structure
 */

app_err_t GetSPIMeteringInfo(Metering_info_spi_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			// (void)memcpy(data, &liveData.spimeterinfo, sizeof(Metering_info_spi_t));
			data->RPhase_voltage = R_Phase.u32Voltage;
			data->RPhase_current = R_Phase.u32Current;
			data->RPhase_power = R_Phase.u32Power;
			data->RPhase_session_energy = R_Phase.u32Energy;
			// printk( ">>>>>>energy meter R_Phase.u16Voltage %f R_Phase.u16Current %f RPhase_session_energy %f =",
			// 		 data->RPhase_voltage, data->RPhase_current, data->RPhase_session_energy);

			data->YPhase_voltage = Y_Phase.u32Voltage;
			data->YPhase_current = Y_Phase.u32Current;
			data->YPhase_power = Y_Phase.u32Power;
			data->YPhase_session_energy = Y_Phase.u32Energy;
			// printk( ">>>>>>energy meter Y_Phase.u16Voltage %f Y_Phase.u16Current %f YPhase_session_energy %f =",
			// 		 data->YPhase_voltage, data->YPhase_current, data->YPhase_session_energy);

			data->BPhase_voltage = B_Phase.u32Voltage;
			data->BPhase_current = B_Phase.u32Current;
			data->BPhase_power = B_Phase.u32Power;
			data->BPhase_session_energy = B_Phase.u32Energy;
			// printk( ">>>>>>energy meter B_Phase.u16Voltage %f B_Phase.u16Current %f BPhase_session_energy %f =",
			// 		 data->BPhase_voltage, data->BPhase_current, data->BPhase_session_energy);
            k_mutex_unlock(&lockGlobalData);
		}
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}
/**
 * @brief: This Function Update spi metering Information To Structure
 */
app_err_t UpdateSPIMeteringInfo(Metering_info_spi_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			(void)memcpy(&liveData.spimeterinfo, data, sizeof(Metering_info_spi_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Get heartbeat Information From Structure
 */
app_err_t GetHeartbeatInfo(heartbeat_info_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			(void)memcpy(data, &liveData.heartbeatInfo, sizeof(heartbeat_info_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Update heartbeat Information To Structure
 */
app_err_t UpdateHeartbeatInfo(heartbeat_info_t *data)
{
	app_err_t ret = NRF_OK;
	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			// printk( "************Heartbeatdata**********%d  %d  %u  %f",
			// 		 data->ev_connection_status,
			// 		 data->ev_current_state,
			// 		 data->sys_tm,
			// 		 data->total_energy);
			(void)memcpy(&liveData.heartbeatInfo, data, sizeof(heartbeat_info_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Get schedule Information From configData Structure
 */
app_err_t GetScheduleInfo(app_scheduler_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			(void)memcpy(data, &configData.scheduler_info, sizeof(app_scheduler_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function Update schedule Information To configData Structure
 */
app_err_t UpdateScheduleInfo(app_scheduler_t *data)
{
	app_err_t ret = NRF_OK;

	if (NULL != data)
	{
		if (k_mutex_lock(&lockGlobalData,  TIMIOUT_DELAY) == NRF_OK)
		{
			printk("data_______________%d  %d  %d  %d  %d  %d",
					 data->is_schedule,
					 data->wd_Starttm,
					 data->wd_Endtm,
					 data->we_Starttm,
					 data->we_Endtm,
					 data->sch_current_limit);
			(void)memcpy(&configData.scheduler_info, data, sizeof(app_scheduler_t));
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}

/**
 * @brief: This Function flush config data into NVS storage
 */
app_err_t flush_ConfigData(void)
{
	return (NvsWriteConfiguration(configData));
}

/**
 * @brief: This Function Write Configuration data into NVS
 */
app_err_t NvsWriteConfiguration(app_ConfigBlock_t data)
{
	app_err_t err = NRF_FAIL;


		if (k_mutex_lock(&nvsMutexlock, TIMIOUT_DELAY) == NRF_OK)
		{
			//err = nvs_set_blob(StorageHandle, CONFIGURATION_DATA_BLOB_NAME, &data, sizeof(data));
			if (NRF_OK != err)
			{
				printk( "Failed to set CONFIGURATION_DATA_BLOB_NAME, Error = %d", err);
			}
			else
			{
				//err = nvs_commit(StorageHandle);
			}
		}
		k_mutex_unlock(&nvsMutexlock);

	return err;
}

/**
 * @brief: This Function Read Configuration data from NVS
 */
app_err_t NvsReadConfiguration(app_ConfigBlock_t *configDataRead)
{
	app_err_t err = NRF_FAIL;

	if (NULL != configDataRead)
	{
		if (k_mutex_lock(&nvsMutexlock, TIMIOUT_DELAY) == NRF_OK)
		{
			size_t required_size = 0;
			//err = nvs_get_blob(StorageHandle, CONFIGURATION_DATA_BLOB_NAME, NULL, &required_size);
			if (NRF_OK == err)
			{
				if (required_size > 0)
				{
					//size_t DBConfigSize = sizeof(app_ConfigBlock_t);
					//err = nvs_get_blob(StorageHandle, CONFIGURATION_DATA_BLOB_NAME, configDataRead, &DBConfigSize);
					if (NRF_OK != err)
					{
						printk( "Failed to get CONFIGURATION_DATA_BLOB_NAME, Error = %d", err);
					}
				}
			}
		}
		k_mutex_unlock(&nvsMutexlock);
	}

	return err;
}

/**
 * @brief: This Function initialize database
 */
app_err_t nvsDatabaseInit(void)
{
	app_err_t err = NRF_FAIL;
	int16_t nvsMutexStatus = 0U;
	int16_t lockGlobalDataStatus = 0U;
	/** Mutex initialization */
	lockGlobalDataStatus = k_mutex_init(&lockGlobalData);
	nvsMutexStatus= k_mutex_init(&nvsMutexlock);

	if ((0 == nvsMutexStatus) && (0 == lockGlobalDataStatus))
	{
		//open common NVS Storage
//		err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &StorageHandle);
		if (NRF_OK != err)
		{
			printk( "NVS Open Storage fail.");
			return err;
		}
		else
		{
			/** Restore data from non-volatile storage */
			RestoreNVSData();
		}
		printk( "NVS Open Storage Successfully");
	}
	NvsReadConfiguration(&configData);
	return err;
}

/**
 * @brief: This Function restore data from nvs storage
 * and fill respective structure
*/
app_err_t RestoreNVSData(void)
{
    app_err_t err = NRF_FAIL;

    //err = nvsReadLastDate(&nvs_LastDate);
    if (NRF_OK != err)
    {
        printk( "Failed to nvsReadLastDate, Error = %d", err);
    }
    //err = nvsReadLastMonth(&nvs_LastMonth);
    if (NRF_OK != err)
    {
        printk( "Failed to nvsReadLastMonth, Error = %d", err);
    }
   
	#if 0
	 energy3ph_info_t energy3ph = {0};
    err = NvsReadLive3phaseEnergyBlock(&energy3ph);
    if (NRF_OK != err)
    {
        printk( "Failed to NvsReadLive3phaseEnergyBlock, Error = %d", err);
    }
    vReadEnergyStartValueNVS((float)energy3ph.RPhase_total_energy, (float)energy3ph.YPhase_total_energy, (float)energy3ph.BPhase_total_energy);
   #endif
    return err;
}
/**
 * @brief: API to read last date from NVS
 *
 * @param[in] uint32_t* LastDate : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t nvsReadLastDate(uint32_t* LastDate)
{
    app_err_t err = NRF_FAIL;
   // err = nvs_get_u32(StorageHandle, LASTDATE_KEY, LastDate);
    //if (ESP_ERR_NVS_NOT_FOUND == err)
    {
       // err = nvs_set_u32(StorageHandle, LASTDATE_KEY, *LastDate);
        if (NRF_OK != err)
        {
            printk( "Failed to set CYCLE_COUNT, Error = %d", err);
        }
        else
        {
            //err = nvs_commit(StorageHandle);
        }
    }
    return err;
}
/**
 * @brief: API to read last Month from NVS
 *
 * @param[in] uint32_t* LastDate : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t nvsReadLastMonth(uint32_t* LastMonth)
{
    app_err_t err = NRF_FAIL;
    //err = nvs_get_u32(StorageHandle, LASTMONTH_KEY, LastMonth);
    //if (ESP_ERR_NVS_NOT_FOUND == err)
    {
        //err = nvs_set_u32(StorageHandle, LASTMONTH_KEY, *LastMonth);
        if (NRF_OK != err)
        {
            printk( "Failed to set CYCLE_COUNT, Error = %d", err);
        }
        else
        {
           // err = nvs_commit(StorageHandle);
        }
    }
    return err;
}
/**
 * @brief: API to write last date to NVS
 *
 * @param[in] uint32_t LastDate
 * @param[out] return : app_err_t
 */
app_err_t nvsWriteLastDate(uint32_t LastDate)
{
    app_err_t err = NRF_FAIL;
    //err = nvs_set_u32(StorageHandle, LASTDATE_KEY, LastDate);
    if (NRF_OK != err)
    {
        printk( "Failed to set last date, Error = %d", err);
    }
    else
    {
        //err = nvs_commit(StorageHandle);
    }
    return err;
}
/**
 * @brief: API to write last Month to NVS
 *
 * @param[in] uint32_t LastMonth
 * @param[out] return : app_err_t
 */
app_err_t nvsWriteLastMonth(uint32_t LastMonth)
{
    app_err_t err = NRF_FAIL;
    //err = nvs_set_u32(StorageHandle, LASTMONTH_KEY, LastMonth);
    if (NRF_OK != err)
    {
        printk( "Failed to set last date, Error = %d", err);
    }
    else
    {
       // err = nvs_commit(StorageHandle);
    }
    return err;
}
/**
 * @brief: API to Write live data into NVS
 */
app_err_t NvsWriteLiveEnergyBlock(energy_info_t energyData)
{
    app_err_t err = NRF_FAIL;
       if(k_mutex_lock(&nvsMutexlock, TIMIOUT_DELAY) == NRF_OK)
        {
            //err = nvs_set_blob(StorageHandle, LIVE_DATA_BLOB_NAME, &energyData, sizeof(energy_info_t));
            if (NRF_OK != err)
            {
                printk("Failed to set LIVE_DATA_BLOB_NAME, Error = %d",err);
            }
            else
            {
                //err = nvs_commit(StorageHandle);
            }
        }
        k_mutex_unlock(&nvsMutexlock);

    return err;
}
/**
 * @brief: API to Read live data from NVS
 */
app_err_t NvsReadLiveEnergyBlock(energy_info_t* energyData)
{
    app_err_t err = NRF_FAIL;
    if (NULL != energyData)
    {
        if(k_mutex_lock(&nvsMutexlock, TIMIOUT_DELAY) == NRF_OK)
        {
            size_t required_size = 0;
           // err = nvs_get_blob(StorageHandle, LIVE_DATA_BLOB_NAME, NULL, &required_size);
            if (NRF_OK == err)
            {
                if (required_size > 0)
                {
                    //size_t dataSize = sizeof(energy_info_t);
                    //err = nvs_get_blob(StorageHandle, LIVE_DATA_BLOB_NAME, energyData, &dataSize);
                    if (NRF_OK != err)
                    {
                        printk("Failed to get LIVE_DATA_BLOB_NAME, Error = %d",err);
                    }
                }
            }
        }
        k_mutex_unlock(&nvsMutexlock);
    }
    return err;
}
/**
 * @brief: API to read automode status from NVS
 *
 * @param[in] uint8_t* mode : Pointer to hold stored value
 * @param[out] return : app_err_t
 */
app_err_t GetAutoModeStatus(uint8_t *Status)
{
	app_err_t ret = NRF_OK;

	if (NULL != Status)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			//(void)memcpy(Status, &configData.AutoModeStatus, sizeof(Status));
            *Status = configData.AutoModeStatus;
		}
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}
/**
 * @brief: API to write automode status to NVS
 *
 * @param[in] uint8_t mode
 * @param[out] return : app_err_t
 */
app_err_t UpdateAutoModeStatus(uint8_t *Status)
{
	app_err_t ret = NRF_OK;

	if (NULL != Status)
	{
		if (k_mutex_lock(&lockGlobalData, TIMIOUT_DELAY) == NRF_OK)
		{
			//(void)memcpy(&configData.AutoModeStatus, Status, sizeof(Status));
            configData.AutoModeStatus = *Status;
            printk( "AutoModeStatus :  %d",configData.AutoModeStatus);
		}
		//ret = flush_ConfigData();
		k_mutex_unlock(&lockGlobalData);
	}
	else
	{
		ret = NRF_FAIL;
	}
	return ret;
}


/****************************************************************************
 * END OF FILE
 ****************************************************************************/
