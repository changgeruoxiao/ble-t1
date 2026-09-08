# ble-t1

nRF52840 / Bluetooth LE 学习与实验工程。

## Hardware

- Board: ProMicro / SuperMini nRF52840（Nice!Nano 兼容 UF2 Bootloader）
- SoC: Nordic nRF52840
- Bootloader volume: `NICENANO`
- Preferred Zephyr board target: `promicro_nrf52840/nrf52840/uf2`
- Packet capture: nRF52840 BLE Packet Sniffer + Wireshark（后续阶段）
- SWD: 暂不作为首选烧录方式；先使用 UF2

Zephyr 当前文档已提供 `promicro_nrf52840/nrf52840/uf2` target。进入 UF2 Bootloader 的方式是快速两次复位，随后将生成的 `build/zephyr/zephyr.uf2` 写入 `NICENANO` 盘。

## Project goal

按可验证的小阶段学习 nRF52840 和 BLE：

1. 工程/工具链可构建
2. GPIO / 板载 LED bring-up
3. BLE advertising
4. BLE connection + GATT characteristic
5. Notification
6. nRF Sniffer + Wireshark 空口分析
7. 后续接入传感器并形成无线传感节点

当前只执行阶段 1~2。具体任务见 [`TASKS/001-bringup.md`](TASKS/001-bringup.md)。

## Build

在已经激活 Zephyr 或 nRF Connect SDK 环境的终端中，从仓库根目录执行：

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always
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

也可以在 UF2 runner 能正确发现挂载点时尝试：

```bash
west flash
```

## Local agent

本仓库为本地 Agent / Codex 驱动开发准备。让 Agent 首先读取：

- `AGENTS.md`
- `TASKS/001-bringup.md`

然后让它在本地检查工具链、实现、构建和修复；不要一开始改 Bootloader，也不要使用 SWD mass erase。
