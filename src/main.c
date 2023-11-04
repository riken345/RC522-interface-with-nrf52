/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include "components/gpio_config/gpio_config.h"
#include "components/adc_services/adc_service.h"
#include "components/i2c_services/i2c_service.h"
#include "components/ioExp_mcp23s17/ioExp_mcp23s17.h"
#include "components/oled_ssd1306/ssd1306.h"
#include "components/app_schedule/app_schedule.h"
#include "components/meter_ade7593/ade7593.h"
#include "components/app_UI/app_ui.h"
#include "components/led_handler/led_handler.h"
#include "components/app_timer/app_timer.h"
#include "components/gpio_config/gpio_config.h"
#include "zephyr/drivers/pwm.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/volt_curr_monitor/volt_curr_monitor.h"
#include "components/eeprom_m24m02/eeprom_m24m02.h"
#include "components/temp_reading/temp.h"

#define SLEEP_TIME_MS 1
#define METERCALIBRATION 1U
void vBoardInit(void);
void vSystemInit(void);
void vApplicationInit(void);
int main(void)
{
    printk("Hello World! \n");
    vBoardInit();
    vSystemInit();
    vApplicationInit();
}
void vBoardInit(void)
{
    /*GPIO initialization*/
    GPIO_vinit();
    /*I2C1 initialization*/
    I2C_init(I2C1);
    /*I2C2 initialization*/
    I2C_init(I2C2);
    /*ADC initialization*/
    ADC_init();
    /*SPI initialization*/
    spi_init();
}
void vApplicationInit(void)
{
    /*initialize CP_pwm_task */
#if METERCALIBRATION
    cppwm_TaskInit();
    /*initialize Meter task*/
    meter_TaskInit();
    /*initialize LED handler */
    LEDHANDLER_TaskInit();
    /*initialize UI task handler */
    app_UITaskInit();
    /* initialize tick Timer*/
    appTickTimer_Init();
    /*initialize Schedule handler */
    ScheduleTask_Init();
#endif
    k_msleep(SLEEP_TIME_MS * 100);
    /*Calibration for line ADC voltage*/
    VOLTAGE_caladcLineGain(230);
    printk("##########################\r\n");

    // VOLTAGE_processADCData();
}
void vSystemInit(void)
{
    /* IOXP RESET*/
    MCP23017_RstIOExpander();
    /*Init IOXP */
    MCP23017_ioExpInit();
    k_msleep(SLEEP_TIME_MS * 2);
    /* OLED Initialization*/
    //
    // ssd1306_oled_print();

    /* Meter IC Initialization*/
    MCP23017_IOExpGpioLevelSet(GPB6_MET_RESET, HIGH);
    k_msleep(SLEEP_TIME_MS * 2);
    ADE7953_initialize();

    ssd1306_oled_init();
    oledstartupdisplayprint();
    k_msleep(2000);
    //set_relay_output(1);
    //k_msleep(2000);
    //IRMSGainCalibration(1);

    // set_relay_output(1);
    // k_msleep(5000);
    // POWERGainCalibration(1);
    // VRMSOffsetCalibration(1);printk("\n");
    // IRMSOffsetCalibration(1);printk("\n");
    // POWEROffsetCalibration(1);printk("\n");
    // IRPOWEROffsetCalibration(1);printk("\n");

    // MCP23017_IOExpGpioLevelSet(GPB1_GREEN, HIGH);
    // MCP23017_IOExpGpioLevelSet(GPB0_RED, HIGH);
    // MCP23017_IOExpGpioLevelSet(GPB2_BLUE, HIGH);

    // while (1)
    // {
    //     uint8_t pushbutton1;
    //     uint8_t pushbutton2;
    //     uint8_t earth_fault;
    //     uint8_t ground_fault;

    //     pushbutton1 = MCP23017_IOExpGpioLevelGet(GPA6_PB1);
    //     printk("GPIO PIN GPA6_PB1 %d\n", pushbutton1);

    //     pushbutton2 = MCP23017_IOExpGpioLevelGet(GPA7_PB2);
    //     printk("GPIO PIN GPA7_PB2 %d\n", pushbutton2);

    //     earth_fault = MCP23017_IOExpGpioLevelGet(GPB7_EL);
    //     printk("GPIO PIN GPB7_EL %d\n", earth_fault);

    //     ground_fault = MCP23017_IOExpGpioLevelGet(GPB4_GND_FLT);
    //     printk("GPIO PIN GPB4_GND_FLT %d\n", ground_fault);

    //     k_msleep(1000);
    // }

/* Read system time from RTC*/
#if 0
    mcp794x_data_t param_data = {0};
    param_data.year = 2023;
    param_data.month = 9;
    param_data.day = 10;
    param_data.hours = 14;
    param_data.minutes = 23;
    param_data.seconds = 20;
    mcp794x_set_datetime(param_data);
#endif

    //GetDateAndTimeFromExtRTC();

    /*initialize EEPROM Chip*/
    // M24M02_SelfTest();
    /*initialize RFID*/

    /*initialize Crypto Chip*/

    /*Init OLED*/

   // oledstartupdisplayprint();
}

#if 0
    /*RFID CS Pin HIGH*/
    GPIO_OnOff(OP_SPI1_CS2, HIGH);
    GPIO_OnOff(OP_SPI1_CS1, HIGH);
#endif