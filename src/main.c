#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define PWM_LED0_NODE DT_ALIAS(pwm_led0)

#if !DT_NODE_HAS_STATUS(PWM_LED0_NODE, okay)
#error "This board does not provide an enabled pwm-led0 Devicetree alias"
#endif

#define BT_UUID_BLE_T1_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x7c7c0001, 0x6e6f, 0x4f72, 0x9c5c, 0x7a1b3d0e2f10)
#define BT_UUID_BLE_T1_VALUE_VAL \
	BT_UUID_128_ENCODE(0x7c7c0002, 0x6e6f, 0x4f72, 0x9c5c, 0x7a1b3d0e2f10)

#define GATT_VALUE_MAX_LEN 20U

static const struct pwm_dt_spec led = PWM_DT_SPEC_GET(PWM_LED0_NODE);
static const struct bt_uuid_128 ble_t1_service_uuid = BT_UUID_INIT_128(BT_UUID_BLE_T1_SERVICE_VAL);
static const struct bt_uuid_128 ble_t1_value_uuid = BT_UUID_INIT_128(BT_UUID_BLE_T1_VALUE_VAL);

static uint8_t gatt_value[GATT_VALUE_MAX_LEN] = "hello";
static size_t gatt_value_len = 5U;

static ssize_t read_gatt_value(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr,
			       void *buf, uint16_t len, uint16_t offset)
{
	ARG_UNUSED(attr);

	return bt_gatt_attr_read(conn, attr, buf, len, offset,
				 gatt_value, gatt_value_len);
}

static ssize_t write_gatt_value(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len,
				uint16_t offset, uint8_t flags)
{
	ARG_UNUSED(conn);
	ARG_UNUSED(attr);

	if ((flags & BT_GATT_WRITE_FLAG_PREPARE) != 0U) {
		return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
	}

	if (offset != 0U) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	if (len > sizeof(gatt_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	memcpy(gatt_value, buf, len);
	gatt_value_len = len;

	printk("GATT value written (%u bytes)\n", (unsigned int)len);
	return len;
}

BT_GATT_SERVICE_DEFINE(ble_t1_service,
	BT_GATT_PRIMARY_SERVICE(&ble_t1_service_uuid.uuid),
	BT_GATT_CHARACTERISTIC(&ble_t1_value_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_gatt_value, write_gatt_value, NULL),
);

static int start_advertising(void);

static void connected(struct bt_conn *conn, uint8_t err)
{
	ARG_UNUSED(conn);

	if (err != 0U) {
		printk("Connection failed (err 0x%02x)\n", err);
		return;
	}

	printk("Connected\n");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(conn);
	printk("Disconnected (reason 0x%02x)\n", reason);
}

static void recycled(void)
{
	int err = start_advertising();

	if (err) {
		printk("Advertising restart failed (err %d)\n", err);
	} else {
		printk("Advertising restarted\n");
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.recycled = recycled,
};

/* 0xFFFF is used here only as a lab/test manufacturer identifier. */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_BLE_T1_SERVICE_VAL),
	BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
		      0xFF, 0xFF, /* test company identifier */
		      0x42, 0x54, 0x31, /* "BT1" */
		      0x03), /* experiment version */
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE,
		CONFIG_BT_DEVICE_NAME,
		sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static int start_advertising(void)
{
	return bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
			       ad, ARRAY_SIZE(ad),
			       sd, ARRAY_SIZE(sd));
}

static int start_ble_advertising(void)
{
	int err = bt_enable(NULL);

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return err;
	}

	printk("Bluetooth initialized\n");

	err = start_advertising();
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
