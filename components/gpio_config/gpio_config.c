#include "gpio_config.h"
#include <zephyr/drivers/gpio.h>
#include "string.h"

#if !DT_NODE_EXISTS(DT_NODELABEL(estop))
#error "Overlay for power output node not properly defined."
#endif
#if !DT_NODE_EXISTS(DT_NODELABEL(io_rst))
#error "Overlay for power output node not properly defined."
#endif
#if !DT_NODE_EXISTS(DT_NODELABEL(rly1_en))
#error "Overlay for power output node not properly defined."
#endif
#if !DT_NODE_EXISTS(DT_NODELABEL(spi1_cs1))
#error "Overlay for power output node not properly defined."
#endif
#if !DT_NODE_EXISTS(DT_NODELABEL(spi1_cs2))
#error "Overlay for power output node not properly defined."
#endif

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec estop = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(estop), gpios, {0});
static const struct gpio_dt_spec io_rst = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(io_rst), gpios, {0});
static const struct gpio_dt_spec rly1_en = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(rly1_en), gpios, {0});
static const struct gpio_dt_spec spi1_cs1 = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(spi1_cs1), gpios, {0});
static const struct gpio_dt_spec spi1_cs2 = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(spi1_cs2), gpios, {0});

/*

*/
void GPIO_vinit(void)
{
	int ret = 0;
	// Configure Output start
	//****************************************
	if (!gpio_is_ready_dt(&estop))
	{
		printk("The load switch pin GPIO port is not ready.\n");
		return ;
	}
	ret = gpio_pin_configure_dt(&estop, GPIO_OUTPUT_INACTIVE);
	if (ret != 0)
	{
		printk("Configuring GPIO pin failed: %d\n", ret);
	}
	//****************************************
	if (!gpio_is_ready_dt(&io_rst))
	{
		printk("The load switch pin GPIO port is not ready.\n");
	}
	ret = gpio_pin_configure_dt(&io_rst, GPIO_OUTPUT_INACTIVE);
	if (ret != 0)
	{
		printk("Configuring GPIO pin failed: %d\n", ret);
	}
	//****************************************
	if (!gpio_is_ready_dt(&rly1_en))
	{
		printk("The load switch pin GPIO port is not ready.\n");
	}
	ret = gpio_pin_configure_dt(&rly1_en, GPIO_OUTPUT_INACTIVE);
	if (ret != 0)
	{
		printk("Configuring GPIO pin failed: %d\n", ret);
	}
	//****************************************
	if (!gpio_is_ready_dt(&spi1_cs1))
	{
		printk("The load switch pin GPIO port is not ready.\n");
	}
	ret = gpio_pin_configure_dt(&spi1_cs1, GPIO_OUTPUT_INACTIVE);
	if (ret != 0)
	{
		printk("Configuring GPIO pin failed: %d\n", ret);
	}
	//****************************************
	if (!gpio_is_ready_dt(&spi1_cs2))
	{
		printk("The load switch pin GPIO port is not ready.\n");
	}
	ret = gpio_pin_configure_dt(&spi1_cs2, GPIO_OUTPUT_INACTIVE);
	if (ret != 0)
	{
		printk("Configuring GPIO pin failed: %d\n", ret);
	}
	// Configure Output End
	//****************************************
	// Configure Input Start

	// Configure Input End
}
void GPIO_Toggle(uint8_t gpioNo)
{
	int ret = 0;
	switch (gpioNo)
	{
	case OP_IORST:
		ret = gpio_pin_toggle_dt(&io_rst);
		if (ret < 0)
		{

		}
		break;

	case OP_RLY1_EN:
		ret = gpio_pin_toggle_dt(&rly1_en);
		if (ret < 0)
		{
			
		}
		break;

	case OP_SPI1_CS1:
		ret = gpio_pin_toggle_dt(&spi1_cs1);
		if (ret < 0)
		{
			
		}
		break;

	case OP_SPI1_CS2:
		ret = gpio_pin_toggle_dt(&spi1_cs2);
		if (ret < 0)
		{
			
		}
		break;

	default:
		break;
	}
}
void GPIO_OnOff(uint8_t gpioNo, bool status)
{
	int ret = 0;
	switch (gpioNo)
	{
	case OP_IORST:
		ret = gpio_pin_set_dt(&io_rst,status);
		if (ret < 0)
		{
			
		}
		break;

	case OP_RLY1_EN:
		ret = gpio_pin_set_dt(&rly1_en,status);
		if (ret < 0)
		{
			
		}
		break;

	case OP_SPI1_CS1:
		ret = gpio_pin_set_dt(&spi1_cs1,status);
		if (ret < 0)
		{
			
		}
		break;

	case OP_SPI1_CS2:
		ret = gpio_pin_set_dt(&spi1_cs2,status);
		if (ret < 0)
		{
			
		}
		break;

	default:
		break;
	}
}
bool GPIO_read(uint8_t gpioNo)
{
	return 0;
}
/****************************************************************************
 * END OF FILE
 ****************************************************************************/