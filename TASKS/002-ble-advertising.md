# Task 002 — BLE advertising + first air capture

## Goal

Complete the first Bluetooth LE experiment on the physical ProMicro / SuperMini nRF52840 board using Nordic nRF Connect SDK v3.4.0:

1. Build the repository after the cloud-side advertising implementation was added.
2. Flash the generated UF2 through the existing `NICENANO` bootloader.
3. Confirm that the board continues to run the breathing LED.
4. Confirm that a BLE scanner sees a device named `ble-t1`.
5. Bring up the separate nRF52840 Packet Sniffer and capture `ble-t1` advertising packets.

The cloud-side changes intentionally stop before local build, physical flash, serial-port discovery, Wireshark setup and RF validation. Those steps belong to the local agent/user because they require the installed NCS toolchain and physical USB devices.

## Code already prepared

The repository now enables:

```text
CONFIG_BT=y
CONFIG_BT_PERIPHERAL=y
CONFIG_BT_DEVICE_NAME="ble-t1"
CONFIG_BT_DEVICE_NAME_DYNAMIC=n
CONFIG_BT_MAX_CONN=1
```

`src/main.c` now:

- initializes Bluetooth with `bt_enable(NULL)`;
- starts legacy connectable/scannable advertising with `BT_LE_ADV_CONN_FAST_1`;
- exposes complete device name `ble-t1` in scan response data;
- includes a recognizable manufacturer-data marker:

```text
FF FF 42 54 31 02
      B  T  1
```

`0xFFFF` is used only as a lab/test identifier and is not intended as a production Bluetooth SIG company identifier.

The Task 001 PWM breathing LED is deliberately kept running so that BLE bring-up does not remove the previously validated board-alive indication.

## Local-agent procedure

### 1. Pull and inspect

```bash
git pull
```

Read:

- `AGENTS.md`
- `docs/project-status.md`
- `docs/sniffer.md`
- this task

Do not rewrite the prepared BLE implementation unless the local build proves that a change is required.

### 2. Verify NCS environment

Use the already working nRF Connect SDK v3.4.0 toolchain environment.

Record at minimum:

```bash
west --version
west topdir
```

Confirm that the selected SDK is still NCS v3.4.0 and that the board target exists:

```text
promicro_nrf52840/nrf52840/uf2
```

### 3. Build

From the repository root:

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

Acceptance:

- build succeeds;
- `build/zephyr/zephyr.uf2` exists;
- generated build output remains untracked.

If the build fails, fix only the minimum compatibility issue required by NCS v3.4.0 and document the reason.

### 4. Flash and physical validation

1. Connect the application board by USB.
2. Enter UF2 mode using two quick `RST -> GND` resets.
3. Confirm `NICENANO` appears.
4. Copy `build/zephyr/zephyr.uf2` to the volume.
5. Confirm the volume disconnects after programming.
6. Confirm the board breathing LED still runs.

Do not claim physical validation until the user actually observes the result.

### 5. BLE scanner validation

Use nRF Connect for Mobile or another BLE scanner.

Expected observation:

```text
Device name: ble-t1
Advertising: present and repeated
```

If available, record RSSI and the observed Bluetooth address, noting that the address can be random and should not be committed as a permanent board identity.

### 6. Sniffer validation

Follow `docs/sniffer.md`.

The first capture only needs to prove that the separate sniffer can see `ble-t1` advertising traffic. Do not begin GATT or pairing work in this task.

Preferred evidence:

- Wireshark lists an interface similar to `nRF Sniffer for Bluetooth LE COMx`;
- `ble-t1` appears in the sniffer device list;
- captured advertising/scan-response packets contain the name `ble-t1` and/or the marker bytes `FF FF 42 54 31 02`;
- save one `.pcap` or `.pcapng` capture locally if useful, but do not commit large captures unless explicitly requested.

## What to report back

After the local agent completes the task, update `docs/project-status.md` and report:

- whether the code built unchanged or required fixes;
- resulting UF2 path and size;
- whether the LED still breathes;
- whether a phone/PC scanner sees `ble-t1`;
- the sniffer COM port;
- whether Wireshark/nrfutil sees the sniffer;
- whether `ble-t1` packets were captured;
- any sniffer firmware or driver action that was necessary.

Stop after successful advertising capture. Task 003 will add connection/GATT behavior.
