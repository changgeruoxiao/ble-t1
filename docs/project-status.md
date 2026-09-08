# 项目环境与进展

更新时间：2026-09-08

## 当前结论

Task 001 已完成：工程可以使用 Nordic nRF Connect SDK 构建目标板 UF2，UF2 已通过 `NICENANO` Bootloader 刷入实物开发板，板载 LED 呼吸灯工作正常。

Task 002 已由云端先完成代码准备，但**尚未完成本地构建与实物验证**：仓库已加入最小 BLE advertising，设备名为 `ble-t1`，并新增 Packet Sniffer 使用说明。下一步由本地 Agent 拉取后完成 NCS v3.4.0 构建、UF2 刷写、手机扫描和 Wireshark 空口抓包。

## 本机开发环境

| 项目 | 当前值 |
| --- | --- |
| nRF Connect SDK | `v3.4.0` |
| NCS 路径 | `C:\orelse\develop\tools\ncs\v3.4.0` |
| Zephyr | `4.4.0` |
| Toolchain 路径 | `C:\ncs\toolchains\dcbdc366a1` |
| west | `1.5.0` |
| Python | `3.12.4` |
| NCS CMake | `4.2.1` |
| Ninja | `1.13.2` |
| Zephyr SDK | `1.0.1` |
| ARM 编译器 | GNU Arm Embedded `14.3.0` |
| nrfutil | `8.1.1` |
| VS Code 扩展 | nRF Connect for VS Code `2026.8.2347` |

`C:\orelse\develop\tools` 中另外存在通用 CMake、Ninja 和 ARM GCC，但本项目构建使用的是 NCS Toolchain Manager 提供的环境，不混用两套工具链。

VS Code 注册 SDK 时曾因未加载 west/toolchain 环境返回 `-4058`。在 NCS 工具链环境中执行 `west zephyr-export` 后，Zephyr CMake 包注册成功，项目可以正常构建。

## 当前硬件与板级信息

- 板型：ProMicro / SuperMini nRF52840 clone。
- SoC：Nordic nRF52840。
- Bootloader：Nice!Nano-compatible UF2。
- UF2 卷标：`NICENANO`。
- 进入 Bootloader：约 0.5 秒内快速两次 `RST -> GND`。
- 目标板：`promicro_nrf52840/nrf52840/uf2`。
- NCS 板级定义将 `led0` 和 `pwm-led0` 映射到 `P0.15`。
- `pwm-led0` 使用 20 ms 周期和反相极性；实物呼吸灯验证通过，未发现板级映射差异。
- 独立抓包硬件：卖家标注 `nRF52840 Packet Sniffer` 的 USB dongle；当前尚未在本机确认 COM 口和固件兼容性。

## Task 001：已验证基线

- [CMakeLists.txt](../CMakeLists.txt)：最小 Zephyr/NCS 应用定义。
- [docs/board-notes.md](board-notes.md)：记录板级资料和实物观察。
- 构建命令：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

- 产物：`build/zephyr/zephyr.uf2`。
- 呼吸灯版本已完成实物刷写并验证。

## Task 002：已准备，待本地 Agent 验证

云端已先完成以下改动：

- `prj.conf` 开启 Bluetooth Peripheral 支持，设备名固定为 `ble-t1`。
- `src/main.c` 在保留呼吸灯的同时执行 `bt_enable()` 并启动 legacy connectable/scannable advertising。
- 广播/扫描响应中提供 `ble-t1` 设备名。
- 广播数据加入实验识别字节：`FF FF 42 54 31 02`，其中包含 ASCII `BT1` 和实验版本 `02`。
- 新增 [TASKS/002-ble-advertising.md](../TASKS/002-ble-advertising.md)。
- 新增 [docs/sniffer.md](sniffer.md)，记录 nRF Util + Wireshark 抓包流程和第三方 sniffer 固件兼容性检查边界。

这些改动尚未在云端声称“编译通过”或“硬件通过”，因为本地 NCS 工具链和物理 USB 设备只在用户电脑上可用。

### 下一步

本地 Agent 应：

1. `git pull`；
2. 阅读 `AGENTS.md`、`TASKS/002-ble-advertising.md`、`docs/sniffer.md`；
3. 在已验证的 NCS v3.4.0 环境中构建；
4. 若构建失败，仅做必要兼容修复并记录原因；
5. 刷写 UF2，确认呼吸灯仍工作；
6. 用手机/PC 扫描确认 `ble-t1`；
7. 插入独立 Packet Sniffer，识别 COM 口并配置 nRF Util/Wireshark；
8. 抓到 `ble-t1` advertising/scan response 后更新本页。

Task 002 验证完成后，再进入 Task 003：连接 + 最小 GATT characteristic。

## 后续计划

1. Task 003：建立 BLE 连接并添加最小 GATT characteristic。
2. Task 004：增加 notification，并用手机或上位机验证数据流。
3. Task 005：扩展 Sniffer 分析到连接事件、ATT/GATT 和 notification。
4. 后续接入传感器，形成无线传感节点。

继续保持边界：不改变 UF2 Bootloader，不使用 SWD mass erase，也不引入 Arduino、PlatformIO 或 legacy nRF5 SDK，除非后续任务明确要求。
