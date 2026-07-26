# zmk-config-hatsu

ZMK firmware config for the Angry Miao Hatsu split keyboard, targeting **current upstream ZMK** with ZMK Studio support.

## Credits

This project builds on [MiaoBreak](https://github.com/hlord2000/MiaoBreak) by [hlord2000](https://github.com/hlord2000) (forked at [ashleymcdonald/MiaoBreak](https://github.com/ashleymcdonald/MiaoBreak)), who did the original reverse engineering, board definition, LED driver, and bootloader work that got ZMK running on the Hatsu. The `bootloader/` directory contains his UF2 bootloader build and flashing procedure.

## Status

- [ ] Port board definition (`boards/arm/angry_miao_hatsu`) to current ZMK/Zephyr
- [ ] Port AW20216S LED driver as out-of-tree module (not in upstream Zephyr)
- [ ] Port CW2015 fuel gauge driver (not in upstream Zephyr)
- [ ] Port RGB battery indicator behavior (keep + enhance)
- [ ] Enable ZMK Studio
- [ ] GitHub Actions build producing left/right UF2s

## Layout

- `bootloader/` — one-time UF2 bootloader flashing (requires hardware debugger, see its README)
- `boards/` — board definition (port in progress)
- `config/` — keymap and Kconfig defaults
- `build.yaml` / `.github/workflows/build.yml` — GitHub Actions firmware build

## Flashing firmware

Once the bootloader is installed (see `bootloader/`), no debugger is needed:

- **Left half**: hold SYSTEM (thumb key left of DEL) + `E`
- **Right half**: hold SYSTEM (thumb key right of RGUI) + `I`

Each half reboots into a USB drive named `AM_HATSU`; copy the matching `.uf2` onto it.
