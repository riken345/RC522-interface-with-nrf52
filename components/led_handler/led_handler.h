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
 * \file     led_handler.h
 * \brief
 */

#ifndef LED_HANDLER_H
#define LED_HANDLER_H
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <stdio.h>
#include "string.h"
#include "components/cp_pwm/cp_pwm.h"
/****************************************************************************
 * MACROS
 ****************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
typedef enum
{
	eLEDIdle = 0,
	eLEDReadyToCharge = 1,
	eLEDCharging = 2,
	eLEDFault = 3,
	eLEDReserved = 4,
	eLEDCharging_done = 5,
	eLEDPoweron = 6,
	eLED_MaxState
} __attribute__((__packed__)) LEDApplication;
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
void LEDHANDLER_Task(void *arg);
void LEDHANDLER_TaskInit(void);
void LED_vSetOrBlink(uint8_t u8color, int ilineno);
void LED_vBlinkLED(uint8_t u8color);
void LED_vSetLEDColor(uint8_t u8color, int ilineno);
void LED_vSetRGBLED(bool bRed, bool bGreen, bool bBlue);
void LED_vLEDMapping(void);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
