# ble-t1

nRF52840 / Bluetooth LE 学习与实验工程。

## Development stack

本项目的主开发栈明确为 **Nordic nRF Connect SDK (NCS)**。

```text
Nordic nRF Connect SDK
├── Zephyr RTOS / device model
├── Nordic nrfx / nrfxlib
├── Nordic Bluetooth controller and libraries
├── MCUboot and related components
└── west / CMake / Kconfig / Devicetree tooling
```

因此，本仓库会使用 Zephyr 的应用结构和 `west` 构建命令，但目标不是单独搭建 upstream Zephyr 环境，而是优先使用 **Nordic 官方 nRF Connect SDK + Nordic toolchain**。

除非任务明确要求，否则不要切换到旧版 nRF5 SDK、Arduino、PlatformIO 或独立 upstream Zephyr。

## Hardware

- Board: ProMicro / SuperMini nRF52840（Nice!Nano 兼容 UF2 Bootloader）
- SoC: Nordic nRF52840
- Bootloader volume observed on Windows: `NICENANO`
- Verified board target under NCS v3.4.0: `promicro_nrf52840/nrf52840/uf2`
- Packet capture: separate nRF52840 BLE Packet Sniffer
- SWD: 暂不作为首选烧录方式；先使用 UF2

进入 UF2 Bootloader 的已验证方式：快速两次 `RST -> GND`，Windows 会出现 `NICENANO` U 盘。生成的 `build/zephyr/zephyr.uf2` 可直接复制到该盘进行烧录。

硬件与官方资料入口统一记录在 [`docs/references.md`](docs/references.md)。

## Project goal

按可验证的小阶段学习 nRF52840 和 BLE：

1. nRF Connect SDK / 工具链可用 ✅
2. GPIO / 板载 LED bring-up ✅
3. BLE advertising + Sniffer 空口验证 ✅
4. BLE connection + GATT read/write ✅
5. Notification ← **当前阶段**
6. Sniffer 深入分析连接、ATT/GATT、notification
7. 后续接入传感器并形成无线传感节点

当前任务：[`TASKS/004-notification.md`](TASKS/004-notification.md)。

协议说明：

- [`docs/gatt.md`](docs/gatt.md)
- [`docs/notification.md`](docs/notification.md)
- [`docs/sniffer.md`](docs/sniffer.md)

项目实机状态与本机环境记录：[`docs/project-status.md`](docs/project-status.md)。

## Current BLE interface

设备名：

```text
ble-t1
```

自定义 Primary Service：

```text
7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10
```

Task 003 control/value Characteristic：

```text
7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10
READ | WRITE
```

上电初始值：

```text
hello
```

Task 004 telemetry Characteristic：

```text
7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10
READ | NOTIFY
```

Task 004 notification payload：

```text
4-byte little-endian uint32 counter
01 00 00 00
02 00 00 00
03 00 00 00
...
```

发送周期约 1 秒；仅在 central 启用 CCCD notification 后发送。

Task 004 广播识别标记：

```text
FF FF 42 54 31 04
```

## Build

在已经激活 **nRF Connect SDK v3.4.0** 环境的终端中，从仓库根目录执行：

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

成功后重点产物：

```text
build/zephyr/zephyr.uf2
```

烧录优先采用 UF2：

1. USB 连接开发板。
2. 0.5 秒内两次 `RST -> GND`，进入 Bootloader。
3. 确认出现 `NICENANO` U 盘。
4. 将 `build/zephyr/zephyr.uf2` 拖入该盘。
5. 开发板自动复位运行。

## Local agent

本仓库按“云端先准备代码/任务，本地 Agent 完成真实工具链和硬件闭环”的方式推进。

本地 Agent 当前应首先读取：

- `AGENTS.md`
- `docs/project-status.md`
- `docs/gatt.md`
- `docs/notification.md`
- `docs/sniffer.md`
- `TASKS/004-notification.md`

然后使用已经验证的 Nordic nRF Connect SDK v3.4.0 环境进行实际构建、UF2 刷写和 BLE notification 实机验证。

云端没有实际 NCS 工具链和物理 USB/BLE 设备，因此 Task 004 当前只标记为“代码已准备”；只有本地编译和实物订阅验证完成后才能标记为完成。

不要修改或擦除 UF2 Bootloader；不要使用 SWD mass erase；Task 004 完成前不要提前接入真实传感器或扩展成复杂生产协议。
