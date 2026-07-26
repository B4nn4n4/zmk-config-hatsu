# zmk-config-hatsu

ZMK firmware config for the Angry Miao Hatsu split keyboard, targeting **current upstream ZMK** (Zephyr 4.1) with **ZMK Studio** support.

## Credits

This project builds on [MiaoBreak](https://github.com/hlord2000/MiaoBreak) by [hlord2000](https://github.com/hlord2000) (forked at [ashleymcdonald/MiaoBreak](https://github.com/ashleymcdonald/MiaoBreak)), who did the original reverse engineering, board definition, AW20216S LED driver, and bootloader work that got ZMK running on the Hatsu. The `bootloader/` directory contains his UF2 bootloader build and flashing procedure.

## Status — everything works

- [x] Board definition ported to current ZMK/Zephyr (HWMv2, `hatsu_left`/`hatsu_right` + `zmk` variant)
- [x] AW20216S LED driver as out-of-tree module (per-key white backlight, `&bl`)
- [x] CW2015 fuel gauge driver (battery level over BLE/USB)
- [x] RGB battery indicator behavior (`&bat`), shows charge on the 4 RGB LEDs
- [x] ZMK Studio (left half, USB) with full physical layout incl. rotated thumb keys
- [x] GitHub Actions build producing left/right UF2s (+ settings_reset images)
- [x] Disassembly-free flashing on **both** halves (see below)
- [x] Split pairing that survives resets (settings storage enabled on both halves — the
      original MiaoBreak bug was settings only on the right)

## Flashing firmware (no debugger needed)

- **Left half**: hold SYSTEM (outermost bottom-row thumb key) + `E`
- **Right half**: hold SYSTEM + `I`

Each half reboots into a USB drive named `AM_HATSU`; copy the matching `.uf2` onto it.

> Implementation note: `&bootl` writes the Adafruit UF2 magic (`0x57`) directly to
> `GPREGRET` and resets. Zephyr ≥ 4 removed the nRF `sys_reboot(type)` GPREGRET write
> the old method relied on, and the Zephyr retention/bootmode path proved unreliable
> on the right half — the direct write works on both.
>
> Custom behavior **node names must be ≤ 8 chars** or they silently fail to route to
> split peripherals (that's why the nodes are literally named `bat`/`bootl`).

If halves refuse to pair: flash the `settings_reset-*.uf2` to **both** halves, then the
normal firmware again — wipes stale split/BT bonds.

## ZMK Studio

Left half ships with `CONFIG_ZMK_STUDIO` + the `studio-rpc-usb-uart` snippet. Open
https://zmk.studio in Chrome/Edge with the left half on USB. Studio keymap changes are
stored in flash, so later `.keymap` file changes need a "Restore Stock Settings" in Studio.

## Layout

- `bootloader/` — one-time UF2 bootloader flashing (requires hardware debugger, see its README).
  New per-half bootloader builds (`AM_HATSU_L` / `AM_HATSU_R` volume labels, latest upstream
  Adafruit bootloader) live at [ashleymcdonald/Adafruit_nRF52_Bootloader](https://github.com/ashleymcdonald/Adafruit_nRF52_Bootloader) — optional upgrade.
- `boards/` — board definitions (HWMv2)
- `dts/` — shared dtsi, physical layout, bindings
- `drivers/` — AW20216S (LED) and CW2015 (fuel gauge) out-of-tree drivers
- `src/` — backlight-map, battery indicator, `&bootl` bootloader behavior, boot-magic-key
- `config/` — keymap and Kconfig user options
- `build.yaml` / `.github/workflows/build.yml` — GitHub Actions firmware build
