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
/** \file i2c_service.c
 * \brief
 *
 **/
/************************************************************************
 * INCLUDES
 ************************************************************************/
#include "i2c_service.h"
#include <zephyr/drivers/i2c.h>
#include "string.h"
#include "zephyr/logging/log.h"
#include "components/general/general.h"
/****************************************************************************
 * MACROS
 ***************************************************************************/
#if !DT_NODE_EXISTS(DT_NODELABEL(i2c0))
#error "Overlay for i2c0 node not properly defined."
#endif

#if !DT_NODE_EXISTS(DT_NODELABEL(i2c1))
#error "Overlay for i2c1 node not properly defined."
#endif

#define I2C1_NODE DT_NODELABEL(i2c0)
#define I2C2_NODE DT_NODELABEL(i2c1)

LOG_MODULE_REGISTER(i2cservice, LOG_LEVEL_NONE); //LOG_LEVEL_DBG
#define TIMIOUT_DELAY K_MSEC(2000U)
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/****************************************************************************
 * GLOBAL VARIABLES
 ****************************************************************************/
static const struct device *i2c1_dev = DEVICE_DT_GET(I2C1_NODE);
static const struct device *i2c2_dev = DEVICE_DT_GET(I2C2_NODE);
struct k_mutex i2cMutexlock; /*!< Mutex lock for access global data */
/****************************************************************************
 * FUNCTIONS PROTOTYPES
 ****************************************************************************/
void I2C_vScanner(uint8_t chanelNo)
{
	int amtFound = 0;
	printk("i2c1 Scanning in progress \n");
	if (I2C1 == chanelNo)
	{
		for (uint8_t i = 4; i <= 0x77; i++)
		{
			struct i2c_msg msgs[1] = {0};
			uint8_t dst = 0;

			/* Send the address to read from */
			msgs[0].buf = &dst;
			msgs[0].len = 0U;
			msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

			if (i2c_transfer(i2c1_dev, &msgs[0], 1, i) == 0)
			{
				// SEGGER_RTT_WriteString(0, "0x%2x FOUND\n", i);
				printk("0x%2x i2c1_scanner FOUND\n", i);
				amtFound = amtFound + 1;
			}
		}
	}
	else if (I2C2 == chanelNo)
	{
		for (uint8_t i = 4; i <= 0x77; i++)
		{
			struct i2c_msg msgs[1]={0};
			uint8_t dst=0;

			/* Send the address to read from */
			msgs[0].buf = &dst;
			msgs[0].len = 0U;
			msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

			if (i2c_transfer(i2c2_dev, &msgs[0], 1, i) == 0)
			{
				// SEGGER_RTT_WriteString(0, "0x%2x FOUND\n", i);
				printk("0x%2x i2c2_scanner FOUND\n", i);
				amtFound = amtFound + 1;
			}
		}
	}
	printk("found on common i2c1: %i\n", amtFound);
}
void I2C_init(uint8_t chanelNo)
{
	static bool bstatus = 0;
	if (0 == bstatus)
	{
		int16_t i2cMutexlockStatus = 0U;
		/** Mutex initialization */
		i2cMutexlockStatus = k_mutex_init(&i2cMutexlock);
		bstatus = 1;
	}

	if (I2C1 == chanelNo)
	{
		/*Common I2C-1
		 *	used for RTC/EEPROM/AuthChip/IO expander
		 */
		if (!device_is_ready(i2c1_dev))
		{
			printk("i2c1 dev not found");
			return; // TODO :need to decide error
		}
	}
	else if (I2C2 == chanelNo)
	{
		/*For OLED I2C-2 */
		if (!device_is_ready(i2c2_dev))
		{
			printk("i2c2 dev not found");
			return; // TODO :need to decide error
		}
	}
}
uint8_t I2C_u8writeByte(uint8_t chanelNo, uint8_t slave_addr, uint8_t regaddr,  uint8_t reg_data, uint16_t len)
{
	int err = NRF_OK;
	if (k_mutex_lock(&i2cMutexlock, TIMIOUT_DELAY) == NRF_OK)
	{
		if (I2C1 == chanelNo)
		{
			// i2c_reg_write_byte(const struct device *dev, uint16_t dev_addr, uint8_t reg_addr, uint8_t value)
			if (i2c_reg_write_byte(i2c1_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to write i2c_burst_write\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
		if (I2C2 == chanelNo)
		{
			if (i2c_reg_write_byte(i2c2_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to write i2c_burst_write\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
	}
	k_mutex_unlock(&i2cMutexlock);
	return err;
	// uint8_t temp_buf[1000];
	// temp_buf[0] =
	// if (i2c_write(i2c_dev, datas, 2, slave_addr)) {
	// 	TC_PRINT("Fail to configure sensor GY271\n");
	// 	return TC_FAIL;
	// }
	// return err;
}
uint8_t I2C_u8readByte(uint8_t chanelNo, uint8_t slave_addr, uint8_t regaddr, uint8_t *reg_data, uint16_t len)
{
	int err = -1;
	if (k_mutex_lock(&i2cMutexlock, TIMIOUT_DELAY) == NRF_OK)
	{
		if (I2C1 == chanelNo)
		{
			// i2c_reg_read_byte(const struct device *dev, uint16_t dev_addr, uint8_t reg_addr, uint8_t *value)
			if (i2c_reg_read_byte(i2c1_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to read i2c_burst_read\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
		if (I2C2 == chanelNo)
		{
			if (i2c_reg_read_byte(i2c2_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to read i2c_burst_read\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
	}
	k_mutex_unlock(&i2cMutexlock);
	return err;
	// return i2c_write_read(i2c_dev, slave_addr, &regaddr, 1, reg_data, len);
}

uint8_t I2C_u8writeByte_16(uint8_t chanelNo, uint8_t slave_addr, uint16_t regaddr,  uint8_t reg_data, uint16_t len)
{
	int err = -1;
	if (k_mutex_lock(&i2cMutexlock, TIMIOUT_DELAY) == NRF_OK)
	{
		if (I2C1 == chanelNo)
		{
			if (i2c_reg_write_byte(i2c1_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to write i2c_burst_write\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
		if (I2C2 == chanelNo)
		{
			if (i2c_reg_write_byte(i2c2_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to write i2c_burst_write\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
	}
	k_mutex_unlock(&i2cMutexlock);
	return err;
}
uint8_t I2C_u8readByte_16(uint8_t chanelNo, uint8_t slave_addr, uint16_t regaddr, uint8_t *reg_data, uint16_t len)
{
	int err = -1;
	if (k_mutex_lock(&i2cMutexlock, TIMIOUT_DELAY) == NRF_OK)
	{
		if (I2C1 == chanelNo)
		{
			if (i2c_reg_read_byte(i2c1_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to read i2c_burst_read\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
		if (I2C2 == chanelNo)
		{
			if (i2c_reg_read_byte(i2c2_dev, slave_addr, regaddr, reg_data))
			{
				printk("Fail to read i2c_burst_read\n");
				k_mutex_unlock(&i2cMutexlock);
				return err;
			}
		}
	}
	printk("reg_data %d",reg_data);
	k_mutex_unlock(&i2cMutexlock);
	return err;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#if 0
#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <stdio.h>

#define I2C_DEV_NAME    DT_LABEL(DT_NODELABEL(i2c0))
#define EEPROM_ADDR     0x50    // 7-bit address of the M24M02 EEPROM

void custom_i2c_write(struct device *i2c_dev, uint8_t *data, size_t len, uint16_t memory_address)
{
    uint8_t write_buffer[3];
    struct i2c_msg msg;

    // Prepare the write buffer with the memory address and data
    write_buffer[0] = (memory_address >> 8) & 0xFF; // High byte of memory address
    write_buffer[1] = memory_address & 0xFF;        // Low byte of memory address
    memcpy(&write_buffer[2], data, len);            // Data to be written

    msg.buf = write_buffer;
    msg.len = len + 2; // Memory address + data length
    msg.flags = I2C_MSG_WRITE;
    msg.addr = EEPROM_ADDR;

    if (i2c_write(i2c_dev, &msg, 1, EEPROM_ADDR) < 0) {
        printk("Failed to write data to EEPROM\n");
    }
}

void main(void)
{
    struct device *i2c_dev;
    uint8_t data_to_write[] = "Hello, M24M02!";
    uint8_t read_data[64]; // Adjust this size as needed
    uint16_t memory_address = 0; // EEPROM memory address to start writing

    i2c_dev = device_get_binding(I2C_DEV_NAME);
    if (!i2c_dev) {
        printk("I2C device not found\n");
        return;
    }

    // Write data to the EEPROM
    custom_i2c_write(i2c_dev, data_to_write, sizeof(data_to_write), memory_address);
    printk("Data written to EEPROM: %s\n", data_to_write);

    // Read data from the EEPROM
    struct i2c_msg read_msg;
    read_msg.buf = read_data;
    read_msg.len = sizeof(read_data);
    read_msg.flags = I2C_MSG_READ | I2C_MSG_STOP;
    read_msg.addr = EEPROM_ADDR;

    if (i2c_transfer(i2c_dev, &read_msg, 1, EEPROM_ADDR) < 0) {
        printk("Failed to read data from EEPROM\n");
        return;
    }

    printk("Data read from EEPROM: %s\n", read_data);
}

#endif