# Task 003 — BLE connection + custom GATT read/write

## Goal

Validate the first connected BLE data path on the physical nRF52840 board using Nordic nRF Connect SDK v3.4.0.

Cloud-side code is already prepared. The local Agent/user must complete the build and physical validation.

Acceptance target:

1. Build the latest repository for `promicro_nrf52840/nrf52840/uf2`.
2. Flash the generated UF2 using the existing `NICENANO` bootloader.
3. Confirm `ble-t1` is still advertising.
4. Connect from a BLE central such as nRF Connect for Mobile.
5. Discover the custom service and characteristic.
6. Read the initial value `hello`.
7. Write a new value such as `task03`.
8. Read again and confirm the new value.
9. Disconnect/reconnect and confirm the RAM value persists until board reset.
10. Optionally capture the connection/ATT traffic with the separate nRF52840 Packet Sniffer.

Do not add notification yet. That belongs to Task 004.

## Code already prepared

### Custom service

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

### Custom value characteristic

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Properties and permissions:

```text
READ | WRITE
```

The value buffer is 20 bytes in RAM and starts as:

```text
hello
```

See `docs/gatt.md` for protocol details.

The advertising packet now also includes the custom service UUID and changes the Task marker to:

```text
FF FF 42 54 31 03
```

The breathing LED remains in the application as the board-alive indicator.

## Local Agent procedure

### 1. Pull and inspect

```bash
git pull
```

Read:

- `AGENTS.md`
- `docs/project-status.md`
- `docs/gatt.md`
- `docs/sniffer.md`
- this task

Do not rewrite the prepared GATT implementation unless the local NCS v3.4.0 build demonstrates a compatibility problem.

### 2. Build

Use the already validated NCS v3.4.0 environment:

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

Acceptance:

- command exits successfully;
- `build/zephyr/zephyr.uf2` exists;
- generated output stays untracked.

If a build error occurs, make the smallest compatibility fix possible and document exactly why it was required.

### 3. Flash

1. Connect the application board by USB.
2. Enter UF2 mode with two quick `RST -> GND` resets.
3. Confirm `NICENANO` appears.
4. Copy `build/zephyr/zephyr.uf2` to it.
5. Confirm the volume disconnects automatically.
6. Confirm the breathing LED still behaves normally.

### 4. Connect with a phone or PC BLE central

Preferred first tool: nRF Connect for Mobile.

Expected device:

```text
ble-t1
```

Connect and discover services.

Expected custom primary service:

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Expected characteristic:

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Expected characteristic capabilities:

```text
Read
Write
```

### 5. Read/write test

Read immediately after board reset.

Expected bytes:

```text
68 65 6C 6C 6F
```

Expected ASCII:

```text
hello
```

Write a complete replacement value, for example:

```text
task03
```

Hex:

```text
74 61 73 6B 30 33
```

Read again and confirm the returned bytes match the value just written.

Then disconnect and reconnect without resetting the board. The value should still be `task03` because it remains in RAM.

Reset the board and reconnect; the initial value should return to `hello`.

### 6. Optional host automation

If Windows Bluetooth is enabled and `bleak` is available, the local Agent may write a temporary/local Python test that:

1. scans for `ble-t1`;
2. connects;
3. reads characteristic UUID `7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10`;
4. writes `task03`;
5. reads again;
6. disconnects.

Do not add throwaway host scripts to the repository unless they are cleaned up into a reusable test tool.

### 7. Optional Sniffer validation

Task 002 already proved that the dongle and command-line sniffer work.

For Task 003, a capture may be taken around the connection/read/write sequence. Useful evidence includes:

```text
CONNECT_IND / connection establishment
ATT Read Request / Read Response
ATT Write Request / Write Response
```

Exact Wireshark labels may differ by dissector/tooling version. If following the connection with the sniffer requires additional target-selection setup, document it but do not let that block GATT functional acceptance.

Do not commit captures containing unrelated nearby BLE traffic.

## Report back

Update `docs/project-status.md` and push a commit recording:

- whether cloud-side code built unchanged;
- exact build command;
- UF2 path and size;
- whether breathing LED still works;
- whether `ble-t1` connected successfully;
- whether custom service/characteristic discovery succeeded;
- initial read result;
- write payload;
- read-back result;
- disconnect/reconnect result;
- reset result;
- optional Sniffer/PCAP observations;
- any compatibility fix required.

Stop after Task 003 verification. Do not implement notification until Task 004 is prepared.
