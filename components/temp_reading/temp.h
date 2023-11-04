
#ifndef RTC_H
#define RTC_H

#include "components/cp_pwm/pwm_handler.h"
#include <stdio.h>
#include "string.h"
#include "components/adc_services/adc_service.h"
#include "components/app_db/app_db.h"
#include "components/general/general.h"


void adctempdataread (void);
float convertAnalogToTemperature(unsigned int analogReadValue);
int averageTemperature(int temperatureValues[]) ;







#endif