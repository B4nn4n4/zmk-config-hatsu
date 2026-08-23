# AGENTS.md — zmk-config-hatsu

ZMK firmware for the Angry Miao Hatsu split keyboard (nRF52840, Adafruit UF2 bootloader).
Targets **upstream ZMK main** (Zephyr 4.1). GitHub Actions builds all firmware; there is
no local toolchain here.

## Build & release flow

- Push to `main` → `.github/workflows/build.yml` → artifacts in `out/firmware/` after
  `gh run download <id> -D out`
- `build.yaml` builds: `hatsu_left//zmk` (with `studio-rpc-usb-uart` snippet),
  `hatsu_right//zmk`, and `settings_reset` images for both halves
- CI iteration is the norm: expect a few push/fail/fix cycles; read failures with
  `gh run view <id> --log-failed | grep -i error`

## Flashing

- Normal path (no debugger): SYSTEM + `E` (left) / SYSTEM + `I` (right) → `AM_HATSU`
  drive → copy uf2. This is the custom `&bootl` behavior (writes `0x57` to
  `NRF_POWER->GPREGRET` then `sys_reboot()`).
- J-Link recovery: `openocd -f interface/jlink.cfg -c "transport select swd; source
  [find target/nrf52.cfg]; init; reset halt; program <firmware>.bin 0x1000 verify reset
  exit"` — **never mass-erase** (kills the UF2 bootloader at 0xf4000). Convert uf2→bin
  with the python snippet used in git history (UF2 base 0x1000).
- Settings partition at `0xd4000` size `0x20000` holds bonds; erase it via J-Link or
  flash `settings_reset-*.uf2` to both halves when pairing is stuck.

## Repo layout

- `boards/angry_miao/hatsu_{left,right}/` — HWMv2 board defs. dts/defconfig files must be
  named `<board>_nrf52840_zmk.{dts,overlay,defconfig}` (qualifier-suffixed)
- `dts/hatsu.dtsi` — shared: kscan transform, flash partitions, usbd, boot-magic keys
- `dts/hatsu_physical_layout.dtsi` — 52-key layout (centi-keyunits, centi-degrees)
- `drivers/led/aw20216s.c` — LED driver; children ordered = LED numbers, `index` prop =
  PWM register. Callers pass child **ordinal** (`DT_NODE_CHILD_IDX`)
- `drivers/sensor/cw2015/` — fuel gauge (read-only; no charge control exists)
- `src/` — backlight-map (custom `zmk_backlight_*` impl), battery_indicator, behaviors.
  Battery indicator doubles as the layer RGB display: layers opt in with an
  `indicator { color = <0xRRGGBB>; };` subnode in their keymap layer definition — plain
  properties on layer nodes are silently dropped by the `zmk,keymap` child-binding
  whitelist, so the color must live in a subnode. Layer events only fire on the
  central, so only the left half shows them
- `config/hatsu_{left,right}.keymap` — NOT hardlinked anymore: `hatsu_left.keymap`
  (canonical, actively edited) and `hatsu_right.keymap` have drifted apart. Custom
  behavior nodes live in the keymap overlay (ZMK compiles keymaps against a stub dts —
  board dts nodes are NOT visible there)
- `config/west.yml` — pins `zmkfirmware/zmk@main`; `config/hatsu_left.conf` enables Studio

## Hard-won gotchas (do not rediscover)

- **Custom behavior node names must be ≤ 8 characters** or they silently fail to invoke
  on split peripherals (nodes are named `bat`/`bootl` for this reason)
- Zephyr ≥ 4 removed the nRF `sys_reboot(type)` → GPREGRET write; the Zephyr
  retention/bootmode path does not work reliably on the right half. Direct GPREGRET
  write + system reset works on both
- Left and right half row-gpio orders differ (right is NOT a copy of the left; getting
  this wrong mirrors all keys) — keep in sync with the original MiaoBreak dts
- `zmk,physical-layout` (with `keys`) replaces the `zmk,matrix-transform` chosen node
- Boards need `select ZMK_BOARD_COMPAT if BOARD_<NAME>_NRF52840_ZMK` and the
  `uf2_boot_mode.dtsi` include; dts files need `/dts-v1/;`
- phandle-array `led-channels` requires `#led-channel-cells = <0>;` in each LED child
- `CONFIG_SETTINGS` must be on BOTH halves or split pairing is lost on reset
- Right half is the BLE peripheral: no USB HID output, no keymap of its own (central
  resolves; behaviors route by locality)
- Keymap layer children inherit the `zmk,keymap` child-binding **recursively**: custom
  properties under layers are silently dropped unless the node has its own compatible +
  binding. Data subnodes need e.g. `compatible = "hatsu,layer-indicator"` +
  `dts/bindings/hatsu,layer-indicator.yaml`; BUILD_ASSERT tripwires in
  battery_indicator.c catch drops at compile time on hatsu_left

## Hardware reference

- Per half: AW20216S (SPI) driving 26 white backlight LEDs + 4 RGB LEDs (battery bar),
  CW2015 fuel gauge (I2C), no reset button, no charge-control interface found
- SWD pads: GND/IO/CLK (+3V on left), photo in `bootloader/pinout.png`
- Backlight: `&bl` on SYSTEM layer; battery bar: `&bat`; profile/pair LEDs: see TODO.md
