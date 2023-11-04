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
 * \file     ioExp_mcp23s17.h
 * \brief    Filename : ioExp_mcp23s17.h Function prototypes for the I2C Expander related APIs.
 */

#ifndef _IOEXP_MCP23S17_H
#define _IOEXP_MCP23S17_H
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "components/i2c_services/i2c_service.h"
#include "components/general/general.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define IO_EXPANDER_ADDRESS 0x20
#define ONE_BYTE_LENGTH 1U
//TODO: Replace actual GPIO here
#define ESTOP_INPUT 9

#define INPUT             0x01
#define OUTPUT            0x03
#define PULLUP            0x04
#define INPUT_PULLUP      0x05
#define PULLDOWN          0x08
#define INPUT_PULLDOWN    0x09
#define OPEN_DRAIN        0x10
#define OUTPUT_OPEN_DRAIN 0x12
#define ANALOG            0xC0

typedef enum MCP23017_PinName{
		GPA0_MCU_BRD_ID1 = 0,
		GPA1_MCU_BRD_ID2,
		GPA2_MCU_BRD_ID3,
		GPA3_MCU_BRD_ID4,
		GPA4_OVP1,
		GPA5_RLY1,
		GPA6_PB1,
		GPA7_PB2,
		GPB0_RED = 8,
		GPB1_GREEN,
		GPB2_BLUE,
		GPB3_RFID_RST,
		GPB4_GND_FLT,
		GPB5_MET_IRQ,
		GPB6_MET_RESET,
		GPB7_EL
} __attribute__((packed)) MCP23017_PinName_e;
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/
typedef enum IOExpanderCmd
{
      IODIR_A		= 0x00, 		///< Controls the direction of the data I/O for port A.
      IPOL_A		= 0x02,			///< Configures the polarity on the corresponding GPIO_ port bits for port A.
      GPINTEN_A         =  0x04,		///< Controls the interrupt-on-change for each pin of port A.
      DEFVAL_A          = 0x06,			///< Controls the default comparaison value for interrupt-on-change for port A.
      INTCON_A          = 0x08,			///< Controls how the associated pin value is compared for the interrupt-on-change for port A.
      GPPU_A		= 0x0C,			///< Controls the pull-up resistors for the port A pins.
      INTF_A		= 0x0E,			///< Reflects the interrupt condition on the port A pins.
      INTCAP_A          = 0x10,			///< Captures the port A value at the time the interrupt occured.
      GPIO_A		= 0x12,			///< Reflects the value on the port A.
      OLAT_A		= 0x14,			///< Provides access to the port A output latches.

	IODIR_B		= 0x01,			///< Controls the direction of the data I/O for port B.
	IPOL_B		= 0x03,			///< Configures the polarity on the corresponding GPIO_ port bits for port B.
	GPINTEN_B	= 0x05,			///< Controls the interrupt-on-change for each pin of port B.
	DEFVAL_B	= 0x07,			///< Controls the default comparaison value for interrupt-on-change for port B.
	INTCON_B	= 0x09,			///< Controls how the associated pin value is compared for the interrupt-on-change for port B.
	GPPU_B		= 0x0D,			///< Controls the pull-up resistors for the port B pins.
	INTF_B		= 0x0F,			///< Reflects the interrupt condition on the port B pins.
	INTCAP_B	= 0x11,			///< Captures the port B value at the time the interrupt occured.
	GPIO_B		= 0x13,			///< Reflects the value on the port B.
	OLAT_B		= 0x15,			///< Provides access to the port B output latches.

        IOCON		= 0x0A,			///< Controls the device.
} __attribute__((packed)) IOExpanderCmd_e;
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
 * @brief:Init I/O Expander
 *
 * @param[in]  void
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t MCP23017_ioExpInit(void);
/**
 * @brief: Write Data to I/O Expander
 *
 * @param[in]  regAddr             Register Address
 * @param[in]  write_data           data pointer
 * @param[in]  len                 length of read data
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t MCP23017_ioExpWriteData(uint8_t reg_addr, uint8_t write_data, uint8_t len);

/**
 * @brief: Read Data From I/O Expander
 *
 * @param[in]  regAddr             Register Address
 * @param[in]  reg_data            data pointer
 * @param[in]  len                 length of read data
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
 */
app_err_t MCP23017_ioExpReadData(uint8_t reg_addr, uint8_t *reg_data, uint8_t len);

/**
 * @brief  Reset IOExpander by external GPIO line
 *
 * @param[in]  void
 *
 * @param[out] app_err_t    return NRF_OK or NRF_FAIL
 */
app_err_t MCP23017_RstIOExpander(void);

/**
 * \fn MCP23017_IOExpGpioLevelSet(uint8_t gpio,uint32_t level)
 * \brief  Set GPIO level.
 *
 * \param  gpio GPIO number.
 * \param  level level to set. 1 or 0.
 *
 * \return
 *    - true: succeed
 *    - false: fail
 */
app_err_t MCP23017_IOExpGpioLevelSet(uint8_t gpio, uint32_t level);

/**
 * \fn MCP23017_IOExpGpioLevelGet(uint8_t gpio)
 * \brief  Get GPIO level.
 *
 * \param  gpio GPIO number.
 *
 * \return
 *    GPIO level, 0 or 1.
 */
int8_t MCP23017_IOExpGpioLevelGet(uint8_t gpio);
/**
 * \fn getEStopPinState(void)
 * \brief  Get EStop GPIO level.
 *
 * \param  void.
 *
 * \return
 *    GPIO level, 0 or 1.
 */
bool getEStopPinState(void);
/**
 * \fn MCP23017_pinMode(uint8_t pin, uint8_t mode, bool inverted);
 * \brief  set pin mode for IO expander pins
 *
 * \param  void.
 *
 * \return
 *    void
 */
void MCP23017_pinMode(uint8_t pin, uint8_t mode, bool inverted);
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#endif
