# Reference links

本文件用于记录 `ble-t1` 的板级、芯片和 SDK 资料入口，避免后续本地 Agent 因为搜索到不同版本或不同克隆板资料而混用。

## 1. 当前开发板资料

### ProMicro nRF52840 / WMNologo 板级资料

用户购买页面给出的资料地址：

- https://wiki.nologo.tech/product/otherboard/NRF52840.html

当前已经从实物和卖家说明确认的信息：

- 主控：Nordic nRF52840
- 板型：ProMicro / SuperMini 风格 nRF52840
- Bootloader：Nice!Nano-compatible UF2 bootloader
- Windows 进入 Bootloader 后观察到的卷标：`NICENANO`
- 进入 Bootloader：约 0.5 秒内快速两次 `RST -> GND`
- 卖家说明默认出厂程序为 Blink-All-IO（所有 IO 约 1 秒翻转）

> 对具体 GPIO、LED、电源控制、充电、电池和引脚映射，以该板级资料和实物验证为准。不要仅依据其他厂商的 nice!nano / SuperMini 克隆板资料进行假设。

### 卖家页面同时给出的相关资料

页面中还出现了一个支持 ZMK 的显示模块资料：

- https://wiki.nologo.tech/product/display/LCD_096-144x72/introduce.html

它不是当前 Task 001 必需硬件，仅作为后续可能的外设参考保留。

卖家页面还展示了 ProMicro nRF52840 的 3D 模型资料链接，但用户截图中 URL 未完整显示，因此这里不猜测或补写不完整地址；需要时重新从卖家页面获取完整链接。

## 2. Nordic 官方 nRF52840 资料

### nRF52840 产品页

- https://www.nordicsemi.com/Products/nRF52840

### nRF52840 Product Specification

- https://docs.nordicsemi.com/bundle/ps_nrf52840/page/keyfeatures_html5.html

这里是芯片级资料来源。需要确认 GPIO、外设、EasyDMA、PPI、时钟、电源、USB、Radio、SWD 或寄存器行为时，优先查 Product Specification，而不是板级 Wiki。

## 3. Nordic 官方 SDK

### nRF Connect SDK

- https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK
- https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/index.html
- https://github.com/nrfconnect/sdk-nrf

本项目主开发栈为 **Nordic nRF Connect SDK (NCS)**。

需要注意：

```text
nRF Connect SDK
├── Zephyr RTOS
├── Nordic nrfx / nrfxlib
├── Bluetooth and other Nordic components
├── MCUboot
└── west / CMake / Kconfig / Devicetree workflow
```

因此代码里使用 Zephyr API 或 `west build` 并不代表项目使用的是一个与 Nordic 无关的第三方 SDK；它们属于当前 NCS 的标准开发方式。

## 4. Legacy SDK

Nordic 旧版 nRF5 SDK：

- https://www.nordicsemi.com/Products/Development-software/nRF5-SDK

本项目不以 nRF5 SDK 作为默认开发环境。只有在明确做旧架构/SoftDevice 对比实验时再引入，避免与当前 NCS 工程混用。

## 5. 本项目的资料优先级

遇到冲突时按以下优先级判断：

1. 当前实物板上的实际验证结果
2. 当前板子的 WMNologo / 卖家板级资料
3. Nordic nRF52840 官方 Product Specification
4. 当前安装版本的 nRF Connect SDK 文档与源码
5. upstream Zephyr 对类似 ProMicro / SuperMini 板型的定义
6. 第三方博客、论坛或其他 clone 板资料

任何与当前克隆板相关的差异，都记录到 `docs/board-notes.md`，不要直接修改 NCS/Zephyr SDK 源码。
