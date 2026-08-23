# TODO — Hatsu firmware roadmap

## Verify on hardware

- Battery indicator states (priority order): `&bat` green bar > system-layer profile
  position (blue, pulsing while unpaired) > SoC < 20% solid red > per-layer colors
  (zmk,layer-colors chosen node) > USB charging pulse (any SoC while plugged in) >
  SoC > 80% green pulse (unplugged only; hidden while a colored layer is active) >
  idle sweep animation
- Profile LED orientation (profile 1 = leftmost RGB LED?)
- Deep sleep: halves sleep, wake on keypress, re-pair cleanly, indicator resumes
- Idle animation brightness (`IDLE_PEAK` in `src/battery_indicator.c`) — may want tuning

## Done

- [x] Per-layer RGB indication: top-level `layer_colors` node in the keymap
      (`compatible = "zmk,layer-colors"`, one uint32 per layer index, 0 = none) selected
      via `chosen { zmk,layer-colors = &layer_colors; }`; num=cyan, function=magenta,
      gaming=orange, mouse=yellow, system keeps BT-profile display. Replaces old
      upper/lower white patterns and their Kconfig entries. NOTE: an inline-subnode
      scheme (`indicator { color = ...; }` under each layer + dedicated binding) kept
      silently dropping props in CI even with the binding present — chosen-node table is
      the proven path; BUILD_ASSERTs in battery_indicator.c guard against drops
- [x] RGB battery indicator rework: state-based render loop with layer/BLE/battery/USB/
      activity event listeners; pink default removed; `&bat` bar now green; gentle idle
      sweep animation; layer 2 = white `[on on off off]`, layer 3 = white `[off off on on]`
- [x] Layers: `upper`(2) + `lower`(3) inserted (all `&trans`), 2 `reserved` layers added;
      `system` moved to index 4 (fixed missing thumb row: 48 → 52 bindings)
- [x] Deep sleep: `CONFIG_ZMK_SLEEP=y` + `wakeup-source` on both kscan nodes
- [x] Charge indication: green pulsing bar at SoC length while USB powered
      (`CONFIG_ZMK_BATTERY_INDICATOR_CHARGING`); no charger IC interface exists, so USB
      power is the only detectable charging source
- [x] Board port to upstream ZMK (Zephyr 4.1, HWMv2)
- [x] AW20216S + CW2015 out-of-tree drivers
- [x] ZMK Studio (physical layout with rotated thumbs)
- [x] `&bootl` disassembly-free UF2 flashing on both halves
- [x] Persistent split pairing (settings on both halves)
- [x] settings_reset images in CI
- [x] Per-half bootloaders (`AM_HATSU_L`/`AM_HATSU_R`) at ashleymcdonald/Adafruit_nRF52_Bootloader
