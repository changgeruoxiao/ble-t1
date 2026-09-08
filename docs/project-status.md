# 项目环境与进展

更新时间：2026-09-08

## 当前结论

Task 001 已完成：工程可以使用 Nordic nRF Connect SDK 构建目标板 UF2，UF2 已通过 `NICENANO` Bootloader 刷入实物开发板，板载 LED 呼吸灯工作正常。

Task 002 的 BLE advertising 目标已完成：本地构建、UF2 刷写、手机扫描和 Sniffer 命令行空口验证均有记录。应用板和 Sniffer 均被 Windows 识别，手机已看到设备名 `ble-t1`，`ble-sniffer` 已抓到包含 `BT1` 标记的广播数据。Task 002 固件后的 LED 再次视觉复核仍可补充；Wireshark GUI 验证是可选项，不影响命令行抓包结论。

Task 003 已完成本地构建和实物验证：`ble-t1` 可连接并发现自定义 GATT Service/Characteristic，初始值 `hello` 可读，写入 `task03` 后可读回，断开重连后 RAM 值保持，复位后恢复为 `hello`。最终固件刷写后用户确认呼吸灯仍正常工作。

Task 004 已由云端完成代码和任务准备，但**尚未声称本地构建或实物通过**：在原 Service 内新增独立 `READ | NOTIFY` telemetry characteristic，订阅后通过 delayed work 约每 1 秒主动推送一个 4 字节 little-endian 递增计数。下一步由本地 Agent 拉取后完成 NCS v3.4.0 构建、UF2 刷写、订阅/退订/重订阅和断线重连验证。

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

## Task 002：BLE advertising 已完成

云端先完成：

- `prj.conf` 开启 Bluetooth Peripheral 支持，设备名固定为 `ble-t1`。
- `src/main.c` 在保留呼吸灯的同时执行 `bt_enable()` 并启动 legacy connectable/scannable advertising。
- 广播/扫描响应中提供 `ble-t1` 设备名。
- Task 002 广播实验识别字节：`FF FF 42 54 31 02`。
- 新增 [TASKS/002-ble-advertising.md](../TASKS/002-ble-advertising.md)。
- 新增 [docs/sniffer.md](sniffer.md)。

本地 Agent 使用 NCS v3.4.0 构建时未修改上述 BLE 代码：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

构建退出码为 `0`。生成的 UF2 为 `C:\Users\27417\Desktop\prepre\ble-t1\build\zephyr\zephyr.uf2`，大小为 `267,776` 字节。该文件已复制到 `D:\NICENANO\ble-t1-task02.uf2`，刷写后 Bootloader 盘自动断开。

Sniffer 命令行验证结果：

- `nrfutil ble-sniffer` 已安装，版本为 `0.21.0`。
- `COM15` 握手成功，Sniffer 固件版本为 `4.1.1`，持续收到 BLE 数据包。
- 捕获文件保存在本机临时目录：`C:\Temp\ble-t1-task02-20260908-211309.pcap`，未提交到仓库。
- PCAP 原始数据中检出完整连续字节 `FF FF 42 54 31 02` 共 1,739 次，确认空口已看到本项目广播。
- 用户已用手机蓝牙扫描确认设备名为 `ble-t1`。
- Wireshark GUI 尚未作为 Task 002 的必要验收项。

## Task 003：GATT read/write 已完成

云端已新增：

- 自定义 Primary Service UUID：`7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10`。
- 自定义 Value Characteristic UUID：`7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10`。
- Characteristic 支持 `READ | WRITE`，最大 20 字节 RAM 数据。
- 上电初始值：ASCII `hello`。
- 完整写入后可立即读回；本阶段拒绝 prepared write 和 offset write，保持 ATT 行为简单可观察。
- 注册连接/断开回调，并输出最小 `printk` 日志。
- 广播数据加入自定义 Service UUID。
- 广播实验标记升级为 `FF FF 42 54 31 03`。
- 保留 Task 001 的 PWM 呼吸灯。
- 新增 [docs/gatt.md](gatt.md)。
- 新增 [TASKS/003-gatt-read-write.md](../TASKS/003-gatt-read-write.md)。

本地构建与刷写结果：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

- 使用 NCS v3.4.0 构建，最终构建退出码为 `0`，Task 003 的 GATT 代码仅做了一处必要功能修复：将断开后的广播重启从 `disconnected` 回调移到连接对象已回收的 `.recycled` 回调。
- 构建过程中曾因当前终端将 `C:\orelse\develop\tools\Lib` 混入 NCS Python 路径而触发 `ctypes` 错配；通过当前构建进程设置 `PYTHONHOME=C:\ncs\toolchains\dcbdc366a1\opt\bin` 解决，未修改 NCS/Zephyr 文件。
- 生成的 UF2 为 `C:\Users\27417\Desktop\prepre\ble-t1\build\zephyr\zephyr.uf2`，大小为 `269,312` 字节。
- 最终 UF2 已通过 `NICENANO` 刷写，刷写后 Bootloader 盘自动断开；用户确认最终固件的呼吸灯正常。

自动化 GATT 验证结果（Python `bleak 3.0.2`）：

- 扫描发现并连接 `ble-t1`；本次随机地址不作为永久设备标识。
- 发现 Primary Service：`7c7c0001-6e6f-4f72-9c5c-7a1b3d0e2f10`。
- 发现 Characteristic：`7c7c0002-6e6f-4f72-9c5c-7a1b3d0e2f10`，属性为 `Read`、`Write`。
- 复位后首次读取：`68 65 6c 6c 6f`（ASCII `hello`）。
- 写入：`74 61 73 6B 30 33`（ASCII `task03`）；立即读回一致。
- 断开后重新发现并连接成功，读取仍为 `task03`，确认 RAM 值跨断开/重连保持。
- 再次通过 UF2 刷写触发复位后，重新连接读取为 `hello`，确认复位恢复初始值。
- Task 003 未增加 notification；Sniffer 连接/ATT 抓包为可选项，本次未执行，不影响 GATT 功能验收。

## Task 004：Notification 已准备，待本地 Agent 验证

云端已完成以下代码设计：

- 保留 Task 003 `...0002` 的 `READ | WRITE` 行为，不把控制数据与推送数据混在一个 Characteristic。
- 新增 telemetry Characteristic UUID：`7c7c0003-6e6f-4f72-9c5c-7a1b3d0e2f10`。
- 新 Characteristic 属性为 `READ | NOTIFY`，并附加 CCCD。
- 订阅后将计数器清零，并立即调度第一个 notification；随后约每 1 秒推送一次。
- payload 固定为 4 字节 little-endian `uint32` 递增计数：`1, 2, 3, ...`。
- 退订或断开连接后取消 delayed work；再次订阅时重新从 `1` 开始。
- 使用 Zephyr delayed work，避免阻塞现有 PWM 呼吸灯循环。
- 广播实验标记升级为 `FF FF 42 54 31 04`。
- 新增 [docs/notification.md](notification.md)。
- 新增 [TASKS/004-notification.md](../TASKS/004-notification.md)。

云端**未声称上述 Task 004 代码已在 NCS v3.4.0 编译通过或实机通过**。本地 Agent 应按 Task 004 文档完成：

1. `git pull`；
2. 在已经验证的 NCS v3.4.0 环境执行构建；
3. 如有编译/API 兼容问题，只做最小必要修复并记录；
4. 通过 `NICENANO` 刷入 UF2，并确认呼吸灯无回归；
5. 用 `bleak` 或手机发现 `...0003 [read, notify]`；
6. 订阅并至少收集 5 个递增通知；
7. 验证退订停止、重订阅从 `1` 重新开始；
8. 验证断开/重连后仍能重新订阅；
9. 更新本页并推送完成提交。

Task 004 完成前，不接入真实传感器，也不把 4 字节计数器提前设计成复杂生产协议。

## 后续计划

1. Task 004：完成 Notification 本地构建和实机闭环。
2. Task 005：扩展 Sniffer 分析到连接事件、CCCD、ATT/GATT 和 notification。
3. 后续接入传感器，形成无线传感节点。

继续保持边界：不改变 UF2 Bootloader，不使用 SWD mass erase，也不引入 Arduino、PlatformIO 或 legacy nRF5 SDK，除非后续任务明确要求。
