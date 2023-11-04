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
/** \file pwm_handler.h
 * \brief Filename : pwm_handler.h Function prototypes for the PWM driver initialization.
 *
 **/
#ifndef PWM_HANDLER_H_
#define PWM_HANDLER_H_
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <stdint.h>
#include<string.h>

/************************************************************************
 * Define Macros
 ************************************************************************/
/**
 * \def MAX_PWM_CHANNEL
 * \brief MAX_PWM_CHANNEL Maximum supported PWM channel
 */
#define MAX_PWM_CHANNEL		GW_MAX_PWM_CHANNEL
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
 * \fn bac_PWM_Init(void)
 * @brief  PWM initialization
 *
 * \param  void
 *
 * \return
 *    - void
 */
void PWM_Init(void);

/**
 * \fn bac_PWM_Start(uint8_t u8dutyCycle)
 * @brief  Start PWM on specific channel
 *
 * \param  u8dutyCycle dutycycle percentage
 *
 * \return
 *    - NRF_OK
 *    - NRF_FAIL
 */
void PWM_Start(uint8_t u8dutyCycle);
/**
 * \fn bac_PWM_Stop(void)
 * @brief  Stop PWM on specific channel
 *
 * \param  void
 *
 * \return
 *    - void
 */
void PWM_Stop(void);


/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
