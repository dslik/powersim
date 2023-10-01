// ---------------------------------------------------------------------------------
// Sensor registration routines
// ---------------------------------------------------------------------------------
// Routines for managing sensors on various power simulator front panels
// ---------------------------------------------------------------------------------
// SPDX-FileCopyrightText: Copyright 2023 David Slik (VE7FIM)
// SPDX-FileAttributionText: https://github.com/dslik/powersim/
// SPDX-License-Identifier: CERN-OHL-S-2.0
// ---------------------------------------------------------------------------------
#include "build.h"
#include "asm_hmi.h"
#include "vita40.h"
#include "snon/snon_utils.h"

#include "mem_utils.h"

#define	INIT_VALUE	"vita40_green_standby"

void sensors_initialize(void)
{
    snon_initialize("1840A Edge Display");
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

    // -------------------------
    // Device Current Sensors

    // Add measurands
    snon_register("Current Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"A\",\"meT\":\"numeric\",\"meAq\":\"sample\"}");

    // Add current sensor
    snon_register("=W01=PGC01", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("=W01=PGC01", SNON_REL_CHILD_OF, "=W01");
    snon_add_relationship("=W01=PGC01", SNON_REL_MEASURAND, "Current Measurand");

    // -------------------------
    // Add L1 current series
    snon_register("L1 Current Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current Series", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current Series", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current Series", SNON_REL_SETPOINT, "L1 Current Setpoint");
    snon_add_relationship("L1 Current Series", SNON_REL_ALARMS, "L1 Current LoLo Alarm");
    snon_add_relationship("L1 Current Series", SNON_REL_ALARMS, "L1 Current Lo Alarm");
    snon_add_relationship("L1 Current Series", SNON_REL_ALARMS, "L1 Current Hi Alarm");
    snon_add_relationship("L1 Current Series", SNON_REL_ALARMS, "L1 Current HiHi Alarm");
    snon_add_relationship("L1 Current Series", SNON_REL_VALUES, "L1 Current");

    snon_register("L1 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current", "[\"0\"]");

    // Add setpoint series
    snon_register("L1 Current Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current Setpoint", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current Setpoint", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current Setpoint", SNON_REL_VALUES, "L1 Current SP");

    snon_register("L1 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L1 Current LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current LoLo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current LoLo Alarm", SNON_REL_VALUES, "L1 Current LoLo");

    snon_register("L1 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L1 Current Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current Lo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current Lo Alarm", SNON_REL_VALUES, "L1 Current Lo");

    snon_register("L1 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L1 Current Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current Hi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current Hi Alarm", SNON_REL_VALUES, "L1 Current Hi");

    snon_register("L1 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L1 Current HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Current HiHi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L1 Current HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L1 Current HiHi Alarm", SNON_REL_VALUES, "L1 Current HiHi");

    snon_register("L1 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Current HiHi", "[\"2\"]");

    // -------------------------
    // Add L2 current series
    snon_register("L2 Current Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current Series", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current Series", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current Series", SNON_REL_SETPOINT, "L2 Current Setpoint");
    snon_add_relationship("L2 Current Series", SNON_REL_ALARMS, "L2 Current LoLo Alarm");
    snon_add_relationship("L2 Current Series", SNON_REL_ALARMS, "L2 Current Lo Alarm");
    snon_add_relationship("L2 Current Series", SNON_REL_ALARMS, "L2 Current Hi Alarm");
    snon_add_relationship("L2 Current Series", SNON_REL_ALARMS, "L2 Current HiHi Alarm");
    snon_add_relationship("L2 Current Series", SNON_REL_VALUES, "L2 Current");

    snon_register("L2 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current", "[\"0\"]");

    // Add setpoint series
    snon_register("L2 Current Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current Setpoint", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current Setpoint", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current Setpoint", SNON_REL_VALUES, "L2 Current SP");

    snon_register("L2 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L2 Current LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current LoLo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current LoLo Alarm", SNON_REL_VALUES, "L2 Current LoLo");

    snon_register("L2 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L2 Current Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current Lo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current Lo Alarm", SNON_REL_VALUES, "L2 Current Lo");

    snon_register("L2 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L2 Current Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current Hi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current Hi Alarm", SNON_REL_VALUES, "L2 Current Hi");

    snon_register("L2 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L2 Current HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Current HiHi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L2 Current HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L2 Current HiHi Alarm", SNON_REL_VALUES, "L2 Current HiHi");

    snon_register("L2 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Current HiHi", "[\"2\"]");

    // -------------------------
    // Add L3 current series
    snon_register("L3 Current Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current Series", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current Series", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current Series", SNON_REL_SETPOINT, "L3 Current Setpoint");
    snon_add_relationship("L3 Current Series", SNON_REL_ALARMS, "L3 Current LoLo Alarm");
    snon_add_relationship("L3 Current Series", SNON_REL_ALARMS, "L3 Current Lo Alarm");
    snon_add_relationship("L3 Current Series", SNON_REL_ALARMS, "L3 Current Hi Alarm");
    snon_add_relationship("L3 Current Series", SNON_REL_ALARMS, "L3 Current HiHi Alarm");
    snon_add_relationship("L3 Current Series", SNON_REL_VALUES, "L3 Current");

    snon_register("L3 Current", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current", "[\"0\"]");

    // Add setpoint series
    snon_register("L3 Current Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current Setpoint", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current Setpoint", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current Setpoint", SNON_REL_VALUES, "L3 Current SP");

    snon_register("L3 Current SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L3 Current LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current LoLo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current LoLo Alarm", SNON_REL_VALUES, "L3 Current LoLo");

    snon_register("L3 Current LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L3 Current Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current Lo Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current Lo Alarm", SNON_REL_VALUES, "L3 Current Lo");

    snon_register("L3 Current Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L3 Current Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current Hi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current Hi Alarm", SNON_REL_VALUES, "L3 Current Hi");

    snon_register("L3 Current Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L3 Current HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Current HiHi Alarm", SNON_REL_MEASURAND, "Current Measurand");
    snon_add_relationship("L3 Current HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGC01");
    snon_add_relationship("L3 Current HiHi Alarm", SNON_REL_VALUES, "L3 Current HiHi");

    snon_register("L3 Current HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Current HiHi", "[\"2\"]");

    // -------------------------
    // Device Voltage Sensors

    // Add measurands
    snon_register("Voltage Measurand", SNON_CLASS_MEASURAND, "{\"meU\":\"V\",\"meT\":\"numeric\",\"meAq\":\"sample\"}");

    // Add Voltage sensor
    snon_register("=W01=PGA01", SNON_CLASS_SENSOR, NULL);
    snon_add_relationship("=W01=PGA01", SNON_REL_CHILD_OF, "=W01");
    snon_add_relationship("=W01=PGA01", SNON_REL_MEASURAND, "Voltage Measurand");

    // -------------------------
    // Add L1 Voltage series
    snon_register("L1 Voltage Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage Series", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage Series", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage Series", SNON_REL_SETPOINT, "L1 Voltage Setpoint");
    snon_add_relationship("L1 Voltage Series", SNON_REL_ALARMS, "L1 Voltage LoLo Alarm");
    snon_add_relationship("L1 Voltage Series", SNON_REL_ALARMS, "L1 Voltage Lo Alarm");
    snon_add_relationship("L1 Voltage Series", SNON_REL_ALARMS, "L1 Voltage Hi Alarm");
    snon_add_relationship("L1 Voltage Series", SNON_REL_ALARMS, "L1 Voltage HiHi Alarm");
    snon_add_relationship("L1 Voltage Series", SNON_REL_VALUES, "L1 Voltage");

    snon_register("L1 Voltage", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage", "[\"0\"]");

    // Add setpoint series
    snon_register("L1 Voltage Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage Setpoint", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage Setpoint", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage Setpoint", SNON_REL_VALUES, "L1 Voltage SP");

    snon_register("L1 Voltage SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L1 Voltage LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage LoLo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage LoLo Alarm", SNON_REL_VALUES, "L1 Voltage LoLo");

    snon_register("L1 Voltage LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L1 Voltage Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage Lo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage Lo Alarm", SNON_REL_VALUES, "L1 Voltage Lo");

    snon_register("L1 Voltage Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L1 Voltage Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage Hi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage Hi Alarm", SNON_REL_VALUES, "L1 Voltage Hi");

    snon_register("L1 Voltage Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L1 Voltage HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L1 Voltage HiHi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L1 Voltage HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L1 Voltage HiHi Alarm", SNON_REL_VALUES, "L1 Voltage HiHi");

    snon_register("L1 Voltage HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L1 Voltage HiHi", "[\"2\"]");

    // -------------------------
    // Add L2 Voltage series
    snon_register("L2 Voltage Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage Series", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage Series", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage Series", SNON_REL_SETPOINT, "L2 Voltage Setpoint");
    snon_add_relationship("L2 Voltage Series", SNON_REL_ALARMS, "L2 Voltage LoLo Alarm");
    snon_add_relationship("L2 Voltage Series", SNON_REL_ALARMS, "L2 Voltage Lo Alarm");
    snon_add_relationship("L2 Voltage Series", SNON_REL_ALARMS, "L2 Voltage Hi Alarm");
    snon_add_relationship("L2 Voltage Series", SNON_REL_ALARMS, "L2 Voltage HiHi Alarm");
    snon_add_relationship("L2 Voltage Series", SNON_REL_VALUES, "L2 Voltage");

    snon_register("L2 Voltage", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage", "[\"0\"]");

    // Add setpoint series
    snon_register("L2 Voltage Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage Setpoint", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage Setpoint", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage Setpoint", SNON_REL_VALUES, "L2 Voltage SP");

    snon_register("L2 Voltage SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L2 Voltage LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage LoLo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage LoLo Alarm", SNON_REL_VALUES, "L2 Voltage LoLo");

    snon_register("L2 Voltage LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L2 Voltage Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage Lo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage Lo Alarm", SNON_REL_VALUES, "L2 Voltage Lo");

    snon_register("L2 Voltage Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L2 Voltage Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage Hi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage Hi Alarm", SNON_REL_VALUES, "L2 Voltage Hi");

    snon_register("L2 Voltage Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L2 Voltage HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L2 Voltage HiHi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L2 Voltage HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L2 Voltage HiHi Alarm", SNON_REL_VALUES, "L2 Voltage HiHi");

    snon_register("L2 Voltage HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L2 Voltage HiHi", "[\"2\"]");

    // -------------------------
    // Add L3 Voltage series
    snon_register("L3 Voltage Series", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage Series", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage Series", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage Series", SNON_REL_SETPOINT, "L3 Voltage Setpoint");
    snon_add_relationship("L3 Voltage Series", SNON_REL_ALARMS, "L3 Voltage LoLo Alarm");
    snon_add_relationship("L3 Voltage Series", SNON_REL_ALARMS, "L3 Voltage Lo Alarm");
    snon_add_relationship("L3 Voltage Series", SNON_REL_ALARMS, "L3 Voltage Hi Alarm");
    snon_add_relationship("L3 Voltage Series", SNON_REL_ALARMS, "L3 Voltage HiHi Alarm");
    snon_add_relationship("L3 Voltage Series", SNON_REL_VALUES, "L3 Voltage");

    snon_register("L3 Voltage", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage", "[\"0\"]");

    // Add setpoint series
    snon_register("L3 Voltage Setpoint", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage Setpoint", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage Setpoint", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage Setpoint", SNON_REL_VALUES, "L3 Voltage SP");

    snon_register("L3 Voltage SP", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage SP", "[\"0\"]");

    // Add Lo Lo alarm series
    snon_register("L3 Voltage LoLo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage LoLo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage LoLo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage LoLo Alarm", SNON_REL_VALUES, "L3 Voltage LoLo");

    snon_register("L3 Voltage LoLo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage LoLo", "[\"-2\"]");

    // Add Lo alarm series
    snon_register("L3 Voltage Lo Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage Lo Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage Lo Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage Lo Alarm", SNON_REL_VALUES, "L3 Voltage Lo");

    snon_register("L3 Voltage Lo", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage Lo", "[\"-1\"]");

    // Add Hi alarm series
    snon_register("L3 Voltage Hi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage Hi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage Hi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage Hi Alarm", SNON_REL_VALUES, "L3 Voltage Hi");

    snon_register("L3 Voltage Hi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage Hi", "[\"1\"]");

    // Add Hi Hi alarm series
    snon_register("L3 Voltage HiHi Alarm", SNON_CLASS_SERIES, NULL);
    snon_add_relationship("L3 Voltage HiHi Alarm", SNON_REL_MEASURAND, "Voltage Measurand");
    snon_add_relationship("L3 Voltage HiHi Alarm", SNON_REL_CHILD_OF, "=W01=PGA01");
    snon_add_relationship("L3 Voltage HiHi Alarm", SNON_REL_VALUES, "L3 Voltage HiHi");

    snon_register("L3 Voltage HiHi", SNON_CLASS_VALUE, NULL);
    snon_set_values("L3 Voltage HiHi", "[\"2\"]");

    // Add front panel LED indicator values
    snon_register("=W01", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W01", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W01=PHA01", SNON_CLASS_VALUE, "WAITING");

    snon_register_81346("=W01=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W01=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W01=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W01=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W01=WBA01=PFA02", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA02=PFA02", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA03=PFA02", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W01=WBA01=PFA03", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA02=PFA03", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W01=WBA03=PFA03", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q21", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q21", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q21=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q21=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q21=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q21=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q21=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q21=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q21=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W02", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W02", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W02=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W02=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W02=WBA01=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA02=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA03=PGA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W02=WBA01=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA02=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA03=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W02=WBA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA02=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W02=WBA03=PGC01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W02=WBA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA02=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W02=WBA03=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q20", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q20", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q20=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q20=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q20=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q20=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q20=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q20=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q20=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W03", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W03", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W03=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W03=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W03=WBA01=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA02=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA03=PGA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W03=WBA01=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA02=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA03=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W03=WBA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA02=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W03=WBA03=PGC01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W03=WBA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA02=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W03=WBA03=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q22", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q22", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q22=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q22=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q22=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q22=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q22=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q22=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q22=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W04", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W04", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W04=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W04=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W04=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W04=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W04=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W04=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=T01", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=T01", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=T01=TAA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=T01=TAA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=T01=TAA01=PFA02", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=T01=TAA01=PFA03", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W05", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W05", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W05=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W05=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W05=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W05=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W05=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W05=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q12", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q12", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q12=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q12=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q12=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q12=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q12=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q12=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q12=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W06", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W06", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W06=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W06=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W06=WBA01=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA02=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA03=PGA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W06=WBA01=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA02=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA03=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W06=WBA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA02=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W06=WBA03=PGC01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W06=WBA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA02=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W06=WBA03=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q10", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q10", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q10=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q10=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q10=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q10=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q10=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q10=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q10=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W07", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W07", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W07=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W07=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W07=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W07=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W07=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W07=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=Q11", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=Q11", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=Q11=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=Q11=QBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q11=QBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=Q11=QBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=Q11=QBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q11=QBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=Q11=QBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W08", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W08", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W08=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W08=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W08=WBA01=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA02=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA03=PGA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W08=WBA01=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA02=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA03=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W08=WBA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA02=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W08=WBA03=PGC01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W08=WBA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA02=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W08=WBA03=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=G01", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=G01", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=G01=GAA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=G01=GAA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=G01=GAA01=PFA02", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=G01=GAA01=PFA03", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=G01=GAA01=RBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=G01=GAA01=RBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=G01=T02", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=G01=T02=TAA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=G01=T02=TAA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=G01=T02=TAA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=G01=T02=TAA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=G01=T02=TAA01=RAC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=G01=T02=TAA01=RAC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W09", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W09", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W09=WBA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA02", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA03", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W09=WBA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA02=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA03=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W09=WBA01=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA02=PGA01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA03=PGA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W09=WBA01=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA02=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA03=PGA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register_81346("=W09=WBA01=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA02=PGC01", SNON_CLASS_DEVICE, NULL);
    snon_register_81346("=W09=WBA03=PGC01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W09=WBA01=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA02=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);
    snon_register_81346("=W09=WBA03=PGC01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

    snon_register("=W10", SNON_CLASS_DEVICE, NULL);
    snon_add_relationship("=W10", SNON_REL_CHILD_OF, "Device");

    snon_register_81346("=W10=WEA01", SNON_CLASS_DEVICE, NULL);

    snon_register_81346("=W10=WEA01=PFA01", SNON_CLASS_VALUE, INIT_VALUE);

}