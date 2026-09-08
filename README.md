# ble-t1

nRF52840 / Bluetooth LE 学习与实验工程。

## Development stack

本项目的主开发栈明确为 **Nordic nRF Connect SDK (NCS)**。

关系如下：

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
- Candidate board target: `promicro_nrf52840/nrf52840/uf2`
- Packet capture: nRF52840 BLE Packet Sniffer + Wireshark（后续阶段）
- SWD: 暂不作为首选烧录方式；先使用 UF2

> 注意：`promicro_nrf52840/nrf52840/uf2` 必须由本地 Agent 在实际安装的 nRF Connect SDK 中确认存在后再使用，不要因为文档中写了该名称就假定所有 NCS 版本都包含它。

进入 UF2 Bootloader 的已验证方式：快速两次 `RST -> GND`，Windows 会出现 `NICENANO` U 盘。生成的 `build/zephyr/zephyr.uf2` 可直接复制到该盘进行烧录。

硬件与官方资料入口统一记录在 [`docs/references.md`](docs/references.md)。

## Project goal

按可验证的小阶段学习 nRF52840 和 BLE：

1. nRF Connect SDK / 工具链可用
2. GPIO / 板载 LED bring-up
3. BLE advertising
4. BLE connection + GATT characteristic
5. Notification
6. nRF Sniffer + Wireshark 空口分析
7. 后续接入传感器并形成无线传感节点

当前只执行阶段 1~2。具体任务见 [`TASKS/001-bringup.md`](TASKS/001-bringup.md)。

## Build

在已经激活 **nRF Connect SDK** 环境的终端中，从仓库根目录执行：

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always
```

前提是本地安装的 NCS 已确认支持该 board target。

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

也可以在 UF2 runner 能正确发现挂载点时尝试：

```bash
west flash
```

## Local agent

本仓库为本地 Agent / Codex 驱动开发准备。让 Agent 首先读取：

- `AGENTS.md`
- `TASKS/001-bringup.md`
- `docs/references.md`

然后让它在本地检查 **Nordic nRF Connect SDK / toolchain**、目标板支持情况，再实现、构建和修复。

初始阶段不要改 Bootloader，也不要使用 SWD mass erase。
