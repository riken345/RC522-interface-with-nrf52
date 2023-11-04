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
/*!
 * \file     ssd1306.h
 * \brief    Filename : ssd1306.h Function prototypes for the OLED related APIs.
 */

#ifndef _SSD1306_H
#define _SSD1306_H
/************************************************************************
 * Include Header Files
 ************************************************************************/

/************************************************************************
 * Define Macros
 ************************************************************************/
#define TIMIOUT_DELAY   K_MSEC(3000U)
#define CURRENT_LIMIT_6    6
#define CURRENT_LIMIT_9_6 10   //(9.6)
#define CURRENT_LIMIT_15 15
#define CURRENT_LIMIT_18 18
#define CURRENT_LIMIT_24 24
#define CURRENT_LIMIT_30 30

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
uint8_t ssd1306_oled_init();
uint8_t ssd1306_oled_print();

void OLED_Current_limit_Print(int Limit);
void OLED_time_Print(int sec);
void oledstartupdisplayprint(void);

void OledPrintData(void);
void display_init(void);
void display_play(void);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
