#ifndef ADC_SERVICE_H
#define ADC_SERVICE_H


#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include "nrfx_saadc.h"
#include <string.h>
#include <stdio.h>
#include <string.h>



#define ADC_DEV		"adc@40007000"
#define ADC_RESOLUTION			12
#define ADC_GAIN				ADC_GAIN_1_4
#define ADC_REFERENCE			ADC_REF_VDD_1_4//ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 20)

#define CP1_FB_ADC   		0
#define ADC_BRD_THERM1   	1
#define ADC_VPMAIN        	4
#define ADC_IP_MAIN        	5
#define ADC_IPNEUT     	    6
#define ADC_VP1             7


typedef enum eADCChannels {
    ADC_VLINE_CHANNEL = 0,
    ADC_VOLT_OFFSET_CHANNEL,
    ADC_CURR_LOAD_CHANNEL,
    ADC_CURR_OFFSET_CHANNEL,
    ADC_PILOT_FEEDBACK_CHANNEL,
    ADC_BRD_THERML_CHANNEL,
    ADC_MAX_CHANNEL,
}__attribute__((__packed__)) eADCChannels_e;

#if 0
/**
 * @enum eADCChannels_e
 * @brief ADC channel number
*/
#endif
void ADC_init(void);

void adc0_read_raw(void);

void adc1_read_raw(void);

void adc4_read_raw(void);

void adc5_read_raw(void);

void adc6_read_raw(void);

void adc7_read_raw(void);
#endif