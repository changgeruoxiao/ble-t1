#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define PWM_LED0_NODE DT_ALIAS(pwm_led0)

#if !DT_NODE_HAS_STATUS(PWM_LED0_NODE, okay)
#error "This board does not provide an enabled pwm-led0 Devicetree alias"
#endif

static const struct pwm_dt_spec led = PWM_DT_SPEC_GET(PWM_LED0_NODE);

/* 0xFFFF is used here only as a lab/test manufacturer identifier. */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
		      0xFF, 0xFF, /* test company identifier */
		      0x42, 0x54, 0x31, /* "BT1" */
		      0x02), /* experiment version */
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE,
		CONFIG_BT_DEVICE_NAME,
		sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static int start_ble_advertising(void)
{
	int err = bt_enable(NULL);

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return err;
	}

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
			      ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return err;
	}

	printk("Advertising started as '%s'\n", CONFIG_BT_DEVICE_NAME);
	return 0;
}

int main(void)
{
	uint32_t brightness = 0;
	int direction = 1;

	if (!pwm_is_ready_dt(&led)) {
		printk("LED PWM device is not ready\n");
		return 0;
	}

	if (start_ble_advertising() != 0) {
		return 0;
	}

	/* Keep the Task 001 breathing LED as a simple "application alive" indicator. */
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
