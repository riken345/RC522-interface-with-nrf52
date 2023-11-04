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
 * \file     eeprom_m24m02.h
 * \brief
 */

#ifndef EEPROM_M24M02_H
#define EEPROM_M24M02_H

/**
 * \headerfile ""
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "components/general/general.h"
/************************************************************************
 * Define Macros
 ************************************************************************/


/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

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
 * @brief: EEPROM self test
 *
 * @param[in]  void             void
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t M24M02_SelfTest(void);

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
app_err_t M24M02_WriteData(uint8_t reg_addr, uint8_t write_data, uint8_t len);

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
app_err_t M24M02_ReadData(uint8_t reg_addr, uint8_t *reg_data, uint8_t len);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
