/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Diagnostic helper: periodically logs the fuel gauge's cell voltage (VCELL)
 * and state of charge. This lets us verify whether the cell voltage is actually
 * rising while a battery gauge SOC reads frozen (e.g. CW2015 that was never
 * configured with its age/voltage-curve table).
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include <zmk/battery.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_BATTERY_DIAG)

BUILD_ASSERT(DT_HAS_CHOSEN(zmk_battery),
             "CONFIG_ZMK_BATTERY_DIAG requires a zmk,battery chosen node");

static const struct device *const diag_battery = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

static void battery_diag_tick(struct k_timer *timer) {
    struct sensor_value vcell;
    struct sensor_value soc;

    if (sensor_sample_fetch_chan(diag_battery, SENSOR_CHAN_GAUGE_VOLTAGE) != 0) {
        return;
    }
    if (sensor_channel_get(diag_battery, SENSOR_CHAN_GAUGE_VOLTAGE, &vcell) != 0) {
        return;
    }

    if (sensor_sample_fetch_chan(diag_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE) != 0) {
        return;
    }
    if (sensor_channel_get(diag_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &soc) != 0) {
        return;
    }

    LOG_INF("diag soc=%d%% vcell=%d.%06d V zsoc=%d%%", soc.val1, vcell.val1, vcell.val2,
            zmk_battery_state_of_charge());
}

K_TIMER_DEFINE(battery_diag_timer, battery_diag_tick, NULL);

static int battery_diag_init(void) {
    if (!device_is_ready(diag_battery)) {
        LOG_ERR("Battery device not ready");
        return -ENODEV;
    }
    k_timer_start(&battery_diag_timer, K_MSEC(1000), K_MSEC(CONFIG_ZMK_BATTERY_DIAG_INTERVAL_MS));
    return 0;
}

SYS_INIT(battery_diag_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* IS_ENABLED(CONFIG_ZMK_BATTERY_DIAG) */
