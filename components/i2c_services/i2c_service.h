/*
 *      Copyright (c) 2023
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *		@author: EV
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

/** \file i2c_service.h
 * \brief
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _I2C_SERVICE_H
#define _I2C_SERVICE_H

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <zephyr/kernel.h>
/****************************************************************************
 * MACROS
 ***************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
/**
 * i2c number enumeration
 */
typedef enum i2cNumber
{
	I2C1 = 0,
    I2C2,
}__attribute__((__packed__)) i2cNumber_e;
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
/**
 * @brief: This Function check All i2c connections
 *          and print address on console
 *
 * @param[in] chanelNo (i2c1/i2c2)
 * @param[out] void
 */
void I2C_vScanner(uint8_t chanelNo);
/**
 * @brief: This Function init All i2c connections
 *
 * @param[in] chanelNo (i2c1/i2c2)
 * @param[out] void
 */
void I2C_init(uint8_t chanelNo);
/**
 * @brief: This Function write i2c bytes
 *
 * @param[in] chanelNo (i2c1/i2c2)
 *    @param[in] slave_addr
 *  * @param[in] regaddr
 *  * @param[in] reg_data
 *  * @param[in] len
 */
uint8_t I2C_u8writeByte(uint8_t chanelNo,uint8_t slave_addr,uint8_t regaddr, uint8_t reg_data, uint16_t len);
/**
 * @brief: This Function init All i2c connections
 *
 * @param[in] chanelNo (i2c1/i2c2)
 *    @param[in] slave_addr
 *  * @param[in] regaddr
 *  * @param[in] reg_data
 *  * @param[in] len
 */
uint8_t I2C_u8readByte(uint8_t chanelNo,uint8_t slave_addr,uint8_t regaddr, uint8_t *reg_data, uint16_t len);
/**
 * @brief: This Function write i2c bytes
 *
 * @param[in] chanelNo (i2c1/i2c2)
 *    @param[in] slave_addr
 *  * @param[in] regaddr
 *  * @param[in] reg_data
 *  * @param[in] len
 */
uint8_t I2C_u8writeByte_16(uint8_t chanelNo,uint8_t slave_addr,uint16_t regaddr, uint8_t reg_data, uint16_t len);
/**
 * @brief: This Function init All i2c connections
 *
 * @param[in] chanelNo (i2c1/i2c2)
 *    @param[in] slave_addr
 *  * @param[in] regaddr
 *  * @param[in] reg_data
 *  * @param[in] len
 */
uint8_t I2C_u8readByte_16(uint8_t chanelNo,uint8_t slave_addr,uint16_t regaddr, uint8_t *reg_data, uint16_t len);

#ifdef __cplusplus
}
#endif
#endif
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
