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
 * \file     ade7593.c
 * \brief
 */
/************************************************************************
 * Include Header Files
 ************************************************************************/
#include <zephyr/kernel.h>
#include "ade7593.h"
#include "zephyr/logging/log.h"
#include "components/gpio_config/gpio_config.h"
#include <zephyr/drivers/spi.h>
/************************************************************************
 * Define Macros
 ************************************************************************/
#define BUFFER_SIZE 7U
#define READ_BUFFER_SIZE 4U
#define BUFF_SIZE 3U
#define SPI_TIMEOUT 100U
#define WRITE_OPERATION 0x00
#define READ_OPERATION 0x80

#define BYTE_1 8
#define BYTE_2 16
#define BYTE_3 24
#define BYTE_4 32

#define TIMEOUT 100U

#define TAG "internal_meter"

#define METER_TASK_NAME "meter task"
#define METER_TASK_STACK_SIZE (4 * 1024) ///< Define Task Stack Size
#define METER_TASK_PRIORITY 9
#define TASK_DELAY_MS 100U
#define METERING_DELAY 50

LOG_MODULE_REGISTER(meter, LOG_LEVEL_DBG);
#if !DT_NODE_EXISTS(DT_NODELABEL(meter_spi))
#error "Overlay for power output node not properly defined."
#endif
#define MY_SPI_MASTER DT_NODELABEL(meter_spi)
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
/* Definitions for internal_meter */
spimeterData_st R_Phase, B_Phase, Y_Phase;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/
void vmeterTask(void *argument);

static struct k_thread internal_meterHandle;
static K_KERNEL_STACK_DEFINE(meter_task_stack, METER_TASK_STACK_SIZE);

// extern const k_tid_t internal_meterHandle; /**< Reference to internal meter FreeRTOS task. */
// K_THREAD_DEFINE(internal_meterHandle, METER_TASK_STACK_SIZE, vmeterTask, NULL, NULL, NULL, METER_TASK_PRIORITY, 0, 0);

// SPI master functionality
const struct device *spi_dev;
static struct k_poll_signal spi_done_sig = K_POLL_SIGNAL_INITIALIZER(spi_done_sig);
/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
struct spi_cs_control spim_cs = {
	//.gpio = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(reg_my_spi_master)),
	.delay = 0,
};
void spi_init(void)
{
	spi_dev = DEVICE_DT_GET(MY_SPI_MASTER);
	if (!device_is_ready(spi_dev))
	{
		printk("SPI master device not ready!\n");
	}
	// if(!device_is_ready(spim_cs.gpio.port)){
	// 	printk("SPI master chip select device not ready!\n");
	// }
}

static const struct spi_config spi_cfg = {
	.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_TRANSFER_MSB |
				 SPI_MODE_CPOL | SPI_MODE_CPHA,
	.frequency = 400000,
	.slave = 0,
	//.cs = &spim_cs,
};

void ADE7953_initialize(void)
{
	static uint8_t ui8Buffer[BUFFER_SIZE] = {0x0};
	app_err_t retVal = NRF_FAIL;
	uint8_t u8val = 0U;
	// Write 0x00AD to Register Address 0x00FE. "This unlocks Register 0x120." per datasheet
	SPI_MeterWrite(0x00FE, 0x00AD, BYTE_4, 1);
	// Write 0x0030 to Register Address 0x0120. "This configures the optimum settings." per datasheet
	SPI_MeterWrite(0x0120, 0x0030, BYTE_4, 1);
	getVersion(&u8val, 1);
	CalibrationValueInit(1);
}
/*
 * @brief Read the HW version of metering chip
 * @param *pui8Val : buffer address to read HW version
 * @retval Error code
 */
app_err_t getVersion(uint8_t *pui8Val, uint8_t connector)
{
	uint8_t ui8Buffer[BUFF_SIZE] = {0x0};
	app_err_t retVal = NRF_OK;

	/* HW Version register address 0x702 */
	ui8Buffer[0] = (VERSION_8 >> 8) & 0xFF; /* MSB */
	ui8Buffer[1] = VERSION_8 & 0xFF;		/* LSB */
	ui8Buffer[2] = READ_OPERATION;

	static uint8_t rx_buffer[2];

	const struct spi_buf tx_buf = {
		.buf = ui8Buffer,
		.len = sizeof(ui8Buffer)};

	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1};

	struct spi_buf rx_buf = {
		.buf = rx_buffer,
		.len = 1,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1};

	GPIO_OnOff(OP_SPI1_CS1, LOW);
	k_msleep(METERING_DELAY);
	if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
	{
		printk("getVersion spi write failed  \n");
		retVal = NRF_FAIL;
	}
	if (NRF_OK != spi_read(spi_dev, &spi_cfg, &rx))
	{
		printk("getVersion spi write failed  \n");
		retVal = NRF_FAIL;
	}
	printk("getVersiongetVersiongetVersion rx_buffer[0]:%d rx_buffer[1] :%d \n", rx_buffer[0], rx_buffer[1]);
	// pui8Val = rx_buffer[0];
	k_msleep(METERING_DELAY);
	GPIO_OnOff(OP_SPI1_CS1, HIGH);
	return retVal;
}

app_err_t spiAlgorithm8_read(uint16_t ui16Address, uint32_t *ui32val,
							 uint8_t connector)
{
	// This is the algorithm that reads from a 32 bit register in the ADE7953.
	//  The arguments are the MSB and LSB of the address of the register respectively.
	// The values of the arguments are obtained from the list of functions above.
	// Caution, some register elements contain information that is only 24 bit with padding on the MSB

	app_err_t err = NRF_OK;
	uint32_t ui32ReadBuffer = 0x00U;
	uint8_t ui8Buffer[READ_BUFFER_SIZE] = {0X00U};

	ui8Buffer[0] = (ui16Address >> 8) & 0XFF;
	ui8Buffer[1] = ui16Address & 0XFF;
	ui8Buffer[2] = READ_OPERATION;

	static uint8_t rx_buffer[5] = {0};

	const struct spi_buf tx_buf = {
		.buf = ui8Buffer,
		.len = sizeof(ui8Buffer)};

	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1};

	struct spi_buf rx_buf = {
		.buf = rx_buffer,
		.len = READ_BUFFER_SIZE,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1};
	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, LOW);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);
	if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
	{
		printk("getVersion spi write failed  \n");
		err = NRF_FAIL;
	}
	if (NRF_OK != spi_read(spi_dev, &spi_cfg, &rx))
	{
		printk("getVersion spi write failed \n");
		err = NRF_FAIL;
	}
	printk("spiAlgorithm8_read rx_buffer[0]:%d rx_buffer[1] :%d rx_buffer[3]:%d rx_buffer[4] :%d \n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3]);
	ui32ReadBuffer = rx_buffer[0] | (rx_buffer[1] << 8) | (rx_buffer[2] << 16) | (rx_buffer[3] << 24);

	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, HIGH);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}

	*ui32val = ((ui32ReadBuffer)&0XFF);
	return err;
}

app_err_t spiAlgorithm32_read(uint16_t ui16Address, uint32_t *ui32val,
							  uint8_t connector)
{
	// This is the algorithm that reads from a 32 bit register in the ADE7953.
	//  The arguments are the MSB and LSB of the address of the register respectively.
	// The values of the arguments are obtained from the list of functions above.
	// Caution, some register elements contain information that is only 24 bit with padding on the MSB

	app_err_t err = NRF_OK;
	uint32_t ui32ReadBuffer = 0x00U;
	uint8_t ui8Buffer[READ_BUFFER_SIZE] = {0X00U};

	ui8Buffer[0] = (ui16Address >> 8) & 0XFF;
	ui8Buffer[1] = ui16Address & 0XFF;
	ui8Buffer[2] = READ_OPERATION;
	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, LOW);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);
	static uint8_t rx_buffer[READ_BUFFER_SIZE];

	const struct spi_buf tx_buf = {
		.buf = ui8Buffer,
		.len = (READ_BUFFER_SIZE - 1)};

	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1};

	struct spi_buf rx_buf = {
		.buf = rx_buffer,
		.len = READ_BUFFER_SIZE,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1};

	if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
	{
		printk("getVersion spi write failed  \n");
		err = NRF_FAIL;
	}
	if (NRF_OK != spi_read(spi_dev, &spi_cfg, &rx))
	{
		printk("getVersion spi write failed \n");
		err = NRF_FAIL;
	}
	// printk("spiAlgorithm8_read rx_buffer[0]:%d rx_buffer[1] :%d rx_buffer[3]:%d rx_buffer[4] :%d \n", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3]);
	ui32ReadBuffer = rx_buffer[0] | (rx_buffer[1] << 8) | (rx_buffer[2] << 16) | (rx_buffer[3] << 24);

	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, HIGH);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);

	*ui32val = (((uint32_t)rx_buffer[0] << 24) + ((uint32_t)rx_buffer[1] << 16) + ((uint32_t)rx_buffer[2] << 8) + (uint32_t)rx_buffer[3]);
	return err;
}

app_err_t spiAlgorithm32_write(uint16_t ui16Address, uint32_t ui32val,
							   uint8_t connector)
{ // This is the algorithm that writes to a register in the ADE7953.
	// The arguments are the MSB and LSB of the address of the register respectively.
	// The values of the arguments are obtained from the list of functions above.
	app_err_t err = NRF_OK;
	uint8_t ui8Buffer[BUFFER_SIZE] = {0x00};

	ui8Buffer[0] = (ui16Address >> 8U) & 0XFF; /*MSB*/
	ui8Buffer[1] = ui16Address & 0XFF;		   /*LSB*/
	ui8Buffer[2] = WRITE_OPERATION;

	ui8Buffer[3] = (ui32val >> 24U) & 0XFF; /*MSB*/
	ui8Buffer[4] = (ui32val >> 16U) & 0XFF;
	ui8Buffer[5] = (ui32val >> 8U) & 0XFF;
	ui8Buffer[6] = ui32val & 0XFF; /*LSB*/

	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, LOW);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);
	const struct spi_buf tx_buf = {
		.buf = ui8Buffer,
		.len = BUFFER_SIZE};

	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1};

	if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
	{
		printk("getVersion spi write failed  \n");
		err = NRF_FAIL;
	}

	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, HIGH);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);
	return err;
}

app_err_t spiAlgorithm8_write(uint16_t ui16Address, uint8_t ui32val,
							  uint8_t connector)
{
	// This is the algorithm that writes to a register in the ADE7953.
	// The arguments are the MSB and LSB of the address of the register respectively.
	// The values of the arguments are obtained from the list of functions above.
	app_err_t err = NRF_OK;
	uint8_t ui8Buffer[BUFFER_SIZE] = {0x00};

	ui8Buffer[0] = (ui16Address >> 8U) & 0XFF; /*MSB*/
	ui8Buffer[1] = ui16Address & 0XFF;		   /*LSB*/
	ui8Buffer[2] = WRITE_OPERATION;

	ui8Buffer[3] = ui32val & 0XFF;

	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, LOW);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}

	k_msleep(METERING_DELAY);
	// static uint8_t tx_buffer[2];
	const struct spi_buf tx_buf = {
		.buf = ui8Buffer,
		.len = BUFFER_SIZE};

	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1};

	if (NRF_OK != spi_write(spi_dev, &spi_cfg, &tx))
	{
		printk("getVersion spi write failed  \n");
		err = NRF_FAIL;
	}
	if (1U == connector)
	{
		GPIO_OnOff(OP_SPI1_CS1, HIGH);
	}
	else if (2U == connector)
	{
	}
	else if (3U == connector)
	{
	}
	k_msleep(METERING_DELAY);
	return err;
}
float decimalize(long input, float factor, float offset) // This function adds a decimal point to the input value and returns it as a float, it also provides linear calibration (y=mx+b) by providing input in the following way as arguments (rawinput, gain, offset)
{
	return ((float)input / factor) + offset;
}

void meter_TaskInit(void)
{
	/* creation of internal_meter */

	k_thread_create(&internal_meterHandle, meter_task_stack,
					K_KERNEL_STACK_SIZEOF(meter_task_stack),
					(k_thread_entry_t)vmeterTask, NULL, NULL, NULL,
					K_PRIO_COOP(METER_TASK_PRIORITY),
					0, K_NO_WAIT);
	k_thread_name_set(&internal_meterHandle, "Meter task");
}
/* USER CODE BEGIN Header_meter_Task */
/**
 * @brief Function implementing the internal_meter thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_meter_Task */
void vmeterTask(void *argument)
{
	/* USER CODE BEGIN meter_Task */
	while (true)
	{
		// printk("!!!!!!!!!!!!!! veterTask !!!!!!!!!!!!!!\n");
		vReadMeasurementRegs(1);
		k_msleep(TASK_DELAY_MS);
	}
	/* USER CODE END meter_Task */
}

app_err_t SPI_MeterWrite(uint16_t u16Address, uint32_t ui32val,
						 uint8_t ByteValue, uint8_t connector)
{
	app_err_t err = NRF_FAIL;
	if (8U == ByteValue)
	{
		err = spiAlgorithm8_write(u16Address, ui32val, connector);
	}
	else if (32U == ByteValue)
	{
		err = spiAlgorithm32_write(u16Address, ui32val, connector);
	}
	return err;
}

app_err_t SPI_MeterRead(uint16_t u16Address, uint32_t *ui32val,
						uint8_t ByteValue, uint8_t connector)
{
	app_err_t err = NRF_FAIL;
	if (8U == ByteValue)
	{
		err = spiAlgorithm8_read(u16Address, ui32val, connector);
	}
	else if (32U == ByteValue)
	{
		err = spiAlgorithm32_read(u16Address, ui32val, connector);
	}
	return err;
}

void CalibrationValueInit(uint8_t connector)
{
	uint32_t ui32Val = 0;

	/********
	 *
	 * calibration values before write
	 */
	printk("********************Before Write****************************");
	SPI_MeterRead(VRMSOS_32, &ui32Val, BYTE_4, connector);
	printk("VRMSOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AIRMSOS_32, &ui32Val, BYTE_4, connector);
	printk("AIRMSOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AWATTOS_32, &ui32Val, BYTE_4, connector);
	printk("AWATTOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVAROS_32, &ui32Val, BYTE_4, connector);
	printk("AVAROS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AVGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AIGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AIGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AWGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AWGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVARGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AVARGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	printk("******************** Write Start ****************************");
	SPI_MeterWrite(VRMSOS_32, VRMSOS_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AIRMSOS_32, AIRMSOS_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AWATTOS_32, AWATTOS_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AVAROS_32, AVAROS_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AVGAIN_32, AVGAIN_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AIGAIN_32, AIGAIN_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AWGAIN_32, AWGAIN_DEFAULT_VALUE, BYTE_4, connector);
	SPI_MeterWrite(AVARGAIN_32, AVARGAIN_DEFAULT_VALUE, BYTE_4, connector);
	printk("******************** After Write****************************");
	SPI_MeterRead(VRMSOS_32, &ui32Val, BYTE_4, connector);
	printk("VRMSOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AIRMSOS_32, &ui32Val, BYTE_4, connector);
	printk("AIRMSOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AWATTOS_32, &ui32Val, BYTE_4, connector);
	printk("AWATTOS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVAROS_32, &ui32Val, BYTE_4, connector);
	printk("AVAROS_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AVGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AIGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AIGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AWGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AWGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;

	SPI_MeterRead(AVARGAIN_32, &ui32Val, BYTE_4, connector);
	printk("AVARGAIN_32 %08X  \n", ui32Val);
	ui32Val = 0;
}
void vReadMeasurementRegs(uint8_t connector)
{
	getIrmsA(connector);
	getVrms(connector);
	// getInstReactivePowerA(connector);
	 //getActiveEnergyA(connector);
	//UpdateSPIMeteringInfo();
}

app_err_t getActiveEnergyA(uint8_t connector)
{
	app_err_t retVal = NRF_FAIL;
	uint32_t ui32Val = 0;
	union
	{
		float decimal;
		uint32_t usl;
	} meter_data;

	retVal = SPI_MeterRead(AWATT_32, &ui32Val, BYTE_4, connector);
	// meter_data.decimal = decimalize(ui32Val, 19090, 0); // convert to float with calibration factors specified
	//printk("ui32Val %u getActiveEnergyA   %f\n", ui32Val, meter_data.decimal * 100);
printk("getActiveEnergyA %d\n",ui32Val);
	if (NRF_OK != retVal)
	{
		if (1U == connector)
		{
			R_Phase.u32Energy = meter_data.usl;
		}
	}
	R_Phase.u32Energy = ui32Val;
	return retVal;
}
app_err_t getInstReactivePowerA(uint8_t connector)
{
	app_err_t retVal = NRF_FAIL;
	uint32_t ui32Val = 0;
	union
	{
		float decimal;
		uint32_t usl;
	} meter_data;

	retVal = SPI_MeterRead(AVAR_32, &ui32Val, BYTE_4, connector);
	// meter_data.decimal = decimalize(ui32Val, 19090, 0); // convert to float with calibration factors specified
	 printk("getInstReactivePowerA %f\n", meter_data.decimal * 100);
	if (NRF_OK != retVal)
	{
		if (1U == connector)
		{
			R_Phase.u32Power = meter_data.usl;
		}
	}
	R_Phase.u32Power = ui32Val;
	return retVal;
}
app_err_t getVrms(uint8_t connector)
{
	app_err_t retVal = NRF_FAIL;
	uint32_t ui32Val = 0;
	union
	{
		float decimal;
		uint32_t usl;
	} meter_data;

	retVal = SPI_MeterRead(VRMS_32, &ui32Val, BYTE_4, connector);
	// meter_data.decimal = decimalize(ui32Val, 19090, 0); // convert to float with calibration factors specified
	printk("ADE7953_getVrms %d\n", ui32Val);
	if (NRF_OK != retVal)
	{
		if (1U == connector)
		{
			R_Phase.u32Voltage = ui32Val;
		}
	}
	R_Phase.u32Voltage = ui32Val / 100;
	// printk("getVrmsgetVrmsgetVrmsgetVrms %d \n", R_Phase.u32Voltage);
	return retVal;
}

app_err_t getIrmsA(uint8_t connector)
{
	app_err_t retVal = NRF_FAIL;
	uint32_t ui32Val = 0;

	union
	{
		float decimal;
		uint32_t usl;
	} meter_data;

	retVal = SPI_MeterRead(IRMSA_32, &ui32Val, BYTE_4, connector);
	//  meter_data.decimal = decimalize(ui32Val, 1327, 0); // convert to float with calibration factors specified
	printk("ADE7953_getIrmsA %d\n", ui32Val);
	if (NRF_OK != retVal)
	{
		if (1U == connector)
		{
			R_Phase.u32Current = ui32Val;
		}
	}
	R_Phase.u32Current = ui32Val / 10;
	return retVal;
}
 
void VRMSGainCalibration(uint8_t connector)
{
	uint32_t ui32RawVrms = 0;
	uint32_t ui32TargetVoltage = 230000;
	uint32_t ui32VoltageGain = 0;
	int32_t ui32Total = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	uint8_t ui8Flag = 0;
	char *cpBuffer = NULL;
	float gfVconst = 3.80716301e-005;

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		(void)SPI_MeterWrite(AVGAIN_32, UNITY_GAIN, BYTE_4, connector);
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read VRMS value from meters VRMS register */
				if (NRF_OK == SPI_MeterRead(VRMS_32, (uint32_t *)&ui32RawVrms,
											BYTE_4, connector))
				{
					/* Decrement counter */
					ui8Temp--;
					/* Add VRMS count */
					ui32Total += ui32RawVrms;
					printk("ui32Totalui32Total %d ui8Temp %d ", ui32Total, ui8Temp);
				}
				else
				{
					ui8Flag = 1;
					break;
				}
			}
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(VRMS_32, (uint32_t *)&ui32RawVrms, BYTE_4,
										connector))
			{
			}
		}
		if (ui8Flag == 0)
		{
			/* Calculate average */
			ui32RawVrms = ui32Total / ui8CalCyc;
			/* Convert target voltage value in mili Amperes */
			ui32TargetVoltage = (ui32TargetVoltage);
			/* Calculate calibration factor in float */
			gfVconst = (float)ui32TargetVoltage / (float)ui32RawVrms;
			/* Folat to fixed conversion */
			ui32VoltageGain = float_to_fixed(
				FIXED_POINT_BIT_RESOLUTION_METER_CHANNEL, gfVconst);
			/* Write calculated current gain to AVGAIN register */
			printk("ui32VoltageGainui32VoltageGain in hex %x  ", ui32VoltageGain); // 4f1cd10			SPI_MeterWrite(AVGAIN_32, ui32VoltageGain, BYTE_4, connector);
			// gstCalParam.ui32VRMSGain = ui32VoltageGain;
			// FRAM_write(FRAM METER_VOLT_GAIN_ADDR_INDEX, (uint8_t*) &ui32VoltageGain, sizeof(ui32VoltageGain));
		}
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}
void VRMSOffsetCalibration(uint8_t connector)
{
	int32_t i32VrmsOffset = 0;
	int32_t ui32Total = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	char *cpBuffer = NULL;

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		SPI_MeterWrite(AVGAIN_32, UNITY_GAIN, BYTE_4, connector);
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read VRMS value from meters VRMS register */
				if (NRF_OK == SPI_MeterRead(VRMS_32, (uint32_t *)&i32VrmsOffset,
											BYTE_4, connector))
				{
					/* Decrement counter */
					ui8Temp--;
					/* Add VRMS count */
					ui32Total += i32VrmsOffset;
					//printk("ui32Totalui32Totalui32Total %d and Temp %d \n", ui32Total, ui8Temp);
				}
				else
				{
					break;
				}
			}
			i32VrmsOffset = ui32Total / ui8CalCyc;
			printk("i32VrmsOffseti32VrmsOffset %d ", i32VrmsOffset); // 2d4e
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(VRMS_32, (uint32_t *)&i32VrmsOffset,
										BYTE_4, connector))
			{
			}
		}
		if (0 != i32VrmsOffset)
		{
			printk("i32VrmsOffseti32VrmsOffset in hex %x ", i32VrmsOffset);
			/* Write calculated current gain to AVGAIN register */
			SPI_MeterWrite(VRMSOS_32, -i32VrmsOffset, BYTE_4, connector);
			// gstCalParam.ui32VRMSOffset = i32VrmsOffset;
			// FRAM_write(FRAM METER_VOLT_OFFSET_ADDR_INDEX, (uint8_t*) &i32VrmsOffset, sizeof(i32VrmsOffset));
		}
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}


void ActivePowerGainCalibration(uint8_t connector)
{
	uint32_t ui32TargetPower = 2300000;
	uint32_t ui32RawIrms = 0;
	uint32_t ui32RawVrms = 0;
	float fIrms = 0.0;
	float fVrms = 0.0;
	uint32_t ui32PowerGain = 0;
	uint32_t ui32RawPower = 0;
	uint32_t ui32AWATTMeterVal = 0;
	uint32_t ui32Total = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	float gfIconst =0.0;
	char *cpBuffer = NULL;

	SPI_MeterWrite(AWGAIN_32, UNITY_GAIN, BYTE_4, connector);
	k_msleep(50);
	SPI_MeterRead(AWATT_32, (uint32_t *)&ui32AWATTMeterVal, BYTE_4, connector);
	k_msleep(50);
	SPI_MeterRead(VRMS_32, (uint32_t *)&ui32RawVrms, BYTE_4, connector);
k_msleep(10);
	SPI_MeterRead(IRMSA_32, (uint32_t *)&ui32RawIrms, BYTE_4, connector);
k_msleep(50);
fVrms = (float)ui32RawVrms/(float)(1000.0f);
fIrms = (float)ui32RawIrms/(float)(10000.0f);
ui32TargetPower = (fVrms * fIrms);

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read VRMS value from meters VRMS register */
				if (NRF_OK == SPI_MeterRead(AWATT_32, (uint32_t *)&ui32AWATTMeterVal,
											BYTE_4, connector))
				{
					ui32RawPower = CalculateTwosComplement(ui32AWATTMeterVal);
					/* Decrement counter */
					ui8Temp--;
					/* Add VRMS count */
					ui32Total += ui32RawPower;
					k_msleep(10);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(AWATT_32, (uint32_t *)&ui32RawPower, BYTE_4,
										connector))
			{
			}
		}
			/* Calculate average */
			ui32RawPower = (uint32_t)(ui32Total / ui8CalCyc);
			/* Calculate calibration factor in float */
			gfIconst = (float)ui32TargetPower / (float)ui32RawPower;
			/* Folat to fixed conversion */
			ui32PowerGain = float_to_fixed(
				FIXED_POINT_BIT_RESOLUTION_METER_CHANNEL, gfIconst);
			/* Write calculated current gain to AVGAIN register */
			printk("ui32CurrentGainui32CurrentGainui32CurrentGain in hex %x ", ui32PowerGain);
			SPI_MeterWrite(AWGAIN_32, ui32PowerGain, BYTE_4, connector);
			// gstCalParam.ui32VRMSGain = ui32PowerGain;
			// FRAM_write(FRAM METER_CURR_GAIN_ADDR_INDEX, (uint8_t*) &ui32PowerGain, sizeof(ui32PowerGain));
		
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}
uint32_t CalculateTwosComplement(uint32_t ui32RawMeterCount)
{
	ui32RawMeterCount = ui32RawMeterCount ^ 0XFFFFFFFF;
	ui32RawMeterCount +=0x01;
	return ui32RawMeterCount;
}

void ActivePowerOffsetCalibration(uint8_t connector)
{
	int32_t i32Offset = 0;
	int32_t ui32Total = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	char *cpBuffer = NULL;

//Performing Active poweroffset calibration
//Set AC voltage and current to 0
//if set yes or no

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read power value from meters register */
				if (NRF_OK == SPI_MeterRead(AWATT_32, (uint32_t *)&i32Offset,
											BYTE_4, connector))
				{
					/* Decrement counter */
					ui8Temp--;
					/* Add  count */
					ui32Total += i32Offset;
					printk("ui32Total %d and Temp %d \n", ui32Total, ui8Temp);
				}
				else
				{
					break;
				}
			}
			i32Offset = ui32Total / ui8CalCyc;
			printk("i32Offset %d ", i32Offset);
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(VRMS_32, (uint32_t *)&i32Offset,
										BYTE_4, connector))
			{
			}
		}
		if (0 != i32Offset)
		{
			printk("i32ActivePowerOffset in hex %x ", i32Offset);
			/* Write calculated current gain to AWATTOS_32 register */
			SPI_MeterWrite(AWATTOS_32, i32Offset, BYTE_4, connector);
			// gstCalParam.ui32VRMSOffset = i32VrmsOffset;
			// FRAM_write(FRAM METER_VOLT_OFFSET_ADDR_INDEX, (uint8_t*) &i32VrmsOffset, sizeof(i32VrmsOffset));
		}
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}
void IRMSGainCalibration(uint8_t connector)
{
	uint32_t ui32RawIrms = 0;
	uint32_t ui32TargetCurrent = 14400;
	uint32_t ui32TargetCurrentMilliAmp = 0;
	int32_t ui32Total = 0;
	int32_t ui32CurrentGain = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	uint8_t ui8Flag = 0;
	char *cpBuffer = NULL;
	float gfIconst = 0;

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		SPI_MeterWrite(AIGAIN_32, UNITY_GAIN, BYTE_4, connector);
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read VRMS value from meters VRMS register */
				if (NRF_OK == SPI_MeterRead(IRMSA_32, (uint32_t *)&ui32RawIrms,
											BYTE_4, connector))
				{
					/* Decrement counter */
					ui8Temp--;
					/* Add VRMS count */
					ui32Total += ui32RawIrms;
				}
				else
				{
					ui8Flag = 1;
					break;
				}
			}
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(IRMSA_32, (uint32_t *)&ui32RawIrms, BYTE_4,
										connector))
			{
			}
		}
		if (ui8Flag == 0)
		{
			/* Calculate average */
			ui32RawIrms = ui32Total / ui8CalCyc;
			/* Convert target voltage value in mili Amperes */
			ui32TargetCurrentMilliAmp = (ui32TargetCurrent);
			/* Calculate calibration factor in float */
			gfIconst = (float)ui32TargetCurrentMilliAmp / (float)ui32RawIrms;
			/* Folat to fixed conversion */
			ui32CurrentGain = float_to_fixed(
				FIXED_POINT_BIT_RESOLUTION_METER_CHANNEL, gfIconst);
			/* Write calculated current gain to AVGAIN register */
			printk("ui32CurrentGainui32CurrentGainui32CurrentGain in hex %x ", ui32CurrentGain);
			SPI_MeterWrite(AIGAIN_32, ui32CurrentGain, BYTE_4, connector);
			// gstCalParam.ui32VRMSGain = ui32CurrentGain;
			// FRAM_write(FRAM METER_CURR_GAIN_ADDR_INDEX, (uint8_t*) &ui32CurrentGain, sizeof(ui32CurrentGain));
		}
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}

void IRMSOffsetCalibration(uint8_t connector)
{
	int32_t i32IrmsOffset = 0;
	int32_t ui32Total = 0;
	uint8_t ui8Temp = 0;
	uint8_t ui8CalCyc = 0;
	char *cpBuffer = NULL;

	if (1)
	{
		ui8CalCyc = 5;
		ui8Temp = ui8CalCyc;
		SPI_MeterWrite(AIGAIN_32, UNITY_GAIN, BYTE_4, connector);
		/* Check for more than 1 calibration cycle */
		if (0 != ui8CalCyc)
		{
			while (ui8Temp)
			{
				/* Read VRMS value from meters VRMS register */
				if (NRF_OK == SPI_MeterRead(IRMSA_32, (uint32_t *)&i32IrmsOffset,BYTE_4, connector))
				{
					/* Decrement counter */
					ui8Temp--;
					/* Add VRMS count */
					ui32Total += i32IrmsOffset;
				}
				else
				{
					break;
				}
			}
			i32IrmsOffset = ui32Total / ui8CalCyc;
		}
		else
		{
			if (NRF_OK != SPI_MeterRead(IRMSA_32, (uint32_t *)&i32IrmsOffset,BYTE_4, connector))
			{
			}
		}
		if (0 != i32IrmsOffset)
		{

			/* Write calculated current gain to AVGAIN register */
			printk("i32IrmsOffseti32IrmsOffseti32IrmsOffseti32IrmsOffset in hex %x", i32IrmsOffset);
			SPI_MeterWrite(AIRMSOS_32, -i32IrmsOffset, BYTE_4, connector);
			// gstCalParam.i32IrmsOffset = i32IrmsOffset;
			// FRAM_write(FRAM METER_CURR_OFFSET_ADDR_INDEX, (uint8_t*) &i32IrmsOffset, sizeof(i32IrmsOffset));
		}
	}
	else if (false == strcmp("NO", cpBuffer))
	{
		// APP_LOGE("Exit from gain calibration...!!!");
	}
	else
	{
		// APP_LOGE("ERROR: Invalid input. ");
	}
}

/****************************************************************************
 * END OF FILE
 ****************************************************************************/
#if 0
float getPowerFactorA() {
//PFA_16
}

int16_t getPhaseCalibA() {
//PHCALA_16
}

float getPeriod() {
//Period_16
}

unsigned long getAPNOLOAD() {
//AP_NOLOAD_32
}

long getInstVoltage() {
//V_32
}
long getInstCurrentA() {
//IA_32
}
unsigned long getVpeak() {
//VPEAK_32
}
unsigned long getIpeakA() {
//IAPEAK_32
}
long getReactiveEnergyA() {
//RENERGYA_32
}
long getApparentEnergyA() {
//APENERGYA_32
}
float getInstApparentPowerA() {
	//AVA_32
}
float getInstActivePowerA() {
	//AWATT_32
}
#endif


