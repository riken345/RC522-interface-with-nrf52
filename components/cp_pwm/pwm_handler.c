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
/** \file pwm_handler.c
 * \brief Filename : pwm_handler.h Function prototypes for the PWM driver initialization.
 *
 **/
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "components/cp_pwm/pwm_handler.h"
#include "zephyr/drivers/pwm.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define PWM_CONSTANT 10000U
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/
static const struct pwm_dt_spec cp_pwm = PWM_DT_SPEC_GET(DT_ALIAS(cp_pwm));
/************************************************************************
 * Define Global Variables
 ************************************************************************/

/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
void PWM_init(void)
{
	if (!device_is_ready(cp_pwm.dev))
	{
		printk("Error: PWM device %s is not ready\n", cp_pwm.dev->name);
		return 0;
	}
}
void PWM_Start(uint8_t u8dutyCycle)
{
	//   printk("PWM_StartPWM_Start %d", u8dutyCycle);
	pwm_set_dt(&cp_pwm, PWM_KHZ(1), u8dutyCycle * PWM_CONSTANT);
}
void PWM_Stop(void)
{
	pwm_set_dt(&cp_pwm, PWM_KHZ(1), 100 * PWM_CONSTANT);
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/