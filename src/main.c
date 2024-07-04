#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
    int status;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	status = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (status < 0) {
		return 0;
	}

	while (1) {
		status = gpio_pin_toggle_dt(&led);
		if (status < 0) {
			return 0;
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
