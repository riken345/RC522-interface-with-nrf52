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
 * \file     ssd1306.c
 * \brief
 */

/************************************************************************
 * Include Header Files
 ************************************************************************/
// #include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include <zephyr/kernel.h>
#include "zephyr/logging/log.h"
#include "components/general/general.h"
#include "components/oled_ssd1306/ssd1306.h"
#include "components/cp_pwm/cp_pwm.h"
#include "components/temp_reading/temp.h"
/************************************************************************
 * Define Macros
 ************************************************************************/
#if defined(CONFIG_SSD16XX)
#define DISPLAY_DRIVER "SSD16XX"
#endif
#if defined(CONFIG_SSD1306)
#define DISPLAY_DRIVER "SSD1306"
#endif
#ifndef DISPLAY_DRIVER
#define DISPLAY_DRIVER "DISPLAY"
#endif

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

LOG_MODULE_REGISTER(display, LOG_LEVEL_DBG);
#define SELECTED_FONT_INDEX 4       // perhaps make this a config parameter
#define LARGE_SELECTED_FONT_INDEX 0 // perhaps make this a config parameter
/************************************************************************
 * Define Enumeration/Structure/Unions
 ************************************************************************/

/************************************************************************
 * Define Private Variables
 ************************************************************************/

/************************************************************************
 * Define Global Variables
 ************************************************************************/
uint16_t rows;
uint8_t ppt;
uint8_t font_width;
uint8_t font_height;
const struct device *dev;
struct k_mutex lockoledprint;
uint8_t display_clear_flag = 0;
extern uint32_t gui32VoltLineRMSVal;
extern u_int8_t avgTemp;
/************************************************************************
 * Define Private Function Prototypes
 ************************************************************************/
uint8_t ssd1306_oled_init(void)
{
    int16_t lockoledprintStatus = 0U;
    /** Mutex initialization */
    lockoledprintStatus = k_mutex_init(&lockoledprint);
    dev = device_get_binding(DISPLAY_DRIVER);
    if (dev == NULL)
    {
        LOG_ERR("Device not found\n");
        return 0;
    }
    if (display_set_pixel_format(dev, PIXEL_FORMAT_MONO10) != 0)
    {
        LOG_ERR("Failed to set required pixel format\n");
        return 0;
    }

    LOG_DBG("initialized %s\n", DISPLAY_DRIVER);

    if (cfb_framebuffer_init(dev))
    {
        LOG_ERR("Framebuffer initialization failed!\n");
        return 0;
    }

    cfb_framebuffer_clear(dev, true);

    display_blanking_off(dev);
    rows = cfb_get_display_parameter(dev, CFB_DISPLAY_ROWS);
    ppt = cfb_get_display_parameter(dev, CFB_DISPLAY_PPT);

    int num_fonts = cfb_get_numof_fonts(dev);

    for (int idx = 0; idx < num_fonts; idx++)
    {

        cfb_get_font_size(dev, idx, &font_width, &font_height);

        LOG_INF("Index[%d] font dimensions %2dx%d",
                idx, font_width, font_height);
    }

    // cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);

    // LOG_INF("Selected font: index[%d]", SELECTED_FONT_INDEX);

    LOG_DBG("x_res %d, y_res %d, ppt %d, rows %d, cols %d\n",
            cfb_get_display_parameter(dev, CFB_DISPLAY_WIDTH),
            cfb_get_display_parameter(dev, CFB_DISPLAY_HEIGH),
            ppt,
            rows,
            cfb_get_display_parameter(dev, CFB_DISPLAY_COLS));
    return 0;
}
uint8_t ssd1306_oled_print(void)
{
    // for (int i = 0; i < rows; i++) {
    // cfb_framebuffer_clear(dev, false);
    // if (cfb_print(dev,
    // 	      "0123456=",
    // 	      0, i * ppt)) {
    // 	LOG_ERR("Failed to print a string\n");
    // 	continue;
    // }
    // }
    k_sleep(K_MSEC(300));
    cfb_framebuffer_clear(dev, false);
    cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
    cfb_print(dev, "0123456", 0, 0);
    cfb_framebuffer_finalize(dev);
    return 0;
}
void display_init(void)
{
    dev = device_get_binding(DISPLAY_DRIVER);

    if (dev == NULL)
    {
        LOG_ERR("Device not found");
        return;
    }

    if (display_set_pixel_format(dev, PIXEL_FORMAT_MONO10) != 0)
    {
        LOG_ERR("Failed to set required pixel format");
        return;
    }

    LOG_INF("Binding to %s", DISPLAY_DRIVER);

    if (cfb_framebuffer_init(dev))
    {
        LOG_ERR("Framebuffer initialization failed!");
        return;
    }

    cfb_framebuffer_clear(dev, true);

    display_blanking_off(dev);

    rows = cfb_get_display_parameter(dev, CFB_DISPLAY_ROWS);
    ppt = cfb_get_display_parameter(dev, CFB_DISPLAY_PPT);

    int num_fonts = cfb_get_numof_fonts(dev);

    for (int idx = 0; idx < num_fonts; idx++)
    {

        cfb_get_font_size(dev, idx, &font_width, &font_height);

        LOG_INF("Index[%d] font dimensions %2dx%d",
                idx, font_width, font_height);
    }

    cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);

    LOG_INF("Selected font: index[%d]", SELECTED_FONT_INDEX);

    cfb_framebuffer_invert(dev); // white on black

    LOG_INF("x_res %d, y_res %d, ppt %d, rows %d, cols %d",
            cfb_get_display_parameter(dev, CFB_DISPLAY_WIDTH),
            cfb_get_display_parameter(dev, CFB_DISPLAY_HEIGH),
            ppt,
            rows,
            cfb_get_display_parameter(dev, CFB_DISPLAY_COLS));
}

/************************************************************************
 * Define Global Function Prototypes
 ************************************************************************/
void oledstartupdisplayprint(void)
{
    cfb_framebuffer_clear(dev, true);
    k_sleep(K_MSEC(10));
    cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
    k_sleep(K_MSEC(10));
    // ssd1306_setFixedFont(ssd1306xled_font8x16);
    // ssd1306_setColor(RGB_COLOR8(255, 255, 0));
    // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
    if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
    {
        cfb_print(dev, " FOXVOLT P1 V0.1", 0, 10);
        cfb_framebuffer_finalize(dev);
        // ssd1306_printFixed(10, 10, "DEVICE STARTING", STYLE_NORMAL);
        k_mutex_unlock(&lockoledprint);
    }
    // if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
    // {
    //     cfb_print(dev, " please wait..", 20, 24);
    //     cfb_framebuffer_finalize(dev);
    //     // ssd1306_printFixed(20, 24, "PLEASE WAIT...", STYLE_NORMAL);
    //     k_mutex_unlock(&lockoledprint);
    // }
    // ssd1306_positiveMode();

    k_sleep(K_MSEC(1000));
    // cfb_framebuffer_clear(dev, true);
    display_clear_flag = 1;
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
}
void OledPrintData(void)
{
    Metering_info_spi_t meter_info = {0U};
    // fault_info_t fault_info = {0U};
    // heartbeat_info_t hb_info = {0U};

    uint8_t Voltage[8U];
    uint8_t Current[8U];
    uint8_t power[13U];
    uint8_t energy[13U];
    uint8_t temp[8U];

    uint32_t Volt = 240;
    float Curr = 1.0f;
    float pow = 0.0f;
    float ener = 0.0f;
    static float Start_energy = 0.0f;
    float session_energy = 0.0f;
    int tempchere =0;

    uint8_t ui8EVState = 0U;

    cfb_framebuffer_clear(dev, true);
    OLED_Current_limit_Print(CURRENT_LIMIT_15);

    GetSPIMeteringInfo(&meter_info);

    Volt = (gui32VoltLineRMSVal / 1000);
    Curr = (float)((float)meter_info.RPhase_current / (float)1000);
    pow = (float)((float)meter_info.RPhase_power / (float)1000);
    ener = (float)((float)meter_info.RPhase_session_energy / (float)1000);
    session_energy = ener - Start_energy;
    tempchere = avgTemp; 

    sprintf((char *)Voltage, "%dV", (gui32VoltLineRMSVal / 1000));
    sprintf((char *)Current, "%04.1fA", Curr-0.06);
    sprintf((char *)power, "%03.1fKW", pow);
    sprintf((char *)energy, "%05.2fKWH", session_energy);
    sprintf((char*)temp, "%dC",avgTemp);
    // ESP_LOGI(TAG,"---->>>> P %s E %s V %s I %s",power,energy,Voltage,Current);
    // if(WEBSOCKET_CONNECTED  == PING_i8WebSocketStatus)
    if (1)
    {
        if (1U == display_clear_flag)
        {
            display_clear_flag = 0;
            cfb_framebuffer_clear(dev, true);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            // ssd1306_setFixedFont(ssd1306xled_font8x16);
        }
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            // cfb_draw_line(dev, 0, 33);
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            cfb_print(dev, "________________", 0, 13); // ToDO Need to Fix
            // ssd1306_drawLine(0, 33, 128, 33);
            k_mutex_unlock(&lockoledprint);
        }
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_drawLine(0, 49, 128, 49);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            // cfb_draw_line(dev, 0, 49);
            k_mutex_unlock(&lockoledprint);
        }
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_drawLine(0, 16, 128, 16);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            // cfb_draw_line(dev, 0, 16);
            k_mutex_unlock(&lockoledprint);
        }

        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 24, (char *)Voltage, STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, Voltage, 2, 24); // ToDO Need to Fix
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }

        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(75, 24, (char *)Current, STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, Current, 40, 24); // ToDO Need to Fix
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }

        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(75, 41, (char *)power, STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, power, 83, 24); // ToDO Need to Fix
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 41, (char *)energy, STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, energy, 60, 41); // ToDO Need to Fix
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }
         if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 41, (char *)energy, STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, temp, 80, 10); // ToDO Need to Fix
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }
    }
#if 0 // For Error We Have to Fix this
    if (PLUGANDPLAY_MODE == get_cp_mode() || RFID_OFFLINE_MODE == get_cp_mode())
    {
        heartbeat_info_t hb_info = {0U};
        GetHeartbeatInfo(&hb_info,1);
        ui8EVState = hb_info.ev_current_state;
    }
    else if (OCPP_MODE == get_cp_mode())
    {
        ui8EVState = CP.port_status[1];
    }

#endif

#if (1)

    if (MOBILE_APP_MODE == get_charger_mode())
    {
        heartbeat_info_t hb_info = {0U};
        GetHeartbeatInfo(&hb_info);
        ui8EVState = hb_info.ev_current_state;
    }
    if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
    {
        switch (ui8EVState)
        {
        case Available:
        {
            // ssd1306_printFixed(0, 57, (char *)"AVAILABLE", STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, " 12V", 30, 10);
            cfb_framebuffer_finalize(dev);
            Start_energy = (float)((float)meter_info.RPhase_session_energy / (float)1000);
        }
        break;
        case Preparing:
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 57, (char *)"PREPARING", STYLE_NORMAL); // 98
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, " 9V", 30, 10);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case Charging:
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 57, (char *)" CHARGING", STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, " 6V", 30, 10);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case Finishing:
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 57, (char *)"FINISHING", STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, " 0V", 30, 10);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case Faulted:
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 57, (char *)"  FAULTED", STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, "  -12V", 30, 10);
            cfb_framebuffer_finalize(dev);
            // Start_energy = (float)((float)meter_info.RPhase_session_energy / (float)1000);
        }
        break;
        default:
        {
            // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
            // ssd1306_printFixed(0, 57, (char *)"AVAILABLE", STYLE_NORMAL);
            cfb_framebuffer_set_font(dev, SELECTED_FONT_INDEX);
            cfb_print(dev, " 12V", 30, 10);
            cfb_framebuffer_finalize(dev);
        }
        break;
        }
        k_mutex_unlock(&lockoledprint);
    }
    else
    {
        display_clear_flag = 1;
        cfb_framebuffer_clear(dev, true);
        // ssd1306_setFixedFont(ssd1306xled_font8x16);
        // ssd1306_setColor(RGB_COLOR8(255, 255, 0));
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_printFixed(10, 10, "CONNECTING TO  ", STYLE_NORMAL);
            cfb_print(dev, "CONNECTING TO ", 10, 10);
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }
        if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
        {
            // ssd1306_printFixed(20, 24, "SERVER.......", STYLE_NORMAL);
            cfb_print(dev, "SERVER.......", 10, 10);
            cfb_framebuffer_finalize(dev);
            k_mutex_unlock(&lockoledprint);
        }
        display_clear_flag = 1;
    }
#endif
}
void OLED_Current_limit_Print(int Limit)
{
    cfb_framebuffer_clear(dev, true);
    cfb_framebuffer_set_font(dev, LARGE_SELECTED_FONT_INDEX);
    if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
    {
        switch (Limit)
        {
        case CURRENT_LIMIT_6:
        {
            cfb_print(dev, "[6A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case CURRENT_LIMIT_9_6:
        {
            cfb_print(dev, "[10A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case CURRENT_LIMIT_15:
        {
            cfb_print(dev, "[15A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case CURRENT_LIMIT_18:
        {
            cfb_print(dev, "[18A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case CURRENT_LIMIT_24:
        {
            cfb_print(dev, "[24A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        case CURRENT_LIMIT_30:
        {
            cfb_print(dev, "[30A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        default:
        {
            cfb_print(dev, "[6A]", 2, 40);
            cfb_framebuffer_finalize(dev);
        }
        break;
        }
        k_mutex_unlock(&lockoledprint);
    }
}
void OLED_time_Print(int sec)
{
    int h, m, s;
    uint8_t time[10U];
    h = (sec / 3600);
    m = (sec - (3600 * h)) / 60;
    s = (sec - (3600 * h) - (m * 60));

    sprintf((char *)time, "%.2d:%.2d:%.2d", h, m, s);
    if (k_mutex_lock(&lockoledprint, TIMIOUT_DELAY) == NRF_OK)
    {
        // ssd1306_setFixedFont(ssd1306xled_font6x8_AB);
        // ssd1306_printFixed(76, 15, (char *)time, STYLE_NORMAL);
        // cfb_framebuffer_set_font(dev, ssd1306xled_font6x8_AB);
        cfb_print(dev, "time", 10, 10); // TODO NEED TO FIX
        cfb_framebuffer_finalize(dev);
        k_mutex_unlock(&lockoledprint);
    }
}

/****************************************************************************
 * END OF FILE
 ****************************************************************************/