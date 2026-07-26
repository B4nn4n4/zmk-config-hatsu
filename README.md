# zmk-config-hatsu

ZMK firmware config for the Angry Miao Hatsu split keyboard, targeting **current upstream ZMK** with ZMK Studio support.

## Credits

This project builds on [MiaoBreak](https://github.com/hlord2000/MiaoBreak) by [hlord2000](https://github.com/hlord2000) (forked at [ashleymcdonald/MiaoBreak](https://github.com/ashleymcdonald/MiaoBreak)), who did the original reverse engineering, board definition, LED driver, and bootloader work that got ZMK running on the Hatsu. The `bootloader/` directory contains his UF2 bootloader build and flashing procedure.

## Status

- [ ] Port board definition (`boards/arm/angry_miao_hatsu`) to current ZMK/Zephyr
- [ ] Port AW20216S LED driver as out-of-tree module (not in upstream Zephyr)
- [ ] Port CW2015 fuel gauge driver (not in upstream Zephyr)
- [ ] Port RGB battery indicator behavior (keep + enhance)
- [ ] Enable ZMK Studio (needs `zmk,physical-layout` with `keys` + drop `zmk,matrix-transform` chosen)
- [x] GitHub Actions build producing left/right UF2s (+ settings_reset images)

## Known issues

- **Right half never enters UF2 mode** (`&bootloader` key + boot-magic-key both reboot to app;
  GPREGRET magic never lands). Left half works. Workaround: flash via J-Link
  (`openocd ... program <bin> 0x1000 verify reset exit`), or use ZMK Studio for keymap changes.
- Root cause unfound: bootloader, MBR, and UICR NRFFW verified byte-identical to reference on
  the right half; identical app code works on the left.

## Layout

- `bootloader/` — one-time UF2 bootloader flashing (requires hardware debugger, see its README)
- `boards/` — board definition (port in progress)
- `config/` — keymap and Kconfig defaults
- `build.yaml` / `.github/workflows/build.yml` — GitHub Actions firmware build

## Flashing firmware

Once the bootloader is installed (see `bootloader/`), no debugger is needed:

- **Left half**: hold SYSTEM (thumb key left of DEL) + `E`
- **Right half**: SYSTEM + `I` is currently broken (see Known issues) — use J-Link or
  the settings_reset/normal uf2 flow via debugger.

The left half reboots into a USB drive named `AM_HATSU`; copy the matching `.uf2` onto it.

If halves refuse to pair: flash the `settings_reset-*.uf2` to **both** halves, then the
normal firmware again — wipes stale split/BT bonds.
