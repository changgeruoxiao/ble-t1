# Task 004 — periodic GATT notification

## Goal

Extend the verified Task 003 GATT service with a dedicated telemetry characteristic and prove the peripheral-to-central notification path on the physical nRF52840 board.

Cloud-side code preparation is already committed. The local Agent/user must complete the real NCS build, UF2 flash and BLE subscription validation.

## Prepared protocol

Primary Service UUID:

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Existing Task 003 read/write characteristic:

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
READ | WRITE
```

New Task 004 telemetry characteristic:

```text
7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10
READ | NOTIFY
```

Notification payload:

```text
uint32 little-endian counter, 4 bytes
```

Expected sequence after subscription:

```text
01 00 00 00
02 00 00 00
03 00 00 00
...
```

Interval: approximately 1 second.

Task 004 advertising marker:

```text
FF FF 42 54 31 04
```

Read `docs/notification.md` for the protocol details and current static GATT attribute layout.

## Local-agent procedure

### 1. Pull and inspect

```bash
git pull
```

Read:

- `AGENTS.md`
- `docs/project-status.md`
- `docs/gatt.md`
- `docs/notification.md`
- `docs/sniffer.md`
- this task

Do not rewrite the prepared implementation unless an actual NCS v3.4.0 compile/runtime issue requires a minimal fix.

### 2. Use the already verified NCS environment

Confirm that the project still uses Nordic nRF Connect SDK v3.4.0 and the verified board target:

```text
promicro_nrf52840/nrf52840/uf2
```

Remember the previously observed local Python contamination issue. If it recurs, keep the fix scoped to the local build environment; do not modify NCS/Zephyr sources.

### 3. Build

From the repository root:

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

Acceptance:

- build exits successfully;
- `build/zephyr/zephyr.uf2` exists;
- generated build output remains untracked;
- if a code change is required for NCS v3.4.0 compatibility, record the exact reason.

Pay particular attention to these Task 004 APIs/macros:

```text
BT_GATT_CHRC_NOTIFY
BT_GATT_CCC
bt_gatt_notify
k_work_init_delayable
k_work_reschedule
```

### 4. Flash

1. Connect the ProMicro/SuperMini application board.
2. Enter the `NICENANO` UF2 bootloader with two quick `RST -> GND` resets.
3. Copy `build/zephyr/zephyr.uf2` to the UF2 volume.
4. Confirm the volume disconnects after flashing.
5. Confirm the breathing LED still operates.

Do not claim hardware validation until the physical LED/result is observed.

### 5. Discover the GATT database

Use Python `bleak`, nRF Connect for Mobile, or another central.

Confirm all of the following:

```text
Service: 7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
Value:   7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10  [read, write]
Notify:  7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10  [read, notify]
```

The existing Task 003 characteristic must continue to work. At minimum, verify that its current reset value can still be read as `hello` after a fresh board reset.

### 6. Subscribe and verify notifications

Preferred automated validation with `bleak`:

1. scan for `ble-t1`;
2. connect;
3. call `start_notify()` on UUID `7c7c0003-...`;
4. collect at least 5 notifications;
5. decode each payload as little-endian `uint32`;
6. verify the sequence is monotonically increasing and normally begins at `1`;
7. confirm the interval is roughly 1 second rather than requiring exact real-time precision.

Expected example:

```text
1
2
3
4
5
```

Also verify the raw payload length is exactly 4 bytes.

### 7. Stop and resubscribe

Verify CCC behavior:

1. stop notifications / unsubscribe;
2. wait at least 2 seconds and confirm no further callbacks arrive;
3. subscribe again;
4. confirm the sequence restarts from `1` as currently designed.

This proves that the peripheral reacts to the CCCD state rather than transmitting continuously regardless of subscription.

### 8. Disconnect/reconnect

Disconnect the central, reconnect, and subscribe again. Confirm notifications can be enabled again and the board resumes normal advertising after disconnect.

This also protects the Task 003 `.recycled` advertising restart behavior from regression.

### 9. Optional Sniffer evidence

Task 004 functional acceptance does not require Wireshark GUI, but the existing Packet Sniffer can be used if convenient.

Useful evidence includes:

- connection setup;
- ATT write to the CCCD enabling notification;
- Handle Value Notification PDUs;
- repeated 4-byte counter payloads.

Do not commit captures containing unrelated nearby-device traffic.

## What to report back

Update `docs/project-status.md` and push a completion commit containing:

- build result and whether cloud code required fixes;
- UF2 path and size;
- breathing LED result;
- discovery of UUID `...0003` and its properties;
- at least 5 decoded notification values;
- approximate notification interval;
- unsubscribe result;
- resubscribe result;
- disconnect/reconnect result;
- whether existing Task 003 read/write behavior regressed;
- optional Sniffer evidence if performed.

Stop after Task 004 notification is verified. Do not yet add a physical sensor or redesign the payload into a production protocol; that belongs to later tasks.
