// ---------------------------------------------------------------------------------
// Sensor registration routines
// ---------------------------------------------------------------------------------
// Routines for managing device sensors
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------

#include "sensors.h"
#include "build.h"
#include "pins.h"
#include "vita40/vita40.h"
#include "snon/snon_utils.h"
#include "pico-utils/ws2812.h"

#include "mem_utils.h"

#define INIT_VALUE              "vita40_white_steady"
#define FRONT_PANEL_LED_COUNT   9 * 30

uint32_t        led_update_counter = 0;
extern uint     pio_sm;
extern uint     pio_sm_offset;

void sensors_initialize_device(void)
{
    snon_initialize("1870A Annunciator");
    snon_add_relationship("Entities", SNON_REL_CHILD_OF, "Device");

    // -------------------------
    // Device Time Sensors

    // Add measurands
    snon_register("Time Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"iso8601\",\"meAq\":\"count\"}");
    snon_register("Seconds Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"s\",\"meT\":\"numeric\",\"meAq\":\"count\",\"meUS\":{\"*\":\"s\"},\"meUSx\":{\"*\":\"seconds\"},\"meR\":\"1\",\"meAc\":\"1\"}");

    // Add sensors
    snon_register("Device Time Sensor", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("Device Time Sensor", SNON_REL_CHILD_OF, "Device");
    snon_add_relationship("Device Time Sensor", SNON_REL_MEASURAND, "Time Measurand");

    snon_register("Device Uptime Sensor", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("Device Uptime Sensor", SNON_REL_CHILD_OF, "Device");
    snon_add_relationship("Device Uptime Sensor", SNON_REL_MEASURAND, "Seconds Measurand");

    // Add series
    snon_register("Device Time Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("Device Time Series", SNON_REL_MEASURAND, "Time Measurand");
    snon_add_relationship("Device Time Series", SNON_REL_CHILD_OF, "Device Time Sensor");
    snon_add_relationship("Device Time Series", SNON_REL_VALUES, "Device Time");

    snon_register("Device Uptime Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("Device Uptime Series", SNON_REL_MEASURAND, "Seconds Measurand");
    snon_add_relationship("Device Uptime Series", SNON_REL_CHILD_OF, "Device Uptime Sensor");
    snon_add_relationship("Device Uptime Series", SNON_REL_VALUES, "Device Uptime");

    // Add special values
    snon_register("Device Time", SNON_CLASS_VALUE, NULL);
    snon_register("Device Uptime", SNON_CLASS_VALUE, NULL);

    // -------------------------
    // Device Debug Sensors

    // Add values 
    snon_register("Debug LED RGB", SNON_CLASS_VALUE, NULL);
    snon_add_relationship("Debug LED RGB", SNON_REL_CHILD_OF, "Device");
    snon_set_value("Debug LED RGB", "0A000A");

    // -------------------------
    // Firmware version
    snon_register("Firmware Version", SNON_CLASS_VALUE, NULL);
    snon_add_relationship("Firmware Version", SNON_REL_CHILD_OF, "Device");
    snon_set_value("Firmware Version", FW_VERSION);
}

void sensors_initialize_hmi(void)
{
    snon_register("=P01", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=P01", SNON_REL_CHILD_OF, "Device");

    // Add front panel LED indicator values
    snon_register_81346("=P01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA02", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA03", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA04", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA05", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA06", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA07", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA08", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA09", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA10", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA11", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA12", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA13", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA14", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=P01=PFA15", SNON_CLASS_VALUE, INIT_VALUE);
}

bool draw_anc_leds(struct repeating_timer *t)
{
    uint32_t    led_values[FRONT_PANEL_LED_COUNT];
    uint32_t    led_value = 0;
    uint16_t    led_counter = 0;
    uint16_t    anc_counter = 0;

    // Update LED values
    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA01"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA02"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;
    
    led_value = vita40_to_urgb(snon_get_value("=P01=PFA03"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA04"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA05"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA06"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA07"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA08"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA09"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA10"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA11"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA12"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA13"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA14"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;

    led_value = vita40_to_urgb(snon_get_value("=P01=PFA15"), led_update_counter);

    while(led_counter != anc_counter)
    {
        led_values[led_counter++] = led_value;
    }

    anc_counter = anc_counter + 18;


    // Set LED values
    ws2812_program_init(pio1, pio_sm, pio_sm_offset, DISPLAY_PIN, 800000, false);

    led_counter = 0;
    while(led_counter < (FRONT_PANEL_LED_COUNT))
    {
        put_pixel(led_values[led_counter]);

        led_counter = led_counter + 1;
    }

    busy_wait_us(200);

    led_update_counter = led_update_counter + 1;
}
