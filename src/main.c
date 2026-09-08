#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define PWM_LED0_NODE DT_ALIAS(pwm_led0)

#if !DT_NODE_HAS_STATUS(PWM_LED0_NODE, okay)
#error "This board does not provide an enabled pwm-led0 Devicetree alias"
#endif

static const struct pwm_dt_spec led = PWM_DT_SPEC_GET(PWM_LED0_NODE);

int main(void)
{
	uint32_t brightness = 0;
	int direction = 1;

	if (!pwm_is_ready_dt(&led)) {
		printk("LED PWM device is not ready\n");
		return 0;
	}

	while (1) {
		uint32_t pulse = (uint32_t)(((uint64_t)led.period * brightness) / 100U);
		int ret = pwm_set_dt(&led, led.period, pulse);

		if (ret < 0) {
			printk("Failed to set LED PWM: %d\n", ret);
			return 0;
		}

		k_msleep(20);

		if (direction > 0) {
			if (brightness == 100U) {
				direction = -1;
			} else {
				brightness++;
			}
		} else if (brightness == 0U) {
			direction = 1;
		} else {
			brightness--;
		}
	}
}
