# Board port TODO
#
# Port angry_miao_hatsu_left / angry_miao_hatsu_right here from
# https://github.com/ashleymcdonald/MiaoBreak (zmk/app/boards/arm/angry_miao_hatsu),
# updated for current ZMK/Zephyr:
#  - kscan gpio matrix + matrix transform (mostly unchanged)
#  - flash partitions (keep Adafruit bootloader layout: code@0x1000, storage@0xd4000, boot@0xf4000)
#  - AW20216S LED driver node (SPI) -> out-of-tree module driver
#  - CW2015 fuel gauge node (I2C) -> out-of-tree module driver
#  - CONFIG_SETTINGS on BOTH halves (bug in original: left lacked it, bonds lost on reset)
#  - CONFIG_ZMK_STUDIO=y
