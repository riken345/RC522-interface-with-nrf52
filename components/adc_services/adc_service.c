#include "adc_service.h"
#include "string.h"
#include "zephyr/logging/log.h"

int32_t gi32ReadADCValue[ADC_MAX_CHANNEL] = {0};
extern bool bSyncDone;
#if !DT_NODE_EXISTS(DT_NODELABEL(adc))
#error "Overlay for power output node not properly defined."
#endif

#define ADC_NODE DT_NODELABEL(adc)

LOG_MODULE_REGISTER(adcModule, LOG_LEVEL_DBG);

// static uint16_t adc_raw;
int16_t sample_buffer[6];
static const struct device *dev_adc = DEVICE_DT_GET(ADC_NODE);
;

struct adc_channel_cfg channel0_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = CP1_FB_ADC,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0,
};
struct adc_channel_cfg channel1_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = ADC_BRD_THERM1,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput1,
};

struct adc_channel_cfg channel4_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = ADC_VPMAIN,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput4,
};

struct adc_channel_cfg channel5_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = ADC_IP_MAIN,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput5,
};
struct adc_channel_cfg channel6_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = ADC_IPNEUT,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput6,
};
struct adc_channel_cfg channel7_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .differential = 0,
    .channel_id = ADC_VP1,
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput7,
};
void ADC_init(void)
{
    int err;
    if (!device_is_ready(dev_adc))
    {
        printk("ADC dev not found");
        return;
    }
    // dev_adc = device_get_binding(ADC_DEV);
    // if (!device_is_ready(dev_adc)) {
    // 	printk("ADC device not found\n");
    // 	return;
    // }
    err = adc_channel_setup(dev_adc, &channel0_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
    err = adc_channel_setup(dev_adc, &channel1_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
    err = adc_channel_setup(dev_adc, &channel4_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
    err = adc_channel_setup(dev_adc, &channel5_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
    err = adc_channel_setup(dev_adc, &channel6_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
    err = adc_channel_setup(dev_adc, &channel7_cfg);
    if (err)
    {
        printk("adc_channel_setup() failed with error: %d\n", err);
    }
}

static enum adc_action adc0_callback(const struct device *dev, struct adc_sequence *seq, int error)
{
    ARG_UNUSED(seq);
#if 0
    for (uint8_t i = 0; i < 6; i++)
    {
        printk("adc_val %d : %u\r\n", i, sample_buffer[i]);
    }
#endif
    if (true == bSyncDone)
    {
        gi32ReadADCValue[ADC_PILOT_FEEDBACK_CHANNEL] = sample_buffer[0]; // AIN0
    }

    gi32ReadADCValue[ADC_BRD_THERML_CHANNEL] = sample_buffer[1];  // AIN1
    gi32ReadADCValue[ADC_VLINE_CHANNEL] = sample_buffer[2];       // AIN4
    gi32ReadADCValue[ADC_CURR_LOAD_CHANNEL] = sample_buffer[3];   // AIN5
    gi32ReadADCValue[ADC_CURR_OFFSET_CHANNEL] = sample_buffer[4]; // AIN6
    gi32ReadADCValue[ADC_VOLT_OFFSET_CHANNEL] = sample_buffer[5]; // AIN7
    memset(sample_buffer, 0, sizeof(sample_buffer));
    return ADC_ACTION_CONTINUE; // ADC_ACTION_REPEAT ,ADC_ACTION_FINISH
}

void adc0_read_raw(void)
{
    int err;
    struct adc_sequence_options _options = {
        .callback = &adc0_callback,
        .interval_us = 10,
    };
    struct adc_sequence sequence = {
        .channels = BIT(channel0_cfg.channel_id) | BIT(channel1_cfg.channel_id) | BIT(channel4_cfg.channel_id),
         .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0,
        .options = &_options,
    };
    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        // printk("ADC reading failed with error %d.\n", err); //-EBUSY
        return;
    }
}
#if 0
void adc1_read_raw(void)
{
    int err;

    struct adc_sequence sequence = {
        .channels = BIT(channel1_cfg.channel_id),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0
    };
    // const struct device *dev_adc = init_adc(channel1_cfg.channel_id);

    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d.\n", err);
        return;
    }
    gi32ReadADCValue[ADC_BRD_THERML_CHANNEL] = sample_buffer[0];
    memset(sample_buffer, 0, sizeof(sample_buffer));
}

void adc4_read_raw(void)
{
    int err;

    struct adc_sequence sequence = {
        .channels = BIT(channel4_cfg.channel_id),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0
    };
    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d.\n", err);
        return;
    }
    gi32ReadADCValue[ADC_VLINE_CHANNEL]  = sample_buffer[0];
    memset(sample_buffer, 0, sizeof(sample_buffer));
}

void adc5_read_raw(void)
{
    int err;

    struct adc_sequence sequence = {
        .channels = BIT(channel5_cfg.channel_id),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0
    };
    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d.\n", err);
        return;
    }
    gi32ReadADCValue[ADC_CURR_LOAD_CHANNEL] = sample_buffer[0];
    memset(sample_buffer, 0, sizeof(sample_buffer));
}

void adc6_read_raw(void)
{
    int err;

    struct adc_sequence sequence = {
        .channels = BIT(channel6_cfg.channel_id),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0
    };
    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d.\n", err);
        return;
    }
    gi32ReadADCValue[ADC_CURR_OFFSET_CHANNEL] =  sample_buffer[0];
    memset(sample_buffer, 0, sizeof(sample_buffer));
}

void adc7_read_raw(void)
{
    int err;

    struct adc_sequence sequence = {
        .channels = BIT(channel7_cfg.channel_id),
        .buffer = sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
        .oversampling = 0,
        .calibrate = 0
    };
    err = adc_read(dev_adc, &sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d.\n", err);
        return;
    }
    gi32ReadADCValue[ADC_VOLT_OFFSET_CHANNEL] = sample_buffer[0];
    memset(sample_buffer, 0, sizeof(sample_buffer));
}
#endif