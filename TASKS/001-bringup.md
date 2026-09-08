# Task 001 — nRF52840 board bring-up with nRF Connect SDK

## Objective

Create the minimum **Nordic nRF Connect SDK (NCS)** application for this repository and verify that the installed NCS can build it for the physical ProMicro / SuperMini nRF52840 board.

Candidate board target:

```text
promicro_nrf52840/nrf52840/uf2
```

The local Agent must verify that this target actually exists in the installed NCS before using it.

The first hardware behavior should be a simple board LED blink if the board definition exposes a usable `led0` alias. If the clone board LED mapping differs, determine the actual mapping without modifying SDK sources.

## SDK requirement

Use **Nordic nRF Connect SDK**, not a separately installed upstream Zephyr environment.

Zephyr APIs, Devicetree, Kconfig, west and CMake are expected because they are part of the NCS development model.

Before implementation, identify and report:

- nRF Connect SDK version;
- active toolchain version/location where practical;
- `west --version`;
- whether `promicro_nrf52840/nrf52840/uf2` is available in that NCS installation.

If only standalone upstream Zephyr is present, do not silently proceed with it. Report that NCS is missing and propose the minimal installation/setup step.

## Constraints

- Keep the current Nice!Nano-compatible UF2 bootloader intact.
- Do not use SWD mass erase.
- Do not modify nRF Connect SDK / Zephyr source files outside this repository.
- Do not start BLE yet.
- Do not add unrelated libraries or framework layers.
- Do not switch to legacy nRF5 SDK, Arduino or PlatformIO.

## Required implementation

Create the minimum application files:

```text
CMakeLists.txt
prj.conf
src/main.c
docs/board-notes.md
.gitignore
```

### `src/main.c`

Implement a minimal blink loop using Zephyr GPIO APIs and the board's Devicetree LED alias when available.

Requirements:

- use Devicetree rather than hard-coding a GPIO number as the first choice;
- fail clearly at compile time or initialization time if the LED alias is unavailable;
- toggle about every 500 ms;
- keep code intentionally small and readable.

### `prj.conf`

Enable only what is needed for the initial GPIO bring-up. Avoid enabling Bluetooth in this task.

### `docs/board-notes.md`

Record:

- board model: ProMicro / SuperMini nRF52840 clone;
- SoC: nRF52840;
- bootloader volume observed by user: `NICENANO`;
- NCS version used;
- board target actually used;
- known/reset entry method: two quick `RST -> GND` resets;
- actual LED mapping found during implementation/validation;
- any mismatch between the NCS/Zephyr board definition and this physical clone.

Also read [`docs/references.md`](../docs/references.md) before making board-specific assumptions.

## Local environment inspection

Use commands appropriate for the installed Nordic environment. At minimum inspect:

```bash
west --version
west boards | grep -i promicro
```

or the PowerShell equivalent on Windows.

Also determine the nRF Connect SDK version from the active SDK installation/workspace rather than merely reporting the Zephyr version.

If `promicro_nrf52840/nrf52840/uf2` does not exist in the installed NCS, do not silently switch board targets. Report the mismatch and identify the least invasive compatible solution, such as a repository-local board definition/overlay when appropriate.

## Build

After the target is confirmed, from repository root:

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always
```

Acceptance for the build stage:

- build exits successfully;
- `build/zephyr/zephyr.uf2` exists;
- generated files are not committed.

## Hardware validation

After a successful local build, tell the user to:

1. connect board by USB;
2. enter UF2 bootloader with two quick resets;
3. confirm `NICENANO` appears;
4. copy `build/zephyr/zephyr.uf2` to it;
5. observe the expected LED behavior.

Do not mark hardware validation complete until the user reports the physical result.

## Deliverable report

When done, summarize:

- nRF Connect SDK version detected;
- toolchain/version information detected;
- board target verification result;
- files created/changed;
- exact build command;
- build result;
- exact UF2 path;
- any board-specific issue discovered;
- the one next action the user should perform on hardware.

Do not proceed to BLE advertising until Task 001 has been physically validated.
