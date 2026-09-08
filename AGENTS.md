# AGENTS.md

This repository is intended to be implemented and iterated by a local coding agent.

## Scope

Target hardware:

- Nordic nRF52840
- ProMicro / SuperMini nRF52840 board
- Nice!Nano-compatible UF2 bootloader
- Candidate Zephyr board target inside NCS: `promicro_nrf52840/nrf52840/uf2`

Primary SDK/framework:

- **Nordic nRF Connect SDK (NCS)** is the required primary development environment.
- The application uses Zephyr APIs, Devicetree, Kconfig, west and CMake because Zephyr is part of NCS.
- Do not treat this repository as a standalone upstream-Zephyr project unless explicitly requested.
- Use C unless a specific task requires another language.

Reference material:

- Read `docs/references.md` before making board-specific assumptions.

## Rules

1. Do not overwrite, erase, replace, or recover the UF2 bootloader unless explicitly requested.
2. Do not use SWD mass erase as part of normal bring-up.
3. Prefer UF2 output and manual UF2 flashing for initial hardware validation.
4. Keep each learning stage small and independently testable.
5. Before editing code, inspect the installed **nRF Connect SDK version and toolchain** and verify the target board exists in that NCS installation.
6. Do not silently substitute standalone upstream Zephyr for NCS.
7. If the board definition differs from the actual clone hardware, prefer a repository-local devicetree overlay/configuration instead of modifying NCS/Zephyr SDK sources.
8. Do not commit generated `build/` output.
9. Treat successful compilation and successful hardware observation as separate acceptance criteria.
10. Record clone-board-specific pin differences and physical observations in `docs/board-notes.md`.
11. Avoid introducing Arduino, PlatformIO, legacy nRF5 SDK / SoftDevice SDK, or other frameworks unless explicitly requested.
12. If `promicro_nrf52840/nrf52840/uf2` is unavailable in the installed NCS, stop and report the exact mismatch before selecting another target.

## Standard workflow

For every task:

1. Read the relevant file under `TASKS/`.
2. Read `docs/references.md` when hardware or SDK details are involved.
3. Inspect current repository state.
4. Inspect local nRF Connect SDK / toolchain availability and version.
5. Verify the requested board target in the installed NCS.
6. Implement the smallest change that satisfies the task.
7. Build with the documented board target when it is confirmed available.
8. Fix all build errors caused by the change.
9. Report the exact generated UF2 path.
10. Do not claim hardware success until the user has flashed it and observed the expected behavior.

Preferred build command after target verification:

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
│   ├── board-notes.md
│   └── references.md
└── TASKS/
    └── 001-bringup.md
```

Do not add unnecessary abstraction during the first stages.
