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
 * \file     eeprom_m24m02.c
 * \brief
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <zephyr/kernel.h>
#include "eeprom_m24m02.h"
#include "zephyr/logging/log.h"
#include "components/gpio_config/gpio_config.h"
#include "components/i2c_services/i2c_service.h"
#include "components/general/general.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define EEPROM_ADDRESS 0xA0 // 7-bit address of the M24M02 EEPROM
#define TEST_ADDRESS 0x0001
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
/* Definitions for internal_meter */

/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/

/**
 * @brief: EEPROM self test
 *
 * @param[in]  void             void
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t M24M02_SelfTest(void)
{
    app_err_t err = NRF_OK;
    uint8_t data_to_write = 50;
    uint8_t read_data = 0;

    // Write data to the EEPROM
    //M24M02_WriteData(TEST_ADDRESS, data_to_write, sizeof(data_to_write));

    // Read data from the EEPROM
    M24M02_ReadData(TEST_ADDRESS, &read_data, sizeof(data_to_write));

    printk("Data read from EEPROM: %d\n", read_data);

    // Verify if the read data matches the written data
    if (data_to_write == read_data)
    {
        printk("EEPROM self-test passed!\n");
    }
    else
    {
        printk("EEPROM self-test failed!\n");
    }
    return err;
}

/**
 * @brief: Write Data to EEPROM
 *
 * @param[in]  regAddr             Register Address
 * @param[in]  write_data           data pointer
 * @param[in]  len                 length of read data
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t M24M02_WriteData(uint8_t reg_addr, uint8_t write_data, uint8_t len)
{
    app_err_t err = NRF_OK;
    err = I2C_u8writeByte_16(I2C1, EEPROM_ADDRESS, reg_addr, write_data, len);
    return err;
}

/**
 * @brief: Read Data From EEPROM
 *
 * @param[in]  regAddr             Register Address
 * @param[in]  reg_data            data pointer
 * @param[in]  len                 length of read data
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t M24M02_ReadData(uint8_t reg_addr, uint8_t *reg_data, uint8_t len)
{
    app_err_t err = NRF_OK;
    err = I2C_u8readByte_16(I2C1, EEPROM_ADDRESS, reg_addr, reg_data, len);
    return err;
}

/****************************************************************************
 * END OF FILE
 ****************************************************************************/
