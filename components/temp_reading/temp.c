#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <math.h>
#include "components/app_schedule/app_schedule.h"
#include "components/app_UI/app_ui.h"
#include "components/rtc_mcp79400/gw_rtc.h"
#include "components/rtc_mcp79400/external_rtc.h"
#include "components/adc_services/adc_service.h"
#include "components/gpio_config/gpio_config.h"
#include "components/oled_ssd1306/ssd1306.h"
#include "components/temp_reading/temp.h"
#include "components/adc_services/adc_service.h"

uint8_t avgTemp = 0;

extern int32_t gi32ReadADCValue[ADC_MAX_CHANNEL];
static uint16_t counter = 0;

void adctempdataread(void) {
    float temperature;
    int temperatureValues[10];
    static int temperatureIndex = 0;

    for (int i = 0; i < 10; i++) {
        temperatureValues[i] = convertAnalogToTemperature(gi32ReadADCValue[ADC_BRD_THERML_CHANNEL]);
    }

    temperatureValues[temperatureIndex] = temperature;
    temperatureIndex++;

    if (temperatureIndex == 10) {
        temperatureIndex = 0;

         avgTemp = averageTemperature(temperatureValues);

        if (counter++ > 1000) {
            counter = 0;
            printk("Average temperature: %d degrees Celsius\n", avgTemp);
        }
    }
}

float convertAnalogToTemperature(unsigned int analogReadValue) {
    // Calculate the ratio of the analog read value to the maximum analog read value.
   // float ratio = analogReadValue / 4096.0f;

    // Calculate the resistance of the temperature sensor.
    //float resistance = 10000.0f / ratio;

    // Calculate the temperature of the sensor using the Steinhart-Hart equation.
    float temperature = 1.0f / ((logf(((10000.0f * analogReadValue) / (4096.0f - analogReadValue)) / 10000.0f) / 3434.0f) + (1.0f / (273.15f + 25.000f))) - 273.15f;

    return temperature;
}

int averageTemperature(int temperatureValues[]) {
    // Calculate the sum of the temperature values.
    int sum = 0;
    
    for (int i = 0; i < 10; i++) {
        sum += temperatureValues[i];
    }

    // Calculate the average temperature.
    int avgTemperature = sum / 10;

    // Return the average temperature as an integer value.
    return avgTemperature;
}
