# TODO — Hatsu firmware roadmap

## 1. RGB battery indicator rework (`src/battery_indicator.c`)

The 4 RGB LEDs per side get a proper state-based design. Priority order (highest first):

| State | Pattern |
|---|---|
| System layer active, BT profile selected | Profile position in blue: profile 1 = `[on off off off]`, 2 = `[off on off off]`, etc. (orientation to be verified on hardware) |
| Pairing mode (profile selected, not yet connected) | Pulse blue |
| SoC > 80% | Pulse the current color |
| SoC < 20% | Solid red |
| Battery check (`&bat`) | Bar in **green** (was blue) |
| Layer 2 active | `[white white off off]` |
| Layer 3 active | `[white white off off]` → `[off off white white]` |
| Idle | Gentle animation across the 4 LEDs (replaces solid pink `0xC70039`) |

Implementation notes:
- Needs event listeners for: layer state changed (`zmk_layer_state_changed`), BT profile
  selected/connected (`zmk_ble_active_profile_changed` / connection events), activity state.
- Battery colors live in the board defconfigs (`CONFIG_ZMK_BATTERY_INDICATOR_COLOR` → green `0x00FF00`).
- The default pink should go away entirely once idle animation exists.

## 2. Layers

- Current: `default(0)`, `function(1)`, `system(2)` — bindings use **named** refs (`&mo FUNCTION`, `&mo SYSTEM`), so inserting a layer is safe.
- Insert one new layer between `function` and `system` (all `&trans`), giving 4 layers total
  (default + function + 2 new + system). User renames/maps in ZMK Studio (will be named upper/lower).
- Add 2 `reserved` layers too so Studio can use them later.
- Layer indication LEDs (see table above) for layers 2 and 3.

## 3. Deep sleep

- `CONFIG_ZMK_SLEEP=y` in both defconfigs
- Add `wakeup-source;` to the kscan node in both board dts files
- Verify: halves sleep, wake on keypress, re-pair cleanly, battery indicator still behaves

## 4. Charge indication (stretch)

- CW2015 is read-only (no charge control possible — no charger IC interface found)
- Optional: low-battery blink / charging-state colors if a charging source can be detected
  (USB connected = `zmk_usb_conn_state_changed`)

## Done

- [x] Board port to upstream ZMK (Zephyr 4.1, HWMv2)
- [x] AW20216S + CW2015 out-of-tree drivers
- [x] ZMK Studio (physical layout with rotated thumbs)
- [x] `&bootl` disassembly-free UF2 flashing on both halves
- [x] Persistent split pairing (settings on both halves)
- [x] settings_reset images in CI
- [x] Per-half bootloaders (`AM_HATSU_L`/`AM_HATSU_R`) at ashleymcdonald/Adafruit_nRF52_Bootloader
