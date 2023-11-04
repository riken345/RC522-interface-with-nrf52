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

/** \file gpio_config.h
 * \brief
 *
 **/
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _GPIO_CONFIG_H
#define _GPIO_CONFIG_H

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <zephyr/kernel.h>
/****************************************************************************
 * MACROS
 ***************************************************************************/
#define HIGH 1
#define LOW  0
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
/**
 * GPIO enumeration
 */
typedef enum gpioNo
{
    OP_IORST = 0,
    OP_RLY1_EN,
    OP_SPI1_CS1,
    OP_SPI1_CS2,

    IP_ESTOP = 10,
    IP_MET1_ZX ,
    IP_MET1_REVP ,
    IP_MET1_ZX_I ,
    IP_RTC_MFP ,
    IP_RFID_IRQ,
}__attribute__((__packed__)) gpioNo_e;
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/

/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
/**
 * @brief: This Function init All i2c connections
 *
 * @param[in] chanelNo (i2c1/i2c2)
 * @param[out] void
 */
void GPIO_Toggle(uint8_t gpioNo);
void GPIO_OnOff(uint8_t gpioNo, bool status);
void GPIO_vinit(void);
#ifdef __cplusplus
}
#endif
#endif
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
