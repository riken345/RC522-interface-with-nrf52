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
 * \file     ioExp_mcp23s17.c
 * \brief
 */

 //README: https://github.com/blemasle/arduino-mcp23017
 //reference taken from abobe code
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include "ioExp_mcp23s17.h"
#include "components/gpio_config/gpio_config.h"
#include "zephyr/logging/log.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#define GPIO_LOW 0
#define GPIO_HIGH 1

LOG_MODULE_REGISTER(ioExpander,LOG_LEVEL_DBG);
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
uint8_t bit_stat_level_port0 = 0x00;
uint8_t bit_stat_level_port1 = 0x00;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
app_err_t MCP23017_IOExpGpioLevelSet(uint8_t gpio, uint32_t level)
{
    uint8_t bit = gpio;
    app_err_t err = NRF_OK;
    if (bit < GPB0_RED)
    {
        if (level)
        {
            bit_stat_level_port0 = (bit_stat_level_port0 | (1 << (bit)));
            err = MCP23017_ioExpWriteData(GPIO_A, bit_stat_level_port0, ONE_BYTE_LENGTH);
            if (NRF_FAIL == err)
            {
                printk("Failed to set GPIO, return Status: %d", err);
            }
        }
        else
        {
            bit_stat_level_port0 = (bit_stat_level_port0 & (~(1 << (bit))));
            err = MCP23017_ioExpWriteData(GPIO_A, bit_stat_level_port0, ONE_BYTE_LENGTH);
            if (NRF_FAIL == err)
            {
                printk("Failed to set GPIO, return Status: %d", err);
            }
        }
    }
    else
    {
        bit = bit - 8;
        if (level)
        {
            bit_stat_level_port1 = (bit_stat_level_port1 | (1 << (bit)));
            err = MCP23017_ioExpWriteData(GPIO_B, bit_stat_level_port1, ONE_BYTE_LENGTH);
            if (NRF_FAIL == err)
            {
                printk("Failed to set GPIO, return Status: %d", err);
            }
        }
        else
        {
            bit_stat_level_port1 = (bit_stat_level_port1 & (~(1 << (bit))));
            err = MCP23017_ioExpWriteData(GPIO_B, bit_stat_level_port1, ONE_BYTE_LENGTH);
            if (NRF_FAIL == err)
            {
                printk("Failed to set GPIO, return Status: %d", err);
            }
        }
    }
    return err;
}

int8_t MCP23017_IOExpGpioLevelGet(uint8_t gpio)
{
    uint8_t value = 0;

    if (gpio < GPB0_RED)
    {
        if (NRF_FAIL == MCP23017_ioExpReadData(GPIO_A, &value, ONE_BYTE_LENGTH))
        {
            return NRF_FAIL;
        }
        value = value & ((0x01) << gpio);
    }
    else
    {
        gpio = gpio - 8;
        if (NRF_FAIL == MCP23017_ioExpReadData(GPIO_B, &value, ONE_BYTE_LENGTH))
        {
            return NRF_FAIL;
        }
        printk("Value GPIO: %d %d", value, gpio);
        value = value & ((0x01) << gpio);
        printk("Value : %d", value);
    }
    if (value)
    {
        return HIGH;
    }
    else
    {
        return LOW;
    }
    return value;
}
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
app_err_t MCP23017_ioExpWriteData(uint8_t reg_addr, uint8_t write_data, uint8_t len)
{
    app_err_t err = NRF_OK;
    err = I2C_u8writeByte(I2C1,IO_EXPANDER_ADDRESS, reg_addr,write_data, len);
    return err;
}

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
app_err_t MCP23017_ioExpReadData(uint8_t reg_addr, uint8_t *reg_data, uint8_t len)
{
    app_err_t err = NRF_OK;
    err = I2C_u8readByte(I2C1,IO_EXPANDER_ADDRESS, reg_addr, reg_data, len);
    return err;
}

/**
 * @brief  Reset IOExpander by external GPIO line
 *
 * @param[in]  void
 *
 * @param[out] app_err_t    return NRF_OK or NRF_FAIL
 */
app_err_t  MCP23017_RstIOExpander(void)
{
    app_err_t err = NRF_OK;
    GPIO_OnOff(OP_IORST, HIGH);
    return err;
}

/**
 * @brief: This Function read DCHG Pin Status.
 *
 * @param[in] void
 *
 * @param[out] bool		return Pin Status
 */
bool getEStopPinState(void)
{
    uint8_t status = 0x00U;
    status = MCP23017_IOExpGpioLevelGet(ESTOP_INPUT);
    printk("status >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %d", status);
    return status;
}


/**
 * @brief:Init I/O Expander
 *
 * @param[in]  void
 *
 * @param[out]  app_err_t  return NRF_OK or NRF_FAIL
 *
*/
app_err_t  MCP23017_ioExpInit(void)
{
   app_err_t err = NRF_FAIL;

        //BANK = 	0 : sequential register addresses
	//MIRROR = 	0 : use configureInterrupt 
	//SEQOP = 	1 : sequential operation disabled, address pointer does not increment
	//DISSLW = 	0 : slew rate enabled
	//HAEN = 	0 : hardware address pin is always enabled on 23017
	//ODR = 	0 : active driver output (INTPOL bit sets the polarity.)
	//INTPOL = 	0 : interrupt active low
	//UNIMPLMENTED 	0 : unimplemented: Read as ‘0’
    err = MCP23017_ioExpWriteData(0x0B, 0b00100000,ONE_BYTE_LENGTH);

    err = MCP23017_ioExpWriteData(IOCON, 0b00100000,ONE_BYTE_LENGTH);
     if (NRF_OK == err)
    {
        printk("Failed to Init GPIO, return Status: %d", err);
    }

      MCP23017_pinMode(GPA0_MCU_BRD_ID1,INPUT,0);
      MCP23017_pinMode(GPA1_MCU_BRD_ID2,INPUT,0);
      MCP23017_pinMode(GPA2_MCU_BRD_ID3,INPUT,0);
      MCP23017_pinMode(GPA3_MCU_BRD_ID4,INPUT,0);
      MCP23017_pinMode(GPA4_OVP1,INPUT,0);
      MCP23017_pinMode(GPA5_RLY1,OUTPUT,0);
      MCP23017_pinMode(GPA6_PB1,INPUT, 0);
      MCP23017_pinMode(GPA7_PB2,INPUT,0);

      MCP23017_pinMode(GPB0_RED,OUTPUT,0);
      MCP23017_pinMode(GPB1_GREEN,OUTPUT,0);
      MCP23017_pinMode(GPB2_BLUE,OUTPUT,0);
      MCP23017_pinMode(GPB3_RFID_RST,OUTPUT,0);
      MCP23017_pinMode(GPB4_GND_FLT,INPUT,0);
      MCP23017_pinMode(GPB5_MET_IRQ,INPUT,0);
      MCP23017_pinMode(GPB6_MET_RESET,OUTPUT,0);
      MCP23017_pinMode(GPB7_EL,INPUT,0);
    return err;
}

void MCP23017_pinMode(uint8_t pin, uint8_t mode, bool inverted)
{
	uint8_t iodirreg = 0U;
	uint8_t pullupreg = 0U;
	uint8_t polreg = 0U;
	uint8_t iodir= 0U;
        uint8_t pol = 0U;
        uint8_t pull = 0U;

	if(pin > 7)
	{
            iodirreg = IODIR_B;
            pullupreg = GPPU_B;
            polreg = IPOL_B;
            pin -= 8;
	}
        else
        {
            iodirreg = IODIR_A;
            pullupreg = GPPU_A;
            polreg = IPOL_A;
        }

        MCP23017_ioExpReadData(iodirreg,&iodir,ONE_BYTE_LENGTH);
	if(mode == INPUT || mode == INPUT_PULLUP) 
        {
            BIT_SET(iodir, pin);
        }
	else 
        {
             BIT_CLEAR(iodir, pin);
        }

        MCP23017_ioExpReadData(pullupreg,&pull,ONE_BYTE_LENGTH);
	if(mode == INPUT_PULLUP) 
        {
          BIT_SET(pull, pin);
        }
	else 
        {
         BIT_CLEAR(pull, pin);
        }

        MCP23017_ioExpReadData(polreg,&pol,ONE_BYTE_LENGTH);
	if(inverted) 
        {
          BIT_SET(pol, pin);
        }
	else
        {
          BIT_CLEAR(pol, pin);
        } 
	MCP23017_ioExpWriteData(iodirreg, iodir,ONE_BYTE_LENGTH);
	MCP23017_ioExpWriteData(pullupreg, pull,ONE_BYTE_LENGTH);
	MCP23017_ioExpWriteData(polreg, pol,ONE_BYTE_LENGTH);
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/