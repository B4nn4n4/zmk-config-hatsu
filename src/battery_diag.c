/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Diagnostic helper: periodically logs the fuel gauge's cell voltage (VCELL)
 * and state of charge. This lets us verify whether the cell voltage is actually
 * rising while a battery gauge SOC reads frozen (e.g. CW2015 that was never
 * configured with its age/voltage-curve table).
 *
 * The reads MUST run from a work-queue (thread) context, not a raw timer
 * callback: the CW2015 driver refuses I2C reads when k_is_in_isr() is true
 * (returns -EWOULDBLOCK), and taking its internal semaphore with K_FOREVER is
 * invalid from an ISR. A delayable work item scheduled on the system workqueue
 * runs in thread context, so VCELL actually gets read.
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
#include <zmk/battery.h>

LOG_MODULE_REGISTER(battery_diag, LOG_LEVEL_INF);

#if IS_ENABLED(CONFIG_ZMK_BATTERY_DIAG)

BUILD_ASSERT(DT_HAS_CHOSEN(zmk_battery),
             "CONFIG_ZMK_BATTERY_DIAG requires a zmk,battery chosen node");

static const struct device *const diag_battery = DEVICE_DT_GET(DT_CHOSEN(zmk_battery));

static void battery_diag_work(struct k_work *work);

K_WORK_DELAYABLE_DEFINE(diag_work, battery_diag_work);

static void battery_diag_work(struct k_work *work) {
    struct sensor_value vcell;
    struct sensor_value soc;

    if (sensor_sample_fetch_chan(diag_battery, SENSOR_CHAN_GAUGE_VOLTAGE) != 0) {
        goto reschedule;
    }
    if (sensor_channel_get(diag_battery, SENSOR_CHAN_GAUGE_VOLTAGE, &vcell) != 0) {
        goto reschedule;
    }

    if (sensor_sample_fetch_chan(diag_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE) != 0) {
        goto reschedule;
    }
    if (sensor_channel_get(diag_battery, SENSOR_CHAN_GAUGE_STATE_OF_CHARGE, &soc) != 0) {
        goto reschedule;
    }

    LOG_INF("diag vcell=%d.%06d V soc=%d.%06d%% (zmk=%d%%)", vcell.val1, vcell.val2, soc.val1,
            soc.val2 / 10000, zmk_battery_state_of_charge());

reschedule:
    k_work_schedule(&diag_work, K_MSEC(CONFIG_ZMK_BATTERY_DIAG_INTERVAL_MS));
}

static int battery_diag_init(void) {
    if (!device_is_ready(diag_battery)) {
        LOG_ERR("Battery device not ready");
        return -ENODEV;
    }
    k_work_schedule(&diag_work, K_MSEC(1000));
    return 0;
}

SYS_INIT(battery_diag_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif /* IS_ENABLED(CONFIG_ZMK_BATTERY_DIAG) */
