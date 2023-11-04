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
 * \file     led_handler.c
 * \brief
 */

/************************************************************************
 * INCLUDES
 ************************************************************************/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include "led_handler.h"
#include "components/ioExp_mcp23s17/ioExp_mcp23s17.h"
#include "components/app_db/app_db.h"
#include "zephyr/logging/log.h"
/****************************************************************************
 * MACROS
 ****************************************************************************/
#define GPIO_TASK_STACK_SIZE (2 * 1024)
#define GPIO_TASK_PRIORITY 10

#define LED_LOW 1
#define LED_HIGH 0
#define OFF_LED 0
#define DELAY_TICK_100 100

#define BLUE_LED_STEADY 1
#define BLUE_LED_BLINK 11
#define GREEN_LED_STEADY 2
#define GREEN_LED_BLINK 12
#define RED_LED_STEADY 4
#define RED_LED_BLINK 14

#define BLINK_ALL_LED 17

LOG_MODULE_REGISTER(ledHandler, LOG_LEVEL_DBG);
/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
uint8_t u8LEDColor[eLED_MaxState];
/****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/
static struct k_thread gpio_task_handle;
static K_KERNEL_STACK_DEFINE(gpio_task_stack, GPIO_TASK_STACK_SIZE);

//extern const k_tid_t gpio_task_handle; /**< Reference to schedule FreeRTOS task. */
//K_THREAD_DEFINE(gpio_task_handle, GPIO_TASK_STACK_SIZE, LEDHANDLER_Task, NULL, NULL, NULL, GPIO_TASK_PRIORITY, 0, 0);
/****************************************************************************
 * FUNCTIONS
 ****************************************************************************/
void LEDHANDLER_Task(void *arg)
{
	uint8_t ui8EVState = 100;
	heartbeat_info_t hb_info = {0U};
	LED_vLEDMapping();
	while (1)
	{
		// LOG_DBG( "!!!!!!!!!!!!!! LEDHANDLER_Task !!!!!!!!!!!!!!\n");
		GetHeartbeatInfo(&hb_info);
		ui8EVState = hb_info.ev_current_state;

		switch (ui8EVState)
		{
		case Available:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDIdle], __LINE__);
		}
		break;
		case Preparing:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDReadyToCharge], __LINE__);
		}
		break;
		case Faulted:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDFault], __LINE__);
		}
		break;
		case Finishing:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDCharging_done], __LINE__);
		}
		break;
		case Charging:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDCharging], __LINE__);
		}
		break;
		case Reserved:
		{
			LED_vSetOrBlink(u8LEDColor[eLEDReserved], __LINE__);
		}
		break;
		default:
		{
			//LED_vSetOrBlink(u8LEDColor[eLEDPoweron], __LINE__);
		}
		break;
		}
		k_msleep((DELAY_TICK_100 * 8));
	}
}

void LEDHANDLER_TaskInit(void)
{
		k_thread_create(&gpio_task_handle, gpio_task_stack,
					K_KERNEL_STACK_SIZEOF(gpio_task_stack),
					(k_thread_entry_t)LEDHANDLER_Task, NULL, NULL, NULL,
					K_PRIO_COOP(GPIO_TASK_PRIORITY),
					0, K_NO_WAIT);
	k_thread_name_set(&gpio_task_handle, "gpio task");
}

void LED_vSetOrBlink(uint8_t u8color, int ilineno)
{
	uint8_t u8BlinkColor = 0;
	if (u8color > 10)
	{
		u8BlinkColor = u8color - 10;
		LED_vBlinkLED(u8BlinkColor);
	}
	else
	{
		LED_vSetLEDColor(u8color, ilineno);
	}
}
void LED_vBlinkLED(uint8_t u8color)
{
	static bool bBlink = true;
	if (true == bBlink)
	{
		LED_vSetLEDColor(u8color, __LINE__);
	}
	else
	{
		LED_vSetLEDColor(OFF_LED, __LINE__);
	}
	bBlink = !bBlink;
}
void LED_vSetLEDColor(uint8_t u8color, int ilineno)
{
	bool bRedLED = 0, bGreenLED = 0, bBlueLED = 0;
	bBlueLED = u8color & 0x1;
	bGreenLED = (u8color >> 1) & 0x1;
	bRedLED = (u8color >> 2) & 0x1;
	LED_vSetRGBLED(bRedLED, bGreenLED, bBlueLED);
}
void LED_vSetRGBLED(bool bRed, bool bGreen, bool bBlue)
{
	if (NRF_OK != MCP23017_IOExpGpioLevelSet(GPB0_RED, bRed))
	{
		LOG_ERR("fail to set ESP_LED1 level:%d", bRed);
	}
	if (NRF_OK != MCP23017_IOExpGpioLevelSet(GPB1_GREEN, bGreen))
	{
		LOG_ERR("fail to set ESP_LED2 level:%d", bGreen);
	}
	if (NRF_OK != MCP23017_IOExpGpioLevelSet(GPB2_BLUE, bBlue))
	{
		LOG_ERR("fail to set ESP_LED3 level:%d", bBlue);
	}
}
void LED_vLEDMapping(void)
{
	u8LEDColor[eLEDIdle] = GREEN_LED_STEADY;			// Available
	u8LEDColor[eLEDReadyToCharge] = BLUE_LED_BLINK; // Preparing
	u8LEDColor[eLEDCharging] = GREEN_LED_BLINK;		// Charging
	u8LEDColor[eLEDFault] = RED_LED_BLINK;			// Fault
	u8LEDColor[eLEDPoweron] = BLINK_ALL_LED;		// Poweron blink all led
	u8LEDColor[eLEDCharging_done] = GREEN_LED_STEADY;
	u8LEDColor[eLEDReserved] = RED_LED_STEADY;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/