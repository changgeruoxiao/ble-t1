# 项目环境与进展

更新时间：2026-09-08

## 当前结论

Task 001 已完成：工程可以使用 Nordic nRF Connect SDK 构建目标板 UF2，UF2 已通过 `NICENANO` Bootloader 刷入实物开发板，板载 LED 呼吸灯工作正常。

Task 002 的 BLE advertising 目标已完成：本地构建、UF2 刷写、手机扫描和 Sniffer 命令行空口验证均有记录。应用板和 Sniffer 均被 Windows 识别，手机已看到设备名 `ble-t1`，`ble-sniffer` 已抓到包含 `BT1` 标记的广播数据。按照任务验收要求，刷入 Task 002 固件后的 LED 再次视觉复核仍待补充；Wireshark GUI 验证是可选项，不影响命令行抓包结论。

## 本机开发环境

| 项目 | 当前值 |
| --- | --- |
| nRF Connect SDK | `v3.4.0` |
| NCS 路径 | `C:\orelse\develop\tools\ncs\v3.4.0` |
| Zephyr | `4.4.0` |
| Toolchain 路径 | `C:\ncs\toolchains\dcbdc366a1` |
| west | `1.5.0` |
| Python | `3.12.4` |
| 自动化 Python | `3.12.9`（`C:\orelse\develop\tools\python.exe`） |
| Python BLE 库 | `bleak 3.0.2` |
| NCS CMake | `4.2.1` |
| Ninja | `1.13.2` |
| Zephyr SDK | `1.0.1` |
| ARM 编译器 | GNU Arm Embedded `14.3.0` |
| NCS nrfutil | `8.1.1` |
| Sniffer host nrfutil | `8.2.1` |
| ble-sniffer | `0.21.0` |
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
- 独立抓包硬件：卖家标注 `nRF52840 Packet Sniffer` 的 USB dongle；Task 002 已确认 COM 口和固件兼容性。
- Task 002 验证时，应用板 CDC ACM 串口为 `COM14`（VID `2FE3` / PID `0004`）。
- Task 002 验证时，Sniffer 串口为 `COM15`（VID `1915` / PID `522A`），Windows 描述为 `nRF Sniffer for Bluetooth LE`，固件版本为 `4.1.1`。

## Task 001：已验证基线

- [CMakeLists.txt](../CMakeLists.txt)：最小 Zephyr/NCS 应用定义。
- [docs/board-notes.md](board-notes.md)：记录板级资料和实物观察。
- 构建命令：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

- 产物：`build/zephyr/zephyr.uf2`。
- 呼吸灯版本已完成实物刷写并验证。

## Task 002：BLE advertising 已完成，LED 回归观察待补

云端已先完成以下改动：

- `prj.conf` 开启 Bluetooth Peripheral 支持，设备名固定为 `ble-t1`。
- `src/main.c` 在保留呼吸灯的同时执行 `bt_enable()` 并启动 legacy connectable/scannable advertising。
- 广播/扫描响应中提供 `ble-t1` 设备名。
- 广播数据加入实验识别字节：`FF FF 42 54 31 02`，其中包含 ASCII `BT1` 和实验版本 `02`。
- 新增 [TASKS/002-ble-advertising.md](../TASKS/002-ble-advertising.md)。
- 新增 [docs/sniffer.md](sniffer.md)，记录 nRF Util + Wireshark 抓包流程和第三方 sniffer 固件兼容性检查边界。

本地 Agent 使用 NCS v3.4.0 构建时未修改这部分代码，构建命令为：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

构建退出码为 `0`。生成的 UF2 为 `C:\Users\27417\Desktop\prepre\ble-t1\build\zephyr\zephyr.uf2`，大小为 `267,776` 字节。该文件已复制到 `D:\NICENANO\ble-t1-task02.uf2`，刷写后 Bootloader 盘自动断开。

Sniffer 命令行验证结果：

- `nrfutil ble-sniffer` 已安装，版本为 `0.21.0`。
- `COM15` 握手成功，Sniffer 固件版本为 `4.1.1`，持续收到 BLE 数据包。
- 捕获文件保存在本机临时目录：`C:\Temp\ble-t1-task02-20260908-211309.pcap`，未提交到仓库。
- PCAP 原始数据中检出完整连续字节 `FF FF 42 54 31 02` 共 1,739 次，确认空口已看到本项目广播。
- Wireshark 未安装在默认路径，`nrfutil ble-sniffer bootstrap` 因找不到 `C:\Program Files\Wireshark\Wireshark.exe` 未完成；这不影响上述命令行捕获。
- 首次命令行捕获未独立解析出完整名称 `ble-t1`，因此不把 Sniffer 的设备名字段当作手机/PC 扫描器验证结果。
- 用户已用手机蓝牙扫描确认设备名为 `ble-t1`。
- 为后续自动化准备的 `bleak 3.0.2` 已安装到本机 Python 3.12.9；尝试扫描时发现 Windows 主机蓝牙适配器处于关闭状态，因此未将该次扫描作为 Task 002 验收证据。

### 剩余观察项

1. 观察本次 Task 002 固件刷写后 LED 是否仍然呼吸，并补充记录。
2. 如需 Wireshark 图形界面证据，安装 Wireshark 后重新执行 `nrfutil ble-sniffer bootstrap`；该项为可选，不影响当前命令行抓包结果。

命令行已抓到带项目标记的 advertising 数据；完成 LED 回归观察后，再进入 Task 003：连接 + 最小 GATT characteristic。

## 后续计划

1. Task 003：建立 BLE 连接并添加最小 GATT characteristic。
2. Task 004：增加 notification，并用手机或上位机验证数据流。
3. Task 005：扩展 Sniffer 分析到连接事件、ATT/GATT 和 notification。
4. 后续接入传感器，形成无线传感节点。

继续保持边界：不改变 UF2 Bootloader，不使用 SWD mass erase，也不引入 Arduino、PlatformIO 或 legacy nRF5 SDK，除非后续任务明确要求。
