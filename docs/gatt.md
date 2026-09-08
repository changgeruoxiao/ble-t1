# ble-t1 GATT protocol notes

This document records the custom Bluetooth LE GATT interface introduced in Task 003 and extended in Task 004.

## Service

Custom primary service UUID:

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

The service UUID is also included in the advertising payload so a scanner can identify the application by service UUID as well as by device name.

## Value characteristic

Characteristic UUID:

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Properties:

```text
READ | WRITE
```

Permissions:

```text
READ | WRITE
```

The characteristic stores up to 20 bytes in RAM.

Initial value after reset:

```text
hello
```

A successful write replaces the entire current value. Offset writes and prepared writes are intentionally rejected so the ATT behavior stays easy to inspect.

Example:

```text
read  -> 68 65 6c 6c 6f          ("hello")
write -> 74 61 73 6b 30 33        ("task03")
read  -> 74 61 73 6b 30 33        ("task03")
```

The value remains in RAM across BLE disconnect/reconnect, but a board reset restores `hello`.

## Telemetry / notification characteristic

Task 004 adds a second characteristic dedicated to peripheral-to-central telemetry:

```text
7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Properties:

```text
READ | NOTIFY
```

The characteristic exposes a 4-byte little-endian `uint32` counter. A CCCD controls whether notifications are enabled. This keeps configuration/control (`...0002`) separate from streaming telemetry (`...0003`).

Detailed Task 004 behavior is documented in [`notification.md`](notification.md).

## Advertising marker

The lab manufacturer marker tracks the current experiment revision:

```text
Task 003: FF FF 42 54 31 03
Task 004: FF FF 42 54 31 04
```

Interpretation:

```text
FF FF     lab/test company identifier only
42 54 31  ASCII "BT1"
03/04     experiment version
```

The `0xFFFF` company identifier is not intended for production use.

## Connection behavior

The application registers Zephyr/NCS connection callbacks and prints simple events such as:

```text
Connected
Disconnected (reason 0x..)
GATT value written (N bytes)
Notifications enabled
Notification counter=N
Notifications disabled
```

These messages are useful for local serial/RTT inspection, while acceptance is based primarily on real central-side BLE behavior.

## Current data-path model

After Task 004 the service has two deliberately different application paths:

```text
Central
  |
  | ATT Read / ATT Write
  v
Value characteristic (...0002)
  |
  v
20-byte RAM control/config value

Peripheral delayed work
  |
  | GATT Notification
  v
Telemetry characteristic (...0003)
  |
  v
Central subscription callback
```

This is the first project stage that resembles the common wireless sensor split between configuration and pushed telemetry.
