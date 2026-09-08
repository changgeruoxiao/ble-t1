# Board notes

## Current bring-up

- Board model: ProMicro / SuperMini nRF52840 clone.
- SoC: Nordic nRF52840.
- Bootloader: Nice!Nano-compatible UF2 bootloader.
- Bootloader volume observed by the user: `NICENANO`.
- UF2 entry method: two quick `RST -> GND` resets, approximately within 0.5 seconds.
- NCS version: `v3.4.0`.
- Board target: `promicro_nrf52840/nrf52840/uf2`.

## LED mapping

The NCS board definition maps both the `led0` GPIO alias and the `pwm-led0` alias to `GPIO0.15` (`P0.15`).
The PWM LED has a 20 ms period and `PWM_POLARITY_INVERTED`; the breathing-light application uses this Devicetree mapping.
After flashing the UF2, the clone's LED was observed breathing correctly, confirming the `P0.15` mapping and PWM polarity.

## Clone-board differences

No mismatch with the NCS board definition was found during this validation.
