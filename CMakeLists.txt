# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})

# This sample is only compatible with nrf52840dk_nrf52840 because it has
# re-directed the 'boards' directory to a directory that only contains
# this board.
set(BOARD nrf52832_charger)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(EVCharger)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
target_sources(app PRIVATE components/gpio_config/gpio_config.c)
target_sources(app PRIVATE components/adc_services/adc_service.c)
target_sources(app PRIVATE components/i2c_services/i2c_service.c)
target_sources(app PRIVATE components/ioExp_mcp23s17/ioExp_mcp23s17.c)
target_sources(app PRIVATE components/general/general.c)
target_sources(app PRIVATE components/oled_ssd1306/ssd1306.c)
target_sources(app PRIVATE components/oled_ssd1306/font5x7.c)
target_sources(app PRIVATE components/oled_ssd1306/font8x8.c)
target_sources(app PRIVATE components/oled_ssd1306/font10x16.c)
target_sources(app PRIVATE components/oled_ssd1306/font15x24.c)
target_sources(app PRIVATE components/oled_ssd1306/font20x32.c)

target_sources(app PRIVATE components/meter_ade7593/ade7593.c)
target_sources(app PRIVATE components/app_db/app_db.c)
target_sources(app PRIVATE components/app_schedule/app_schedule.c)
target_sources(app PRIVATE components/app_timer/app_timer.c)
target_sources(app PRIVATE components/led_handler/led_handler.c)

target_sources(app PRIVATE components/app_UI/app_UI.c)
target_sources(app PRIVATE components/app_UI/json_parser.c)
target_sources(app PRIVATE components/rtc_mcp79400/external_rtc.c)
target_sources(app PRIVATE components/rtc_mcp79400/rtc_utility.c)
target_sources(app PRIVATE components/rtc_mcp79400/gw_rtc.c)
target_sources(app PRIVATE components/temp_reading/temp.c)

target_sources(app PRIVATE components/cp_pwm/cp_pwm.c)
target_sources(app PRIVATE components/cp_pwm/pwm_handler.c)
target_sources(app PRIVATE components/eeprom_m24m02/eeprom_m24m02.c)
target_sources(app PRIVATE components/volt_curr_monitor/volt_curr_monitor.c)


target_include_directories(app PRIVATE ./)