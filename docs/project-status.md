# 项目环境与进展

更新时间：2026-09-08

## 当前结论

Task 001 已完成：工程可以使用 Nordic nRF Connect SDK 构建目标板 UF2，UF2 已通过 `NICENANO` Bootloader 刷入实物开发板，板载 LED 呼吸灯工作正常。

当前尚未开始 BLE advertising；在进入 BLE 之前，保留本页作为环境和硬件 bring-up 基线。

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

## 当前实现与验证

- [CMakeLists.txt](../CMakeLists.txt)：最小 Zephyr 应用定义。
- [prj.conf](../prj.conf)：仅启用 PWM 和启动错误信息所需的 `printk`，没有启用 Bluetooth。
- [src/main.c](../src/main.c)：使用 Devicetree 的 `pwm-led0`，亮度从 0% 增加到 100% 再降低，形成呼吸灯。
- [docs/board-notes.md](board-notes.md)：记录板级资料和实物观察。

最终构建命令：

```text
west build -b promicro_nrf52840/nrf52840/uf2 . -p always --no-sysbuild
```

NCS v3.4.0 默认构建会启用 sysbuild，导致应用 UF2 位于嵌套目录；`--no-sysbuild` 用于保持任务文档要求的输出路径：

```text
build/zephyr/zephyr.uf2
```

当前呼吸灯版本的 UF2 大小为 98,816 字节。构建成功，实物刷写后 `NICENANO` 盘自动断开，LED 呼吸效果已确认。

## 后续计划

按小阶段推进：

1. Task 002：加入最小 BLE advertising，先只广播固定设备名和基本 GAP 信息。
2. Task 003：建立 BLE 连接并添加最小 GATT characteristic。
3. Task 004：增加 notification，并用手机或上位机验证数据流。
4. Task 005：使用 nRF Sniffer 和 Wireshark 观察 advertising、连接和 notification 空口过程。
5. 后续再接入传感器，形成无线传感节点。

进入 Task 002 前，不改变 UF2 Bootloader，不使用 SWD mass erase，也不引入 Arduino、PlatformIO 或 legacy nRF5 SDK。
