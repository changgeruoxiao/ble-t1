# Task 001 — nRF52840 board bring-up

## Objective

Create the minimum Zephyr application for this repository and verify that it can be built for:

```text
promicro_nrf52840/nrf52840/uf2
```

The first hardware behavior should be a simple board LED blink if the board definition exposes a usable `led0` alias. If the clone board LED mapping differs, determine the actual mapping without modifying the SDK itself.

## Constraints

- Keep the current Nice!Nano-compatible UF2 bootloader intact.
- Do not use SWD mass erase.
- Do not modify Zephyr / nRF Connect SDK source files outside this repository.
- Do not start BLE yet.
- Do not add unrelated libraries or framework layers.

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

Implement a minimal blink loop using Zephyr GPIO APIs and the board's devicetree LED alias when available.

Requirements:

- use devicetree rather than hard-coding a GPIO number as the first choice;
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
- preferred board target;
- known/reset entry method: two quick `RST -> GND` resets;
- actual LED mapping found during implementation/validation;
- any mismatch between upstream board definition and this physical clone.

## Local environment inspection

Before implementing, inspect:

```bash
west --version
west boards | grep -i promicro
```

or equivalent commands supported by the installed SDK.

Also print/record the Zephyr or nRF Connect SDK version being used.

If `promicro_nrf52840/nrf52840/uf2` does not exist in the installed SDK, do not silently switch board targets. Report the mismatch and identify the least invasive compatible solution.

## Build

From repository root:

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

- SDK/version detected;
- files created/changed;
- exact build command;
- build result;
- exact UF2 path;
- any board-specific issue discovered;
- the one next action the user should perform on hardware.

Do not proceed to BLE advertising until Task 001 has been physically validated.
