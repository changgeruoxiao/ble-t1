# ble-t1 GATT protocol notes

This document records the custom Bluetooth LE GATT interface introduced in Task 003.

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

No pairing, bonding, authentication, encryption, notification or indication is required in Task 003. Security and notification are intentionally deferred to later tasks.

The characteristic stores up to 20 bytes in RAM.

Initial value after reset:

```text
hello
```

A successful write replaces the entire current value. Offset writes and prepared writes are intentionally rejected in this first experiment so the ATT behavior stays easy to inspect.

Example:

```text
read  -> 68 65 6c 6c 6f          ("hello")
write -> 74 61 73 6b 30 33        ("task03")
read  -> 74 61 73 6b 30 33        ("task03")
```

The value remains in RAM across BLE disconnect/reconnect, but a board reset restores `hello`.

## Advertising marker for Task 003

Task 003 changes the lab manufacturer marker to:

```text
FF FF 42 54 31 03
```

Interpretation:

```text
FF FF     lab/test company identifier only
42 54 31  ASCII "BT1"
03        experiment version 03
```

The `0xFFFF` company identifier is not intended for production use.

## Connection behavior

The application registers Zephyr/NCS connection callbacks and prints simple events:

```text
Connected
Disconnected (reason 0x..)
GATT value written (N bytes)
```

These messages are useful for local serial/RTT inspection, but Task 003 acceptance is based primarily on successful BLE connection and characteristic read/write behavior.

## Why this task is deliberately small

Task 003 establishes the minimum application data path:

```text
Central (phone/PC)
        |
        | ATT Read / ATT Write
        v
GATT characteristic
        |
        v
20-byte RAM value on nRF52840
```

Task 004 will extend this same characteristic or a dedicated data characteristic with `NOTIFY`, which is closer to the data-upload model used by a wireless sensor node.
