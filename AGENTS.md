# AGENTS.md

This repository is intended to be implemented and iterated by a local coding agent.

## Scope

Target hardware:

- Nordic nRF52840
- ProMicro / SuperMini nRF52840 board
- Nice!Nano-compatible UF2 bootloader
- Zephyr board target: `promicro_nrf52840/nrf52840/uf2`

Primary framework:

- Zephyr / nRF Connect SDK compatible application structure
- C language unless a specific reason requires otherwise

## Rules

1. Do not overwrite, erase, replace, or recover the UF2 bootloader unless explicitly requested.
2. Do not use SWD mass erase as part of normal bring-up.
3. Prefer UF2 output and manual UF2 flashing for initial hardware validation.
4. Keep each learning stage small and independently testable.
5. Before editing code, inspect the local Zephyr/nRF Connect SDK version and verify the target exists.
6. If the board definition differs from the actual clone hardware, prefer a local devicetree overlay/configuration instead of modifying SDK sources.
7. Do not commit generated `build/` output.
8. Treat successful compilation and successful hardware observation as separate acceptance criteria.
9. Record any clone-board-specific pin differences in `docs/board-notes.md`.
10. Avoid introducing Arduino, PlatformIO, SoftDevice SDK, or other frameworks unless explicitly requested.

## Standard workflow

For every task:

1. Read the relevant file under `TASKS/`.
2. Inspect current repository state.
3. Inspect local toolchain / SDK availability.
4. Implement the smallest change that satisfies the task.
5. Build with the documented board target.
6. Fix all build errors caused by the change.
7. Report the exact generated UF2 path.
8. Do not claim hardware success until the user has flashed it and observed the expected behavior.

Preferred build command:

```bash
west build -b promicro_nrf52840/nrf52840/uf2 . -p always
```

## Expected application layout

For the initial bring-up, keep the repository simple:

```text
ble-t1/
├── AGENTS.md
├── CMakeLists.txt
├── prj.conf
├── README.md
├── src/
│   └── main.c
├── docs/
│   └── board-notes.md
└── TASKS/
    └── 001-bringup.md
```

Do not add unnecessary abstraction during the first stages.
