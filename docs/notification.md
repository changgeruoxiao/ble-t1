# ble-t1 notification protocol notes

This document records the notification data path introduced in Task 004.

## Why Task 004 adds a second characteristic

Task 003 already has a general-purpose read/write characteristic:

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Task 004 keeps that interface unchanged and adds a dedicated telemetry characteristic instead of mixing configuration data and streaming data into one attribute. This is closer to a practical wireless sensor design:

```text
control/config characteristic -> READ | WRITE
telemetry characteristic      -> READ | NOTIFY
```

## Service

Primary Service UUID remains:

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

## Existing value characteristic

UUID:

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Properties remain:

```text
READ | WRITE
```

Its RAM behavior from Task 003 is intentionally preserved.

## New notification characteristic

UUID:

```text
7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Properties:

```text
READ | NOTIFY
```

Permissions:

```text
READ
```

A Client Characteristic Configuration Descriptor (CCCD) is attached by `BT_GATT_CCC(...)`. A central must subscribe before the peripheral sends notifications.

## Payload

The notification payload is deliberately small and deterministic:

```text
4 bytes, unsigned 32-bit little-endian counter
```

Examples:

```text
counter 1 -> 01 00 00 00
counter 2 -> 02 00 00 00
counter 3 -> 03 00 00 00
```

The counter resets to zero when notification subscription is enabled. The first scheduled notification therefore carries value `1`.

The characteristic can also be read. A read returns the current counter in the same 4-byte little-endian representation.

## Timing

When notifications are enabled:

```text
subscribe
   -> immediate work scheduling
   -> counter 1
   -> approximately 1 second
   -> counter 2
   -> approximately 1 second
   -> counter 3
   -> ...
```

The implementation uses Zephyr delayed work instead of blocking the existing PWM breathing loop.

When the CCCD is disabled or the BLE connection disconnects, the delayed notification work is cancelled.

## GATT attribute layout note

The current static service expands to these logical attributes:

```text
0  Primary Service
1  Value Characteristic declaration
2  Value Characteristic value
3  Notify Characteristic declaration
4  Notify Characteristic value
5  CCCD
```

The Task 004 implementation passes `ble_t1_service.attrs[4]` to `bt_gatt_notify()` because notifications must reference the characteristic value attribute. The code names this index `NOTIFY_VALUE_ATTR_INDEX` so the dependency is explicit.

If the service layout changes later, this index must be reviewed. A later refactor may replace the fixed index with a helper or explicit service structure if the protocol grows.

## Advertising marker

Task 004 upgrades the lab manufacturer marker to:

```text
FF FF 42 54 31 04
```

Interpretation:

```text
FF FF     lab/test company identifier only
42 54 31  ASCII "BT1"
04        experiment version 04
```

## Expected central behavior

A phone or PC central should be able to:

1. connect to `ble-t1`;
2. discover the custom service;
3. discover the existing read/write characteristic;
4. discover the new read/notify characteristic;
5. enable notifications on UUID `...0003`;
6. receive monotonically increasing 4-byte little-endian counters;
7. disable notifications and observe that updates stop;
8. subscribe again and observe the sequence restart from `1`.

## Relationship to a sensor product

This counter is only test data. In a real wireless vibration/condition-monitoring node, the same path can later carry sampled or calculated telemetry such as:

```text
RMS
peak
battery voltage
temperature
status bits
sequence number
```

Task 004 proves the peripheral-to-central push path before any real sensor is connected.
